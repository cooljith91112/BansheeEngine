//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsRenderSystem.h"
#include "BsGLHardwareBufferManager.h"
#include "BsGLSLProgramFactory.h"
#include "BsVector4.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Implementation of a render system using OpenGL. Provides abstracted
	 *			access to various low level OpenGL methods.
	 */
    class BS_RSGL_EXPORT GLRenderSystem : public RenderSystem
    {
    public:
        GLRenderSystem();
        ~GLRenderSystem();

		/**
		 * @copydoc RenderSystem::getName()
		 */
        const String& getName() const;

		/**
		 * @copydoc RenderSystem::getShadingLanguageName()
		 */
		const String& getShadingLanguageName() const;

		/**
		 * @copydoc RenderSystem::setRenderTarget()
		 */
        void setRenderTarget(RenderTargetPtr target);

        /**
		 * @copydoc RenderSystem::setVertexBuffers()
		 */
		void setVertexBuffers(UINT32 index, VertexBufferPtr* buffers, UINT32 numBuffers);

		/**
		 * @copydoc RenderSystem::setIndexBuffer()
		 */
		void setIndexBuffer(const IndexBufferPtr& buffer);

		/**
		 * @copydoc RenderSystem::setVertexDeclaration()
		 */
		void setVertexDeclaration(VertexDeclarationPtr vertexDeclaration);

		/**
		 * @copydoc RenderSystem::setDrawOperation()
		 */
		void setDrawOperation(DrawOperationType op);

        /**
		 * @copydoc RenderSystem::setScissorRect()
		 */
        void setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom);

		/**
		 * @copydoc RenderSystem::setTexture()
		 */
        void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &tex);
        
		/**
		 * @copydoc RenderSystem::setSamplerState()
		 */
		void setSamplerState(GpuProgramType gptype, UINT16 unit, const SamplerStatePtr& state);

		/**
		 * @copydoc RenderSystem::setBlendState()
		 */
		void setBlendState(const BlendStatePtr& blendState);

		/**
		 * @copydoc RenderSystem::setRasterizerState()
		 */
		void setRasterizerState(const RasterizerStatePtr& rasterizerState);

		/**
		 * @copydoc RenderSystem::setDepthStencilState()
		 */
		void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue);

		/**
		 * @copydoc RenderSystem::setViewport()
		 */
		void setViewport(Viewport vp);

		/**
		 * @copydoc RenderSystem::bindGpuProgram()
		 */
		void bindGpuProgram(HGpuProgram prg);

		/**
		 * @copydoc RenderSystem::unbindGpuProgram()
		 */
		void unbindGpuProgram(GpuProgramType gptype);

		/**
		 * @copydoc RenderSystem::bindGpuParams()
		 */
		void bindGpuParams(GpuProgramType gptype, GpuParamsPtr params);

		/**
		 * @copydoc RenderSystem::beginFrame()
		 */
        void beginFrame(void);

		/**
		 * @copydoc RenderSystem::endFrame()
		 */
        void endFrame(void);

		/**
		 * @copydoc RenderSystem::draw()
		 */
		void draw(UINT32 vertexOffset, UINT32 vertexCount);

		/**
		 * @copydoc RenderSystem::drawIndexed()
		 */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount);

		/**
		 * @copydoc RenderSystem::clearRenderTarget()
		 */
		void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		/**
		 * @copydoc RenderSystem::clearViewport()
		 */
		void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

        /**
		 * @copydoc RenderSystem::getColorVertexElementType()
		 */
        VertexElementType getColorVertexElementType() const;

		/**
		 * @copydoc RenderSystem::getHorizontalTexelOffset()
		 */
        float getHorizontalTexelOffset();

		/**
		 * @copydoc RenderSystem::getVerticalTexelOffset()
		 */
        float getVerticalTexelOffset();

		/**
		 * @copydoc RenderSystem::getMinimumDepthInputValue()
		 */
        float getMinimumDepthInputValue();

		 /**
		 * @copydoc RenderSystem::getMaximumDepthInputValue()
		 */
        float getMaximumDepthInputValue();

		/**
		 * @copydoc RenderSystem::convertProjectionMatrix()
		 */
        void convertProjectionMatrix(const Matrix4& matrix,
            Matrix4& dest, bool forGpuProgram = false);

		/************************************************************************/
		/* 				Internal use by OpenGL RenderSystem only                */
		/************************************************************************/

		/**
		 * @brief	Query has the main context been initialized.
		 */
		bool _isContextInitialized() const { return mGLInitialised; }

		/**
		 * @brief	Returns main context. Caller must ensure the context has been initialized.
		 */
		GLContext* getMainContext() const { return mMainContext; } 

		/**
		 * @brief	Returns a support object you may use for creating
		 */
		GLSupport* getGLSupport() const { return mGLSupport; }

	protected:
		/**
		 * @copydoc	RenderSystem::initialize_internal().
		 */
		void initialize_internal(AsyncOp& asyncOp);

		/**
		 * @copydoc	RenderSystem::destroy_internal().
		 */
        void destroy_internal(void);

		/**
		* @brief	Call before doing a draw operation, this method sets everything up.
		*/
		void beginDraw();

		/**
		* @brief	Needs to accompany every beginDraw after you are done with a single draw operation.
		*/
		void endDraw();

		/**
		 * @brief	Clear a part of a render target.
		 */
		void clearArea(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0, const RectI& clearArea = RectI::EMPTY);

		/**
		 * @brief	Set up clip planes against which all geometry will get clipped.
		 */
		void setClipPlanesImpl(const PlaneList& clipPlanes);

		/**
		 * @brief	Set up a clip plane at a specific clip plane index. If enabled,
		 *			geometry will be clipped against the positive side of the plane.
		 *
		 * @note	Valid index range is [0, 5].
		 */
        void setClipPlane(UINT16 index, float A, float B, float C, float D);

		/**
		 * @brief	Enable or disable clipping against a clip plane at the specified index.
		 *
		 * @note	Valid index range is [0, 5].
		 */
        void enableClipPlane (UINT16 index, bool enable);

		/**
		* @brief	Changes the currently active texture unit. Any texture related operations
		*			will then be performed on this unit.
		*/
		bool activateGLTextureUnit(UINT16 unit);

		/**
		 * @brief	Changes the active GPU program.
		 */
		void setActiveProgram(GpuProgramType gptype, GLSLGpuProgramPtr program);

		/**
		 * @brief	Retrieves the active GPU program of the specified type.
		 */
		GLSLGpuProgramPtr getActiveProgram(GpuProgramType gptype) const;

		/**
		 * @brief	Converts Banshee blend mode to OpenGL blend mode.
		 */
		GLint getBlendMode(BlendFactor blendMode) const;

		/**
		* @brief	Converts Banshee texture addressing mode to OpenGL texture addressing mode.
		*/
		GLint getTextureAddressingMode(TextureAddressingMode tam) const;

		/**
		 * @brief	Gets a combined min/mip filter value usable by OpenGL from the currently
		 *			set min and mip filters.
		 */
		GLuint getCombinedMinMipFilter() const;

		/**
		* @brief	OpenGL shares all texture slots, but the engine prefers to keep textures
		* 			separate per-stage. This will convert texture unit that is set per stage
		* 			into a global texture unit usable by OpenGL.
		*/
		UINT32 getGLTextureUnit(GpuProgramType gptype, UINT32 unit);

		/**
		* @brief	OpenGL shares all buffer bindings, but the engine prefers to keep buffers
		* 			separate per-stage. This will convert block buffer binding that is set per stage
		* 			into a global block buffer binding usable by OpenGL.
		*/
		UINT32 getGLUniformBlockBinding(GpuProgramType gptype, UINT32 binding);

		/**
		* @brief	Returns the OpenGL specific mode used for drawing, depending on the
		* 			currently set draw operation;
		*/
		GLint getGLDrawMode() const;

		/**
		 * @brief	Creates render system capabilities that specify which features are
		 *			or aren't supported.
		 */
		RenderSystemCapabilities* createRenderSystemCapabilities() const;

		/**
		 * @brief	Finish initialization by setting up any systems dependant on render system
		 *			capabilities.
		 */
		void initFromCaps(RenderSystemCapabilities* caps);

		/**
		 * @brief	Switch the currently used OpenGL context. You will need to re-bind
		 *			any previously bound values manually. (e.g. textures, gpu programs and such)
		 */
		void switchContext(GLContext* context);

		/************************************************************************/
		/* 								Sampler states                     		*/
		/************************************************************************/

		/**
		 * @brief	Sets the texture addressing mode for a texture unit. This determines
		 *			how are UV address values outside of [0, 1] range handled when sampling
		 *			from texture.
		 */
        void setTextureAddressingMode(UINT16 stage, const UVWAddressingMode& uvw);

		/**
		 * @brief	Sets the texture border color for a texture unit. Border color
		 *			determines color returned by the texture sampler when border addressing mode
		 *			is used and texture address is outside of [0, 1] range.
		 */
        void setTextureBorderColor(UINT16 stage, const Color& color);

		/**
		 * @brief	Sets the mipmap bias value for a given texture unit. Bias allows
		 *			you to adjust the mipmap selection calculation. Negative values force a
		 *			larger mipmap to be used, and positive values smaller. Units are in values
		 *			of mip levels, so -1 means use a mipmap one level higher than default.
		 */
		void setTextureMipmapBias(UINT16 unit, float bias);

		/**
		 * @brief	Allows you to specify how is the texture bound to the specified texture unit filtered.
		 *			Different filter types are used for different situations like magnifying or minifying a texture.
		 */
        void setTextureFiltering(UINT16 unit, FilterType ftype, FilterOptions filter);

		/**
		 * @brief	Sets anisotropy value for the specified texture unit.
		 */
		void setTextureAnisotropy(UINT16 unit, UINT32 maxAnisotropy);

		/**
		 * @brief	Gets anisotropy value for the specified texture unit.
		 */
		GLfloat getCurrentAnisotropy(UINT16 unit);

		/************************************************************************/
		/* 								Blend states                      		*/
		/************************************************************************/

		/**
		 * @brief	Sets up blending mode that allows you to combine new pixels with pixels already in the render target.
		 *			Final pixel value = (renderTargetPixel * sourceFactor) op (pixel * destFactor).
		 */
		void setSceneBlending(BlendFactor sourceFactor, BlendFactor destFactor, BlendOperation op);

		/**
		* @brief	Sets up blending mode that allows you to combine new pixels with pixels already in the render target.
		*			Allows you to set up separate blend operations for alpha values.
		*	
		*			Final pixel value = (renderTargetPixel * sourceFactor) op (pixel * destFactor). (And the same for alpha)
		*/
		void setSceneBlending(BlendFactor sourceFactor, BlendFactor destFactor, BlendFactor sourceFactorAlpha, 
			BlendFactor destFactorAlpha, BlendOperation op, BlendOperation alphaOp);

		/**
		 * @brief	Sets alpha test that allows you to reject pixels that fail the comparison function
		 *			versus the provided reference value.
		 */
		void setAlphaTest(CompareFunction func, unsigned char value);

		/**
		 * @brief	Enable alpha to coverage. Alpha to coverage allows you to perform blending without needing 
		 *			to worry about order of rendering like regular blending does. It requires multi-sampling to 
		 *			be active in order to work, and you need to supply an alpha texture that determines object transparency.
		 */
		void setAlphaToCoverage(bool enabled);

		/**
		 * @brief	Enables or disables writing to certain color channels of the render target.
		 */
		void setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

		/************************************************************************/
		/* 								Rasterizer states                  		*/
		/************************************************************************/

		/**
		 * @brief	Sets vertex winding order. Normally you would use this to cull back facing
		 *			polygons.
		 */
		void setCullingMode(CullingMode mode);

		/**
		 * @brief	Sets the polygon rasterization mode. Determines how are polygons interpreted.
		 */
		void setPolygonMode(PolygonMode level);

		/**
		 * @brief	Sets a depth bias that will offset the depth values of new pixels by the specified amount.
		 *			Final depth bias value is a combination of the constant depth bias and slope depth bias.
		 *			Slope depth bias has more effect the higher the slope of the rendered polygon.
		 *
		 * @note	This is useful if you want to avoid z fighting for objects at the same or similar depth.
		 */
		void setDepthBias(float constantBias, float slopeScaleBias);

		/**
		 * @brief	Scissor test allows you to mask off rendering in all but a given rectangular area
		 * 			identified by the rectangle set by setScissorRect().
		 */
		void setScissorTestEnable(bool enable);

		/************************************************************************/
		/* 						Depth stencil state                      		*/
		/************************************************************************/
		
		/**
		 * @brief	Should new pixels perform depth testing using the set depth comparison function before
		 *			being written.
		 */
		void setDepthBufferCheckEnabled(bool enabled = true);

		/**
		 * @brief	Should new pixels write to the depth buffer.
		 */
		void setDepthBufferWriteEnabled(bool enabled = true);

		/**
		 * @brief	Sets comparison function used for depth testing. Determines how are new and existing
		 *			pixel values compared - if comparison function returns true the new pixel is written.
		 */
		void setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL);

		/**
		 * @brief	Turns stencil tests on or off. By default this is disabled.
		 *			Stencil testing allow you to mask out a part of the rendered image by using
		 *			various stencil operations provided.
		 */
		void setStencilCheckEnabled(bool enabled);

		/**
		 * @brief	Allows you to set stencil operations that are performed when stencil test passes or fails.
		 *
		 * @param	stencilFailOp	Operation executed when stencil test fails.
		 * @param	depthFailOp		Operation executed when stencil test succeeds but depth test fails.
		 * @param	passOp			Operation executed when stencil test succeeds and depth test succeeds.
		 * @param	front			Should the stencil operations be applied to front or back facing polygons.
		 */
		void setStencilBufferOperations(StencilOperation stencilFailOp = SOP_KEEP,
			StencilOperation depthFailOp = SOP_KEEP, StencilOperation passOp = SOP_KEEP,
			bool front = true);

		/**
		 * @brief	Sets a stencil buffer comparison function. The result of this will cause one of 3 actions 
		 *			depending on whether the test fails, succeeds but with the depth buffer check still failing, 
		 *			or succeeds with the depth buffer check passing too.
		 *
		 * @param func	Comparison function that determines whether a stencil test fails or passes. Reference value
		 *				gets compared to the value already in the buffer using this function.
		 * @param mask	The bitmask applied to both the stencil value and the reference value
		 *				before comparison
		 * @param ccw	If set to true, the stencil operations will be applied to counterclockwise
		 *				faces. Otherwise they will be applied to clockwise faces.
		 */
		void setStencilBufferFunc(CompareFunction func = CMPF_ALWAYS_PASS, UINT32 mask = 0xFFFFFFFF, bool front = true);

		/**
		 * @brief	The bitmask applied to the stencil value before writing it to the stencil buffer.
		 */
		void setStencilBufferWriteMask(UINT32 mask = 0xFFFFFFFF);

		/**
		 * @brief	Sets a reference values used for stencil buffer comparisons. 
		 * 			Actual comparison function and stencil operations are set by setting the DepthStencilState.
		 */
		void setStencilRefValue(UINT32 refValue);

		/************************************************************************/
		/* 							UTILITY METHODS                      		*/
		/************************************************************************/

		/**
		 * @brief	Converts the provided matrix m into a representation usable by OpenGL.
		 */
		void makeGLMatrix(GLfloat gl_matrix[16], const Matrix4& m);

		/**
		 * @brief	Converts the engine depth/stencil compare function into OpenGL representation.
		 */
		GLint convertCompareFunction(CompareFunction func) const;

		/**
		 * @brief	Convers the engine stencil operation in OpenGL representation. Optionally inverts
		 *			the operation (increment becomes decrement, etc.).
		 */
		GLint convertStencilOp(StencilOperation op, bool invert = false) const;

		/**
		 * @brief	Checks if there are any OpenGL errors and prints them to the log.
		 */
		bool checkForErrors() const;

	private:
		UINT32 mScissorTop, mScissorBottom, mScissorLeft, mScissorRight;
		UINT32 mViewportLeft, mViewportTop, mViewportWidth, mViewportHeight;

		UINT32 mStencilReadMask;
		UINT32 mStencilWriteMask;
		UINT32 mStencilRefValue;
		CompareFunction mStencilCompareFront;
		CompareFunction mStencilCompareBack;

		// View matrix to set world against
		Matrix4 mViewMatrix;

		// Last min & mip filtering options, so we can combine them
		FilterOptions mMinFilter;
		FilterOptions mMipFilter;

		// Holds texture type settings for every stage
		UINT32	mNumTextureTypes;
		GLenum* mTextureTypes;

		bool mDepthWrite;
		bool mColorWrite[4];

		GLSupport* mGLSupport;
		bool mGLInitialised;

		GLSLProgramFactory* mGLSLProgramFactory;
		GLSLProgramPipelineManager* mProgramPipelineManager;

		GLSLGpuProgramPtr mCurrentVertexProgram;
		GLSLGpuProgramPtr mCurrentFragmentProgram;
		GLSLGpuProgramPtr mCurrentGeometryProgram;
		GLSLGpuProgramPtr mCurrentHullProgram;
		GLSLGpuProgramPtr mCurrentDomainProgram;

		const GLSLProgramPipeline* mActivePipeline;

		UINT32 mFragmentTexOffset;
		UINT32 mVertexTexOffset;
		UINT32 mGeometryTexOffset;

		UINT32 mFragmentUBOffset;
		UINT32 mVertexUBOffset;
		UINT32 mGeometryUBOffset;
		UINT32 mHullUBOffset;
		UINT32 mDomainUBOffset;
		UINT32 mComputeUBOffset;

		Vector<VertexBufferPtr> mBoundVertexBuffers;
		VertexDeclarationPtr mBoundVertexDeclaration;
		IndexBufferPtr mBoundIndexBuffer;
		DrawOperationType mCurrentDrawOperation;

		GLContext *mMainContext;
		GLContext *mCurrentContext;

		bool mDrawCallInProgress;

		UINT16 mActiveTextureUnit;
    };
}
