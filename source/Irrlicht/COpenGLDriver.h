// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_VIDEO_OPEN_GL_H_INCLUDED__
#define __C_VIDEO_OPEN_GL_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "CNullDriver.h"
#include "IMaterialRendererServices.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#if defined(_IRR_WINDOWS_API_)
	// include windows headers for HWND
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include "glext.h"
#ifdef _MSC_VER
	#pragma comment(lib, "OpenGL32.lib")
	#pragma comment(lib, "GLu32.lib")
#endif
#elif defined(MACOSX)
	#define GL_EXT_texture_env_combine 1
	#include "CIrrDeviceMacOSX.h"
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>
#elif defined(_IRR_USE_SDL_DEVICE_)
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		#define GL_GLEXT_LEGACY 1
		#define GLX_GLXEXT_LEGACY 1
	#else
		#define GL_GLEXT_PROTOTYPES 1
		#define GLX_GLXEXT_PROTOTYPES 1
	#endif
	#include <SDL/SDL_opengl.h>
	#define NO_SDL_GLEXT
	#include "glext.h"
#else
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		#define GL_GLEXT_LEGACY 1
		#define GLX_GLXEXT_LEGACY 1
	#else
		#define GL_GLEXT_PROTOTYPES 1
		#define GLX_GLXEXT_PROTOTYPES 1
	#endif
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glx.h>
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#include "glext.h"
	#undef GLX_ARB_get_proc_address // avoid problems with local glxext.h
	#include "glxext.h"
	#endif
#endif

namespace irr
{
namespace video
{
	class COpenGLTexture;

	class COpenGLDriver : public CNullDriver, public IMaterialRendererServices
	{
	public:

		#ifdef _IRR_WINDOWS_API_
		//! win32 constructor
		COpenGLDriver(const core::dimension2d<s32>& screenSize, HWND window, bool fullscreen,
			bool stencilBuffer, io::IFileSystem* io, bool antiAlias);

		//! inits the windows specific parts of the open gl driver
		bool initDriver(const core::dimension2d<s32>& screenSize, HWND window,
			u32 bits, bool fullscreen, bool vsync);
		#endif

		#ifdef _IRR_USE_LINUX_DEVICE_
		COpenGLDriver(const core::dimension2d<s32>& screenSize, bool fullscreen,
			bool stencilBuffer, io::IFileSystem* io, bool vsync, bool antiAlias);
		#endif

		#ifdef MACOSX
		COpenGLDriver(const core::dimension2d<s32>& screenSize, bool fullscreen,
			bool stencilBuffer, CIrrDeviceMacOSX *device,io::IFileSystem* io, bool vsync, bool antiAlias);
		#endif

		#ifdef _IRR_USE_SDL_DEVICE_
		COpenGLDriver(const core::dimension2d<s32>& screenSize, bool fullscreen,
			bool stencilBuffer, io::IFileSystem* io, bool vsync, bool antiAlias);
		#endif

		//! destructor
		virtual ~COpenGLDriver();

		//! presents the rendered scene on the screen, returns false if failed
		virtual bool endScene( s32 windowId, core::rect<s32>* sourceRect=0 );

		//! clears the zbuffer
		virtual bool beginScene(bool backBuffer, bool zBuffer, SColor color);

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

		//! draws a vertex primitive list
		void drawVertexPrimitiveList(const void* vertices, u32 vertexCount, const u16* indexList, u32 primitiveCount, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType);

		//! queries the features of the driver, returns true if feature is available
		bool queryFeature(E_VIDEO_DRIVER_FEATURE feature);

		//! Sets a material. All 3d drawing functions draw geometry now
		//! using this material.
		//! \param material: Material to be used from now on.
		virtual void setMaterial(const SMaterial& material);

		//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		virtual void draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
			SColor color=SColor(255,255,255,255), bool useAlphaChannelOfTexture=false);

		//! draws a set of 2d images, using a color and the alpha
		/** channel of the texture if desired. The images are drawn
		beginning at pos and concatenated in one line. All drawings
		are clipped against clipRect (if != 0).
		The subtextures are defined by the array of sourceRects
		and are chosen by the indices given.
		\param texture: Texture to be drawn.
		\param pos: Upper left 2d destination position where the image will be drawn.
		\param sourceRects: Source rectangles of the image.
		\param indices: List of indices which choose the actual rectangle used each time.
		\param clipRect: Pointer to rectangle on the screen where the image is clipped to.
		This pointer can be 0. Then the image is not clipped.
		\param color: Color with which the image is colored.
		Note that the alpha component is used: If alpha is other than 255, the image will be transparent.
		\param useAlphaChannelOfTexture: If true, the alpha channel of the texture is
		used to draw the image. */
		virtual void draw2DImage(video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::array<core::rect<s32> >& sourceRects,
				const core::array<s32>& indices,
				const core::rect<s32>* clipRect=0,
				SColor color=SColor(255,255,255,255),
				bool useAlphaChannelOfTexture=false);

		//! Draws a part of the texture into the rectangle.
		virtual void draw2DImage(video::ITexture* texture, const core::rect<s32>& destRect,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
			video::SColor* colors=0, bool useAlphaChannelOfTexture=false);

		//! draw an 2d rectangle
		virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos,
			const core::rect<s32>* clip = 0);

		//!Draws an 2d rectangle with a gradient.
		virtual void draw2DRectangle(const core::rect<s32>& pos,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip = 0);

		//! Draws a 2d line.
		virtual void draw2DLine(const core::position2d<s32>& start,
					const core::position2d<s32>& end,
					SColor color=SColor(255,255,255,255));

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
					const core::vector3df& end,
					SColor color = SColor(255,255,255,255));

		//! \return Returns the name of the video driver. Example: In case of the Direct3D8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName();

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights();

		//! adds a dynamic light
		virtual void addDynamicLight(const SLight& light);

		//! returns the maximal amount of dynamic lights the device can handle
		virtual u32 getMaximalDynamicLightAmount();

		//! Sets the dynamic ambient light color. The default color is
		//! (0,0,0,0) which means it is dark.
		//! \param color: New color of the ambient light.
		virtual void setAmbientLight(const SColorf& color);

		//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
		//! this: First, draw all geometry. Then use this method, to draw the shadow
		//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
		virtual void drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail);

		//! Fills the stencil shadow with color. After the shadow volume has been drawn
		//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
		//! to draw the color of the shadow.
		virtual void drawStencilShadow(bool clearStencilBuffer=false,
			video::SColor leftUpEdge = video::SColor(0,0,0,0),
			video::SColor rightUpEdge = video::SColor(0,0,0,0),
			video::SColor leftDownEdge = video::SColor(0,0,0,0),
			video::SColor rightDownEdge = video::SColor(0,0,0,0));

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area);

		//! Sets the fog mode.
		virtual void setFog(SColor color, bool linearFog, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog);

		//! Only used by the internal engine. Used to notify the driver that
		//! the window was resized.
		virtual void OnResize(const core::dimension2d<s32>& size);

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType();

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state);

		// public access to the (loaded) extensions.
		void extGlActiveTextureARB(GLenum texture);
		void extGlClientActiveTextureARB(GLenum texture);
		void extGlGenProgramsARB(GLsizei n, GLuint *programs);
		void extGlBindProgramARB(GLenum target, GLuint program);
		void extGlProgramStringARB(GLenum target, GLenum format, GLsizei len, const GLvoid *string);
		void extGlDeleteProgramsARB(GLsizei n, const GLuint *programs);
		void extGlProgramLocalParameter4fvARB(GLenum, GLuint, const GLfloat *);
		GLhandleARB extGlCreateShaderObjectARB(GLenum shaderType);
		void extGlShaderSourceARB(GLhandleARB shader, int numOfStrings, const char **strings, int *lenOfStrings);
		void extGlCompileShaderARB(GLhandleARB shader);
		GLhandleARB extGlCreateProgramObjectARB(void);
		void extGlAttachObjectARB(GLhandleARB program, GLhandleARB shader);
		void extGlLinkProgramARB(GLhandleARB program);
		void extGlUseProgramObjectARB(GLhandleARB prog);
		void extGlDeleteObjectARB(GLhandleARB object);
		void extGlGetInfoLogARB(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
		void extGlGetObjectParameterivARB(GLhandleARB object, GLenum type, int *param);
		GLint extGlGetUniformLocationARB(GLhandleARB program, const char *name);
		void extGlUniform4fvARB(GLint location, GLsizei count, const GLfloat *v);

		void extGlUniform1ivARB (GLint loc, GLsizei count, const GLint *v);
		void extGlUniform1fvARB (GLint loc, GLsizei count, const GLfloat *v);
		void extGlUniform2fvARB (GLint loc, GLsizei count, const GLfloat *v);
		void extGlUniform3fvARB (GLint loc, GLsizei count, const GLfloat *v);
		void extGlUniformMatrix2fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
		void extGlUniformMatrix3fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
		void extGlUniformMatrix4fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
		void extGlGetActiveUniformARB (GLhandleARB program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
		void extGlPointParameterfARB (GLint loc, GLfloat f);
		void extGlPointParameterfvARB (GLint loc, const GLfloat *v);
		void extGlStencilFuncSeparate (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
		void extGlStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
		void extGlCompressedTexImage2D(GLenum target, GLint level,
			GLenum internalformat, GLsizei width, GLsizei height,
			GLint border, GLsizei imageSize, const void* data);

        void extGlBindFramebufferEXT (GLenum target, GLuint framebuffer);
        void extGlDeleteFramebuffersEXT (GLsizei n, const GLuint *framebuffers);
        void extGlGenFramebuffersEXT (GLsizei n, GLuint *framebuffers);
        GLenum extGlCheckFramebufferStatusEXT (GLenum target);
        void extGlFramebufferTexture2DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
        void extGlBindRenderbufferEXT (GLenum target, GLuint renderbuffer);
        void extGlDeleteRenderbuffersEXT (GLsizei n, const GLuint *renderbuffers);
        void extGlGenRenderbuffersEXT (GLsizei n, GLuint *renderbuffers);
        void extGlRenderbufferStorageEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
        void extGlFramebufferRenderbufferEXT (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

		//! Can be called by an IMaterialRenderer to make its work easier.
		void setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
			bool resetAllRenderstates);

		//! Sets a vertex shader constant.
		virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);

		//! Sets a pixel shader constant.
		virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);

		//! Sets a constant for the vertex shader based on a name.
		virtual bool setVertexShaderConstant(const c8* name, const f32* floats, int count);

		//! Sets a constant for the pixel shader based on a name.
		virtual bool setPixelShaderConstant(const c8* name, const f32* floats, int count);

		//! sets the current Texture
		//! Returns whether setting was a success or not.
		bool setTexture(s32 stage, video::ITexture* texture);

		//! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
		//! Returns whether disabling was successful or not.
		bool disableTextures(s32 fromStage=0);

		//! Adds a new material renderer to the VideoDriver, using extGLGetObjectParameterivARB(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status) pixel and/or
		//! vertex shaders to render geometry.
		s32 addShaderMaterial(const c8* vertexShaderProgram, const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData);

		//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
		s32 addHighLevelShaderMaterial(const c8* vertexShaderProgram, const c8* vertexShaderEntryPointName,
			E_VERTEX_SHADER_TYPE vsCompileTarget, const c8* pixelShaderProgram, const c8* pixelShaderEntryPointName,
			E_PIXEL_SHADER_TYPE psCompileTarget, IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial,
			s32 userData);

		//! Returns pointer to the IGPUProgrammingServices interface.
		IGPUProgrammingServices* getGPUProgrammingServices();

		//! Returns a pointer to the IVideoDriver interface. (Implementation for
		//! IMaterialRendererServices)
		virtual IVideoDriver* getVideoDriver();

		//! Returns the maximum amount of primitives (mostly vertices) which
		//! the device is able to render with one drawIndexedTriangleList
		//! call.
		virtual u32 getMaximalPrimitiveCount();

		ITexture* createRenderTargetTexture(const core::dimension2d<s32>& size);

		bool setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
					bool clearZBuffer, SColor color);

		//! Clears the ZBuffer.
		virtual void clearZBuffer();

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot();

		//! checks if an OpenGL error has happend and prints it
		//! for performance reasons only available in debug mode
		bool testGLError();

	private:

		//! inits the parts of the open gl driver used on all platforms
		bool genericDriverInit(const core::dimension2d<s32>& screenSize);
		//! returns a device dependent texture from a software surface (IImage)
		virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const char* name);

		//! creates a transposed matrix in supplied GLfloat array to pass to OpenGL
		void createGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m);
		void createGLTextureMatrix(GLfloat gl_matrix[16], const core::matrix4& m);


		//! sets the needed renderstates
		void setRenderStates3DMode();

		//! sets the needed renderstates
		void setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel);

		// returns the current size of the screen or rendertarget
		core::dimension2d<s32> getCurrentRenderTargetSize();

		void loadExtensions();
		void createMaterialRenderers();

		core::stringw Name;
		core::matrix4 Matrices[ETS_COUNT];
		core::array<u8> ColorBuffer;

		// enumeration for rendering modes such as 2d and 3d for minizing the switching of renderStates.
		enum E_RENDER_MODE
		{
			ERM_NONE = 0,	// no render state has been set yet.
			ERM_2D,		// 2d drawing rendermode
			ERM_3D		// 3d rendering mode
		};

		E_RENDER_MODE CurrentRenderMode;
		bool ResetRenderStates; // bool to make all renderstates be reseted if set.
		bool Transformation3DChanged;
		bool StencilBuffer;
		bool AntiAlias;
		bool MultiTextureExtension;
		bool MultiSamplingExtension;
		bool AnisotropyExtension;
		bool ARBVertexProgramExtension; //GL_ARB_vertex_program
		bool ARBFragmentProgramExtension; //GL_ARB_fragment_program
		bool ARBShadingLanguage100Extension;
		bool SeparateStencilExtension;
		bool GenerateMipmapExtension;
		bool TextureCompressionExtension;
		bool TextureNPOTExtension;
		bool FramebufferObjectExtension;
		bool EXTPackedDepthStencil;
		bool EXTSeparateSpecularColor;

		SMaterial Material, LastMaterial;
		COpenGLTexture* RenderTargetTexture;
		ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
		s32 LastSetLight;
		f32 MaxAnisotropy;

		GLint MaxTextureUnits;
		GLint MaxLights;
		GLint MaxIndices;

		core::dimension2d<s32> CurrentRendertargetSize;

		#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
			PFNGLACTIVETEXTUREARBPROC pGlActiveTextureARB;
			PFNGLCLIENTACTIVETEXTUREARBPROC	pGlClientActiveTextureARB;
			PFNGLGENPROGRAMSARBPROC pGlGenProgramsARB;
			PFNGLBINDPROGRAMARBPROC pGlBindProgramARB;
			PFNGLPROGRAMSTRINGARBPROC pGlProgramStringARB;
			PFNGLDELETEPROGRAMSNVPROC pGlDeleteProgramsARB;
			PFNGLPROGRAMLOCALPARAMETER4FVARBPROC pGlProgramLocalParameter4fvARB;
			PFNGLCREATESHADEROBJECTARBPROC pGlCreateShaderObjectARB;
			PFNGLSHADERSOURCEARBPROC pGlShaderSourceARB;
			PFNGLCOMPILESHADERARBPROC pGlCompileShaderARB;
			PFNGLCREATEPROGRAMOBJECTARBPROC pGlCreateProgramObjectARB;
			PFNGLATTACHOBJECTARBPROC pGlAttachObjectARB;
			PFNGLLINKPROGRAMARBPROC pGlLinkProgramARB;
			PFNGLUSEPROGRAMOBJECTARBPROC pGlUseProgramObjectARB;
			PFNGLDELETEOBJECTARBPROC pGlDeleteObjectARB;
			PFNGLGETINFOLOGARBPROC pGlGetInfoLogARB;
			PFNGLGETOBJECTPARAMETERIVARBPROC pGlGetObjectParameterivARB;
			PFNGLGETUNIFORMLOCATIONARBPROC pGlGetUniformLocationARB;
			PFNGLUNIFORM1IVARBPROC pGlUniform1ivARB;
			PFNGLUNIFORM1FVARBPROC pGlUniform1fvARB;
			PFNGLUNIFORM2FVARBPROC pGlUniform2fvARB;
			PFNGLUNIFORM3FVARBPROC pGlUniform3fvARB;
			PFNGLUNIFORM4FVARBPROC pGlUniform4fvARB;
			PFNGLUNIFORMMATRIX2FVARBPROC pGlUniformMatrix2fvARB;
			PFNGLUNIFORMMATRIX3FVARBPROC pGlUniformMatrix3fvARB;
			PFNGLUNIFORMMATRIX4FVARBPROC pGlUniformMatrix4fvARB;
			PFNGLGETACTIVEUNIFORMARBPROC pGlGetActiveUniformARB;
			PFNGLPOINTPARAMETERFARBPROC  pGlPointParameterfARB;
			PFNGLPOINTPARAMETERFVARBPROC pGlPointParameterfvARB;
			#ifdef GL_ATI_separate_stencil
			PFNGLSTENCILFUNCSEPARATEPROC pGlStencilFuncSeparate;
			PFNGLSTENCILOPSEPARATEPROC pGlStencilOpSeparate;
			PFNGLSTENCILFUNCSEPARATEATIPROC pGlStencilFuncSeparateATI;
			PFNGLSTENCILOPSEPARATEATIPROC pGlStencilOpSeparateATI;
			#endif
			#ifdef PFNGLCOMPRESSEDTEXIMAGE2DPROC
			PFNGLCOMPRESSEDTEXIMAGE2DPROC pGlCompressedTexImage2D;
			#endif // PFNGLCOMPRESSEDTEXIMAGE2DPROC
			#ifdef _IRR_WINDOWS_API_
			typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
			PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;
			#elif defined(_IRR_LINUX_PLATFORM_) && defined(GLX_SGI_swap_control)
			PFNGLXSWAPINTERVALSGIPROC glxSwapIntervalSGI;
			#endif
			PFNGLBINDFRAMEBUFFEREXTPROC pGlBindFramebufferEXT;
			PFNGLDELETEFRAMEBUFFERSEXTPROC pGlDeleteFramebuffersEXT;
			PFNGLGENFRAMEBUFFERSEXTPROC pGlGenFramebuffersEXT;
			PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC pGlCheckFramebufferStatusEXT;
			PFNGLFRAMEBUFFERTEXTURE2DEXTPROC pGlFramebufferTexture2DEXT;
			PFNGLBINDRENDERBUFFEREXTPROC pGlBindRenderbufferEXT;
			PFNGLDELETERENDERBUFFERSEXTPROC pGlDeleteRenderbuffersEXT;
			PFNGLGENRENDERBUFFERSEXTPROC pGlGenRenderbuffersEXT;
			PFNGLRENDERBUFFERSTORAGEEXTPROC pGlRenderbufferStorageEXT;
			PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC pGlFramebufferRenderbufferEXT;
		#endif

		#ifdef _IRR_WINDOWS_API_
			HDC HDc; // Private GDI Device Context
			HWND Window;
			HGLRC HRc; // Permanent Rendering Context
		#elif defined(_IRR_USE_LINUX_DEVICE_)
			GLXDrawable XWindow;
			Display* XDisplay;
		#elif defined(MACOSX)
			CIrrDeviceMacOSX *_device;
		#endif
	};

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_OPENGL_
#endif


