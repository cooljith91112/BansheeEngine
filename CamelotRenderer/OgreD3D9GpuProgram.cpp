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
#include "OgreD3D9GpuProgram.h"
#include "OgreMatrix4.h"
#include "OgreException.h"
#include "OgreD3D9Mappings.h"
#include "OgreD3D9RenderSystem.h"

namespace Ogre {
    //-----------------------------------------------------------------------------
    D3D9GpuProgram::D3D9GpuProgram() 
        : GpuProgram(), mpExternalMicrocode(NULL), mColumnMajorMatrices(false)
    {
    }

	//-----------------------------------------------------------------------------
	D3D9GpuProgram::~D3D9GpuProgram()
	{

	}
    
	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::setExternalMicrocode(const void* pMicrocode, size_t size)
	{
		LPD3DXBUFFER pBuffer=0;
		HRESULT hr=D3DXCreateBuffer(size, &pBuffer);
		if(pBuffer)
		{
			memcpy(pBuffer->GetBufferPointer(), pMicrocode, size);
			this->setExternalMicrocode(pBuffer);
			SAFE_RELEASE(pBuffer);
		}
	}
	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::setExternalMicrocode(ID3DXBuffer* pMicrocode)
	{ 
		SAFE_RELEASE(mpExternalMicrocode);
		mpExternalMicrocode = pMicrocode;
		if(mpExternalMicrocode)
		{
			mpExternalMicrocode->AddRef();	
		}
	}
    //-----------------------------------------------------------------------------
	LPD3DXBUFFER D3D9GpuProgram::getExternalMicrocode(void)
	{
		return mpExternalMicrocode;
	}

	//-----------------------------------------------------------------------------
    void D3D9GpuProgram::load(void)
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		for (uint i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
		{
			IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);

			load(d3d9Device);
		}		       
    }

	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::load(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (mpExternalMicrocode)
		{
			loadFromMicrocode(d3d9Device, mpExternalMicrocode);
		}
		else
		{
			// Call polymorphic load
			loadFromSource(d3d9Device);
		}
	}
	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::unload(void)
	{
		SAFE_RELEASE(mpExternalMicrocode);
	}
	//-----------------------------------------------------------------------------
    void D3D9GpuProgram::loadFromSource(void)
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		for (uint i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
		{
			IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);

			loadFromSource(d3d9Device);
		}
    }

	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::loadFromSource(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION


		// Populate compile flags
        DWORD compileFlags = 0;

		// Create the shader
		// Assemble source into microcode
		LPD3DXBUFFER microcode;
		LPD3DXBUFFER errors;
		HRESULT hr = D3DXAssembleShader(
			mSource.c_str(),
			static_cast<UINT>(mSource.length()),
			NULL,               // no #define support
			NULL,               // no #include support
			compileFlags,       // standard compile options
			&microcode,
			&errors);

		if (FAILED(hr))
		{
			String message = "Cannot assemble D3D9 shader. Errors:\n";
			message += static_cast<const char*>(errors->GetBufferPointer());

			errors->Release();
			OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, message,
				"D3D9GpuProgram::loadFromSource");
		}
	
		loadFromMicrocode(d3d9Device, microcode);		

		SAFE_RELEASE(microcode);
		SAFE_RELEASE(errors);
	}
    //-----------------------------------------------------------------------
    GpuProgramParametersSharedPtr D3D9GpuProgram::createParameters(void)
    {
        // Call superclass
        GpuProgramParametersSharedPtr params = GpuProgram::createParameters();

        // Need to transpose matrices if compiled with column-major matrices
        params->setTransposeMatrices(mColumnMajorMatrices);

        return params;
    }	
	//-----------------------------------------------------------------------------
    D3D9GpuVertexProgram::D3D9GpuVertexProgram() 
        : D3D9GpuProgram()       
    {
        mType = GPT_VERTEX_PROGRAM;		
    }
	//-----------------------------------------------------------------------------
	D3D9GpuVertexProgram::~D3D9GpuVertexProgram()
	{	
	}
	//-----------------------------------------------------------------------------
    void D3D9GpuVertexProgram::loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode)
    {		 
		DeviceToVertexShaderIterator it = mMapDeviceToVertexShader.find(d3d9Device);

		if (it != mMapDeviceToVertexShader.end())
			SAFE_RELEASE(it->second);

		if (isSupported())
		{
			// Create the shader
			IDirect3DVertexShader9* pVertexShader;
			HRESULT hr;
			
			hr = d3d9Device->CreateVertexShader( 
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				&pVertexShader);

			if (FAILED(hr))
			{
				OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Cannot create D3D9 vertex shader from microcode",
					"D3D9GpuVertexProgram::loadFromMicrocode");
	            
			}

			mMapDeviceToVertexShader[d3d9Device] = pVertexShader;
		}
		else
		{
			OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
				"Specified D3D9 vertex shader is not supported!",
				"D3D9GpuVertexProgram::loadFromMicrocode");

			// TODO LOG PORT - Log this error somewhere
			//LogManager::getSingleton().logMessage(
			//	"Unsupported D3D9 vertex shader '" + mName + "' was not loaded.");

			mMapDeviceToVertexShader[d3d9Device] = NULL;
		}
    }
	//-----------------------------------------------------------------------------
    void D3D9GpuVertexProgram::unload(void)
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

        DeviceToVertexShaderIterator it = mMapDeviceToVertexShader.begin();

		while (it != mMapDeviceToVertexShader.end())
		{
			SAFE_RELEASE(it->second);
			++it;
		}
		mMapDeviceToVertexShader.clear();		
		D3D9GpuProgram::unload();
    }

	//-----------------------------------------------------------------------------
	void D3D9GpuVertexProgram::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		
	}

	//-----------------------------------------------------------------------------
	void D3D9GpuVertexProgram::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToVertexShaderIterator it;

		// Find the shader of this device.
		it = mMapDeviceToVertexShader.find(d3d9Device);

		// Case shader found -> release it and erase from map.
		if (it != mMapDeviceToVertexShader.end())
		{
			SAFE_RELEASE(it->second);
			mMapDeviceToVertexShader.erase(it);
		}
	}

	//-----------------------------------------------------------------------------
	IDirect3DVertexShader9* D3D9GpuVertexProgram::getVertexShader( void )
	{
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		DeviceToVertexShaderIterator it;

		// Find the shader of this device.
		it = mMapDeviceToVertexShader.find(d3d9Device);
		
		// Shader was not found -> load it.
		if (it == mMapDeviceToVertexShader.end())		
		{
			load(d3d9Device);		
			it = mMapDeviceToVertexShader.find(d3d9Device);
		}
	
		return it->second;
	}

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
    D3D9GpuFragmentProgram::D3D9GpuFragmentProgram() 
        : D3D9GpuProgram()       
    {
        mType = GPT_FRAGMENT_PROGRAM;
    }
	//-----------------------------------------------------------------------------
	D3D9GpuFragmentProgram::~D3D9GpuFragmentProgram()
	{
	}
	//-----------------------------------------------------------------------------
    void D3D9GpuFragmentProgram::loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode)
    {
		DeviceToPixelShaderIterator it = mMapDeviceToPixelShader.find(d3d9Device);

		if (it != mMapDeviceToPixelShader.end())
			SAFE_RELEASE(it->second);

		if (isSupported())
		{
			// Create the shader
			IDirect3DPixelShader9* pPixelShader;
			HRESULT hr;

			hr = d3d9Device->CreatePixelShader(
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				&pPixelShader);

			if (FAILED(hr))
			{
				OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Cannot create D3D9 pixel shader from microcode.",
					"D3D9GpuFragmentProgram::loadFromMicrocode");
	            
			}

			mMapDeviceToPixelShader[d3d9Device] = pPixelShader;
		}
		else
		{
			// TODO LOG PORT - Log this somewhere
			//LogManager::getSingleton().logMessage(
			//	"Unsupported D3D9 pixel shader was not loaded.");

			mMapDeviceToPixelShader[d3d9Device] = NULL;
		}
    }
	//-----------------------------------------------------------------------------
    void D3D9GpuFragmentProgram::unload(void)
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToPixelShaderIterator it = mMapDeviceToPixelShader.begin();

		while (it != mMapDeviceToPixelShader.end())
		{
			SAFE_RELEASE(it->second);
			++it;
		}
		mMapDeviceToPixelShader.clear();	
		D3D9GpuProgram::unload();
    }
	//-----------------------------------------------------------------------------
	void D3D9GpuFragmentProgram::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		
	}

	//-----------------------------------------------------------------------------
	void D3D9GpuFragmentProgram::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToPixelShaderIterator it;

		// Find the shader of this device.
		it = mMapDeviceToPixelShader.find(d3d9Device);

		// Case shader found -> release it and erase from map.
		if (it != mMapDeviceToPixelShader.end())
		{
			SAFE_RELEASE(it->second);
			mMapDeviceToPixelShader.erase(it);
		}
	}

	//-----------------------------------------------------------------------------
	IDirect3DPixelShader9* D3D9GpuFragmentProgram::getPixelShader( void )
	{
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		DeviceToPixelShaderIterator it;

		// Find the shader of this device.
		it = mMapDeviceToPixelShader.find(d3d9Device);

		// Shader was not found -> load it.
		if (it == mMapDeviceToPixelShader.end())		
		{
			load(d3d9Device);			
			it = mMapDeviceToPixelShader.find(d3d9Device);
		}

		return it->second;
	}
	//-----------------------------------------------------------------------------

}
