// Copyright (C) 2002-20014 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES1_DRIVER_H_INCLUDED__
#define __C_OGLES1_DRIVER_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"

#ifdef _IRR_COMPILE_WITH_OGLES1_

#include "CNullDriver.h"
#include "IMaterialRendererServices.h"
#include "EDriverFeatures.h"
#include "fast_atof.h"
#include "COGLESExtensionHandler.h"
#include "IContextManager.h"

#if defined(_IRR_WINDOWS_API_)
// include windows headers for HWND
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma comment(lib, "libGLES_CM.lib")
#endif

namespace irr
{
namespace video
{

	class COGLES1Driver : public CNullDriver, public IMaterialRendererServices, public COGLES1ExtensionHandler
	{
		friend class COpenGLCoreTexture<COGLES1Driver>;

	public:
		//! constructor
		COGLES1Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);

		//! destructor
		virtual ~COGLES1Driver();

		virtual bool beginScene(u16 clearFlag, SColor clearColor = SColor(255, 0, 0, 0), f32 clearDepth = 1.f, u8 clearStencil = 0,
			const SExposedVideoData& videoData = SExposedVideoData(), core::rect<s32>* sourceRect = 0) _IRR_OVERRIDE_;

		virtual bool endScene() _IRR_OVERRIDE_;

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat) _IRR_OVERRIDE_;


		struct SHWBufferLink_opengl : public SHWBufferLink
		{
			SHWBufferLink_opengl(const scene::IMeshBuffer *_MeshBuffer): SHWBufferLink(_MeshBuffer), vbo_verticesID(0),vbo_indicesID(0){}

			GLuint vbo_verticesID; //tmp
			GLuint vbo_indicesID; //tmp

			GLuint vbo_verticesSize; //tmp
			GLuint vbo_indicesSize; //tmp

		};

		bool updateVertexHardwareBuffer(SHWBufferLink_opengl *HWBuffer);
		bool updateIndexHardwareBuffer(SHWBufferLink_opengl *HWBuffer);

		//! updates hardware buffer if needed
		virtual bool updateHardwareBuffer(SHWBufferLink *HWBuffer) _IRR_OVERRIDE_;

		//! Create hardware buffer from mesh
		virtual SHWBufferLink *createHardwareBuffer(const scene::IMeshBuffer* mb) _IRR_OVERRIDE_;

		//! Delete hardware buffer (only some drivers can)
		virtual void deleteHardwareBuffer(SHWBufferLink *HWBuffer) _IRR_OVERRIDE_;

		//! Draw hardware buffer
		virtual void drawHardwareBuffer(SHWBufferLink *HWBuffer) _IRR_OVERRIDE_;

		virtual IRenderTarget* addRenderTarget() _IRR_OVERRIDE_;

		//! draws a vertex primitive list
		virtual void drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType) _IRR_OVERRIDE_;

		void drawVertexPrimitiveList2d3d(const void* vertices, u32 vertexCount, const void* indexList, u32 primitiveCount, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType=EIT_16BIT, bool threed=true);

		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const _IRR_OVERRIDE_
		{
//			return FeatureEnabled[feature] && COGLES1ExtensionHandler::queryFeature(feature);
			return COGLES1ExtensionHandler::queryFeature(feature);
		}

		//! Sets a material.
		virtual void setMaterial(const SMaterial& material) _IRR_OVERRIDE_;

		virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
			SColor color = SColor(255, 255, 255, 255), bool useAlphaChannelOfTexture = false) _IRR_OVERRIDE_;

		virtual void draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
			const video::SColor* const colors = 0, bool useAlphaChannelOfTexture = false) _IRR_OVERRIDE_;

		virtual void draw2DImage(const video::ITexture* texture, bool flip);

		//! draws a set of 2d images
		virtual void draw2DImageBatch(const video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::array<core::rect<s32> >& sourceRects,
				const core::array<s32>& indices, s32 kerningWidth = 0,
				const core::rect<s32>* clipRect=0,
				SColor color=SColor(255,255,255,255),
				bool useAlphaChannelOfTexture=false) _IRR_OVERRIDE_;

		//! draws a set of 2d images, using a color and the alpha channel of the texture if desired.
		virtual void draw2DImageBatch(const video::ITexture* texture,
				const core::array<core::position2d<s32> >& positions,
				const core::array<core::rect<s32> >& sourceRects,
				const core::rect<s32>* clipRect=0,
				SColor color=SColor(255,255,255,255),
				bool useAlphaChannelOfTexture=false) _IRR_OVERRIDE_;

		//! draw an 2d rectangle
		virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos,
			const core::rect<s32>* clip = 0) _IRR_OVERRIDE_;

		//!Draws an 2d rectangle with a gradient.
		virtual void draw2DRectangle(const core::rect<s32>& pos,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip = 0) _IRR_OVERRIDE_;

		//! Draws a 2d line.
		virtual void draw2DLine(const core::position2d<s32>& start,
					const core::position2d<s32>& end,
					SColor color=SColor(255,255,255,255)) _IRR_OVERRIDE_;

		//! Draws a single pixel
		virtual void drawPixel(u32 x, u32 y, const SColor & color) _IRR_OVERRIDE_;

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
					const core::vector3df& end,
					SColor color = SColor(255,255,255,255)) _IRR_OVERRIDE_;

		//! Returns the name of the video driver.
		virtual const wchar_t* getName() const _IRR_OVERRIDE_;

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights() _IRR_OVERRIDE_;

		//! adds a dynamic light
		virtual s32 addDynamicLight(const SLight& light) _IRR_OVERRIDE_;

		//! Turns a dynamic light on or off
		//! \param lightIndex: the index returned by addDynamicLight
		//! \param turnOn: true to turn the light on, false to turn it off
		virtual void turnLightOn(s32 lightIndex, bool turnOn) _IRR_OVERRIDE_;

		//! returns the maximal amount of dynamic lights the device can handle
		virtual u32 getMaximalDynamicLightAmount() const _IRR_OVERRIDE_;

		//! Sets the dynamic ambient light color.
		virtual void setAmbientLight(const SColorf& color) _IRR_OVERRIDE_;

		//! Draws a shadow volume into the stencil buffer.
		virtual void drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible=0) _IRR_OVERRIDE_;

		//! Fills the stencil shadow with color.
		virtual void drawStencilShadow(bool clearStencilBuffer=false,
			video::SColor leftUpEdge = video::SColor(0,0,0,0),
			video::SColor rightUpEdge = video::SColor(0,0,0,0),
			video::SColor leftDownEdge = video::SColor(0,0,0,0),
			video::SColor rightDownEdge = video::SColor(0,0,0,0)) _IRR_OVERRIDE_;

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area) _IRR_OVERRIDE_;

		//! Sets the fog mode.
		virtual void setFog(SColor color, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog) _IRR_OVERRIDE_;

		//! Only used internally by the engine
		virtual void OnResize(const core::dimension2d<u32>& size) _IRR_OVERRIDE_;

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const _IRR_OVERRIDE_;

		//! get color format of the current color buffer
		virtual ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const _IRR_OVERRIDE_;

		//! Can be called by an IMaterialRenderer to make its work easier.
		virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
			bool resetAllRenderstates) _IRR_OVERRIDE_;

		//! Compare in SMaterial doesn't check texture parameters, so we should call this on each OnRender call.
		virtual void setTextureRenderStates(const SMaterial& material, bool resetAllRenderstates);

		//! Get a vertex shader constant index.
		virtual s32 getVertexShaderConstantID(const c8* name) _IRR_OVERRIDE_;

		//! Get a pixel shader constant index.
		virtual s32 getPixelShaderConstantID(const c8* name) _IRR_OVERRIDE_;

		//! Sets a constant for the vertex shader based on an index.
		virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;

		//! Int interface for the above.
		virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;

		//! Sets a constant for the pixel shader based on an index.
		virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;

		//! Int interface for the above.
		virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;

		//! Sets a vertex shader constant.
		virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1) _IRR_OVERRIDE_;

		//! Sets a pixel shader constant.
		virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1) _IRR_OVERRIDE_;

		//! Adds a new material renderer to the VideoDriver
		virtual s32 addShaderMaterial(const c8* vertexShaderProgram, const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData) _IRR_OVERRIDE_;

		//! Adds a new material renderer to the VideoDriver
		virtual s32 addHighLevelShaderMaterial(const c8* vertexShaderProgram, const c8* vertexShaderEntryPointName,
			E_VERTEX_SHADER_TYPE vsCompileTarget, const c8* pixelShaderProgram, const c8* pixelShaderEntryPointName,
			E_PIXEL_SHADER_TYPE psCompileTarget, const c8* geometryShaderProgram, const c8* geometryShaderEntryPointName,
			E_GEOMETRY_SHADER_TYPE gsCompileTarget, scene::E_PRIMITIVE_TYPE inType, scene::E_PRIMITIVE_TYPE outType,
			u32 verticesOut, IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial,
			s32 userData, E_GPU_SHADING_LANGUAGE shadingLang) _IRR_OVERRIDE_;

		//! Returns pointer to the IGPUProgrammingServices interface.
		virtual IGPUProgrammingServices* getGPUProgrammingServices() _IRR_OVERRIDE_;

		//! Returns a pointer to the IVideoDriver interface.
		virtual IVideoDriver* getVideoDriver() _IRR_OVERRIDE_;

		//! Returns the maximum amount of primitives
		virtual u32 getMaximalPrimitiveCount() const _IRR_OVERRIDE_;

		virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
			const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN) _IRR_OVERRIDE_;

		virtual bool setRenderTargetEx(IRenderTarget* target, u16 clearFlag, SColor clearColor = SColor(255, 0, 0, 0),
			f32 clearDepth = 1.f, u8 clearStencil = 0) _IRR_OVERRIDE_;

		virtual void clearBuffers(u16 flag, SColor color = SColor(255, 0, 0, 0), f32 depth = 1.f, u8 stencil = 0) _IRR_OVERRIDE_;

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot(video::ECOLOR_FORMAT format=video::ECF_UNKNOWN, video::E_RENDER_TARGET target=video::ERT_FRAME_BUFFER) _IRR_OVERRIDE_;

		//! checks if an OpenGL error has happened and prints it
		bool testGLError();

		//! Set/unset a clipping plane.
		virtual bool setClipPlane(u32 index, const core::plane3df& plane, bool enable=false) _IRR_OVERRIDE_;

		//! Enable/disable a clipping plane.
		virtual void enableClipPlane(u32 index, bool enable) _IRR_OVERRIDE_;

		//! Returns the graphics card vendor name.
		virtual core::stringc getVendorInfo() _IRR_OVERRIDE_
		{
			return VendorName;
		}

		//! Get the maximal texture size for this driver
		core::dimension2du getMaxTextureSize() const _IRR_OVERRIDE_;

		void removeTexture(ITexture* texture) _IRR_OVERRIDE_;

		//! Convert E_BLEND_FACTOR to OpenGL equivalent
		GLenum getGLBlend(E_BLEND_FACTOR factor) const;

		//! Get ZBuffer bits.
		GLenum getZBufferBits() const;

		void getColorFormatParameters(ECOLOR_FORMAT format, GLint& internalFormat, GLenum& pixelFormat,
			GLenum& pixelType, void(**converter)(const void*, s32, void*));

		COGLES1CacheHandler* getCacheHandler() const;

	private:
		void uploadClipPlane(u32 index);

		//! inits the opengl-es driver
		bool genericDriverInit(const core::dimension2d<u32>& screenSize, bool stencilBuffer);

		virtual ITexture* createDeviceDependentTexture(const io::path& name, IImage* image) _IRR_OVERRIDE_;

		virtual ITexture* createDeviceDependentTextureCubemap(const io::path& name, const core::array<IImage*>& image) _IRR_OVERRIDE_;

		//! creates a transposed matrix in supplied GLfloat array to pass to OGLES1
		inline void getGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m);
		inline void getGLTextureMatrix(GLfloat gl_matrix[16], const core::matrix4& m);

		//! Set GL pipeline to desired texture wrap modes of the material
		void setWrapMode(const SMaterial& material);

		//! Get OpenGL wrap enum from Irrlicht enum
		GLint getTextureWrapMode(u8 clamp) const;

		//! sets the needed renderstates
		void setRenderStates3DMode();

		//! sets the needed renderstates
		void setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel);

		void createMaterialRenderers();

		//! Assign a hardware light to the specified requested light, if any
		//! free hardware lights exist.
		//! \param[in] lightIndex: the index of the requesting light
		void assignHardwareLight(u32 lightIndex);

		COGLES1CacheHandler* CacheHandler;

		core::stringw Name;
		core::matrix4 Matrices[ETS_COUNT];
		core::array<u8> ColorBuffer;

		//! enumeration for rendering modes such as 2d and 3d for minimizing the switching of renderStates.
		enum E_RENDER_MODE
		{
			ERM_NONE = 0,	// no render state has been set yet.
			ERM_2D,		// 2d drawing rendermode
			ERM_3D		// 3d rendering mode
		};

		E_RENDER_MODE CurrentRenderMode;
		//! bool to make all renderstates reset if set to true.
		bool ResetRenderStates;
		bool Transformation3DChanged;
		u8 AntiAlias;

		SMaterial Material, LastMaterial;
		core::array<core::plane3df> UserClipPlane;
		core::array<bool> UserClipPlaneEnabled;

		core::stringc VendorName;

		core::matrix4 TextureFlipMatrix;

		//! Color buffer format
		ECOLOR_FORMAT ColorFormat;

		SIrrlichtCreationParameters Params;

		//! All the lights that have been requested; a hardware limited
		//! number of them will be used at once.
		struct RequestedLight
		{
			RequestedLight(SLight const & lightData)
				: LightData(lightData), HardwareLightIndex(-1), DesireToBeOn(true) { }

			SLight	LightData;
			s32		HardwareLightIndex; // GL_LIGHT0 - GL_LIGHT7
			bool	DesireToBeOn;
		};
		core::array<RequestedLight> RequestedLights;

		IContextManager* ContextManager;
	};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OGLES1_

#endif
