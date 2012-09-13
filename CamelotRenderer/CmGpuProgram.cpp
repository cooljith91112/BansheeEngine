/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmGpuProgram.h"
#include "CmHighLevelGpuProgram.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmRenderSystemCapabilities.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmRenderSystemManager.h"

namespace CamelotEngine
{
    //-----------------------------------------------------------------------------
    GpuProgram::GpuProgram() 
        :mType(GPT_VERTEX_PROGRAM),
		mCompileError(false), mProfile(GPP_NONE)
    {
		createParameterMappingStructures();
    }
    //-----------------------------------------------------------------------------
    void GpuProgram::setType(GpuProgramType t)
    {
        mType = t;
    }
    //-----------------------------------------------------------------------------
    void GpuProgram::setSyntaxCode(const String& syntax)
    {
        mSyntaxCode = syntax;
    }
    //-----------------------------------------------------------------------------
    void GpuProgram::setSource(const String& source)
    {
        mSource = source;
		mCompileError = false;
    }
		

    //-----------------------------------------------------------------------------
    void GpuProgram::load(void)
    {
        // Call polymorphic load
		try 
		{
			loadFromSource();

			assert(mDefaultParams == nullptr);

			mDefaultParams = createParameters();
		}
		catch (const Exception&)
		{
			// will already have been logged
			//LogManager::getSingleton().stream()
			//	<< "Gpu program " << mName << " encountered an error "
			//	<< "during loading and is thus not supported.";

			mCompileError = true;
		}

    }
    //-----------------------------------------------------------------------------
    bool GpuProgram::isSupported(void) const
    {
        if (mCompileError || !isRequiredCapabilitiesSupported())
            return false;

		RenderSystem* rs = CamelotEngine::RenderSystemManager::getActive();
		return rs->getCapabilities()->isShaderProfileSupported(mSyntaxCode);
    }
	//-----------------------------------------------------------------------------
	bool GpuProgram::isRequiredCapabilitiesSupported(void) const
	{
		return true;
	}
	//---------------------------------------------------------------------
	void GpuProgram::createParameterMappingStructures(bool recreateIfExists) const
	{
		createLogicalParameterMappingStructures(recreateIfExists);
		createNamedParameterMappingStructures(recreateIfExists);
	}
	//---------------------------------------------------------------------
	void GpuProgram::createLogicalParameterMappingStructures(bool recreateIfExists) const
	{
		if (recreateIfExists || (mFloatLogicalToPhysical == nullptr))
			mFloatLogicalToPhysical = GpuLogicalBufferStructPtr(new GpuLogicalBufferStruct());
		if (recreateIfExists || (mIntLogicalToPhysical == nullptr))
			mIntLogicalToPhysical = GpuLogicalBufferStructPtr(new GpuLogicalBufferStruct());
	}
	//---------------------------------------------------------------------
	void GpuProgram::createNamedParameterMappingStructures(bool recreateIfExists) const
	{
		if (recreateIfExists || (mConstantDefs == nullptr))
			mConstantDefs = GpuNamedConstantsPtr(new GpuNamedConstants());
	}
    //-----------------------------------------------------------------------------
    GpuProgramParametersSharedPtr GpuProgram::createParameters(void)
    {
        // Default implementation simply returns standard parameters.
        GpuProgramParametersSharedPtr ret = GpuProgramParametersSharedPtr(new GpuProgramParameters());	
		
		// set up named parameters, if any
		if ((mConstantDefs != nullptr) && !mConstantDefs->map.empty())
		{
			ret->_setNamedConstants(mConstantDefs);
		}
		// link shared logical / physical map for low-level use
		ret->_setLogicalIndexes(mFloatLogicalToPhysical, mIntLogicalToPhysical);

        // Copy in default parameters if present
        if (mDefaultParams != nullptr)
            ret->copyConstantsFrom(*(mDefaultParams.get()));
        
        return ret;
    }
    //-----------------------------------------------------------------------------
    GpuProgramParametersSharedPtr GpuProgram::getDefaultParameters(void)
    {
        if (mDefaultParams == nullptr)
        {
            mDefaultParams = createParameters();
        }
        return mDefaultParams;
    }

    //-----------------------------------------------------------------------
    const String& GpuProgram::getLanguage(void) const
    {
        static const String language = "asm";

        return language;
    }
}
