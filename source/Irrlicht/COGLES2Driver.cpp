// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "COGLES2Driver.h"
// needed here also because of the create methods' parameters
#include "CNullDriver.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2Texture.h"
#include "COGLES2MaterialRenderer.h"
#include "COGLES2NormalMapRenderer.h"
#include "COGLES2ParallaxMapRenderer.h"
#include "COGLES2Renderer2D.h"
#include "CImage.h"
#include "os.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace irr
{
namespace video
{

//! constructor and init code
	COGLES2Driver::COGLES2Driver(const SIrrlichtCreationParameters& params,
			const SExposedVideoData& data, io::IFileSystem* io
#if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
			, const MIrrIPhoneDevice& device
#endif
	)
		: CNullDriver(io, params.WindowSize), COGLES2ExtensionHandler(),
		CurrentRenderMode(ERM_NONE), ResetRenderStates(true),
		Transformation3DChanged(true), AntiAlias(params.AntiAlias),
		RenderTargetTexture(0), CurrentRendertargetSize(0, 0), ColorFormat(ECF_R8G8B8),
		EglDisplay(EGL_NO_DISPLAY)
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
		, HDc(0)
#elif defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
		, ViewFramebuffer(0)
		, ViewRenderbuffer(0)
		, ViewDepthRenderbuffer(0)
#endif
		, NoHighLevelShader(true)
		, BlendEnabled(false)
		, SourceFactor(EBF_ZERO)
		, DestFactor(EBF_ZERO)
	{
#ifdef _DEBUG
		setDebugName("COGLES2Driver");
#endif
		ExposedData = data;
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
		EglWindow = (NativeWindowType)data.OpenGLWin32.HWnd;
		HDc = GetDC((HWND)EglWindow);
		EglDisplay = eglGetDisplay((NativeDisplayType)HDc);
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
		EglWindow = (NativeWindowType)ExposedData.OpenGLLinux.X11Window;
		EglDisplay = eglGetDisplay((NativeDisplayType)ExposedData.OpenGLLinux.X11Display);
#elif defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
		Device = device;
#endif
		if (EglDisplay == EGL_NO_DISPLAY)
		{
			os::Printer::log("Getting OpenGL-ES2 display.");
			EglDisplay = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);
		}
		if (EglDisplay == EGL_NO_DISPLAY)
		{
			os::Printer::log("Could not get OpenGL-ES2 display.");
		}

		EGLint majorVersion, minorVersion;
		if (!eglInitialize(EglDisplay, &majorVersion, &minorVersion))
		{
			os::Printer::log("Could not initialize OpenGL-ES2 display.");
		}
		else
		{
			char text[64];
			sprintf(text, "EglDisplay initialized. Egl version %d.%d\n", majorVersion, minorVersion);
			os::Printer::log(text);
		}

		EGLint attribs[] =
		{
			EGL_RED_SIZE, 5,
			EGL_GREEN_SIZE, 5,
			EGL_BLUE_SIZE, 5,
			EGL_ALPHA_SIZE, params.WithAlphaChannel ? 1 : 0,
			EGL_BUFFER_SIZE, params.Bits,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			//EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
			EGL_DEPTH_SIZE, params.ZBufferBits,
			EGL_STENCIL_SIZE, params.Stencilbuffer,
			EGL_SAMPLE_BUFFERS, params.AntiAlias ? 1 : 0,
			EGL_SAMPLES, params.AntiAlias,
#ifdef EGL_VERSION_1_3
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#endif
			EGL_NONE, 0
		};
		EGLint contextAttrib[] =
		{
#ifdef EGL_VERSION_1_3
			EGL_CONTEXT_CLIENT_VERSION, 2,
#endif
			EGL_NONE, 0
		};

		EGLConfig config;
		EGLint num_configs;
		u32 steps=5;
		while (!eglChooseConfig(EglDisplay, attribs, &config, 1, &num_configs) || !num_configs)
		{
			switch (steps)
			{
			case 5: // samples
				if (attribs[19]>2)
				{
					--attribs[19];
				}
				else
				{
					attribs[17]=0;
					attribs[19]=0;
					--steps;
				}
				break;
			case 4: // alpha
				if (attribs[7])
				{
					attribs[7]=0;
					if (params.AntiAlias)
					{
						attribs[17]=1;
						attribs[19]=params.AntiAlias;
						steps=5;
					}
				}
				else
					--steps;
				break;
			case 3: // stencil
				if (attribs[15])
				{
					attribs[15]=0;
					if (params.AntiAlias)
					{
						attribs[17]=1;
						attribs[19]=params.AntiAlias;
						steps=5;
					}
				}
				else
					--steps;
				break;
			case 2: // depth size
				if (attribs[13]>16)
				{
					attribs[13]-=8;
				}
				else
					--steps;
				break;
			case 1: // buffer size
				if (attribs[9]>16)
				{
					attribs[9]-=8;
				}
				else
					--steps;
				break;
			default:
				os::Printer::log("Could not get config for OpenGL-ES2 display.");
				return;
			}
		}
		if (params.AntiAlias && !attribs[17])
			os::Printer::log("No multisampling.");
		if (params.WithAlphaChannel && !attribs[7])
			os::Printer::log("No alpha.");
		if (params.Stencilbuffer && !attribs[15])
			os::Printer::log("No stencil buffer.");
		if (params.ZBufferBits > attribs[13])
			os::Printer::log("No full depth buffer.");
		if (params.Bits > attribs[9])
			os::Printer::log("No full color buffer.");
		os::Printer::log(" Creating EglSurface with nativeWindow...");
		EglSurface = eglCreateWindowSurface(EglDisplay, config, EglWindow, NULL);
		if (EGL_NO_SURFACE == EglSurface)
		{
			os::Printer::log("FAILED\n");
			EglSurface = eglCreateWindowSurface(EglDisplay, config, NULL, NULL);
			os::Printer::log("Creating EglSurface without nativeWindows...");
		}
		else
			os::Printer::log("SUCCESS\n");
		if (EGL_NO_SURFACE == EglSurface)
		{
			os::Printer::log("FAILED\n");
			os::Printer::log("Could not create surface for OpenGL-ES2 display.");
		}
		else
			os::Printer::log("SUCCESS\n");

#ifdef EGL_VERSION_1_2
		if (minorVersion>1)
			eglBindAPI(EGL_OPENGL_ES_API);
#endif
		os::Printer::log("Creating EglContext...");
		EglContext = eglCreateContext(EglDisplay, config, EGL_NO_CONTEXT, contextAttrib);
		if (testEGLError())
		{
			os::Printer::log("FAILED\n");
			os::Printer::log("Could not create Context for OpenGL-ES2 display.");
		}

		eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);
		if (testEGLError())
		{
			os::Printer::log("Could not make Context current for OpenGL-ES2 display.");
		}

		genericDriverInit(params.WindowSize, params.Stencilbuffer);

		// set vsync
		if (params.Vsync)
			eglSwapInterval(EglDisplay, 1);
	}


	//! destructor
	COGLES2Driver::~COGLES2Driver()
	{
		deleteMaterialRenders();
		deleteAllTextures();

		// HACK : the following is commented because destroying the context crashes under Linux (Thibault 04-feb-10)
		/*eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(EglDisplay, EglContext);
		eglDestroySurface(EglDisplay, EglSurface);*/
		eglTerminate(EglDisplay);

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
		if (HDc)
			ReleaseDC((HWND)EglWindow, HDc);
#endif

		delete TwoDRenderer;
		delete FixedPipeline;
	}

// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

	bool COGLES2Driver::genericDriverInit(const core::dimension2d<u32>& screenSize, bool stencilBuffer)
	{
		Name = glGetString(GL_VERSION);
		printVersion();

		os::Printer::log(eglQueryString(EglDisplay, EGL_CLIENT_APIS));

		// print renderer information
		vendorName = glGetString(GL_VENDOR);
		os::Printer::log(vendorName.c_str(), ELL_INFORMATION);

		u32 i;
		for (i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			CurrentTexture[i] = 0;
		// load extensions
		initExtensions(this,
						EglDisplay,
						stencilBuffer);

		StencilBuffer = stencilBuffer;

		DriverAttributes->setAttribute("MaxTextures", MaxTextureUnits);
		DriverAttributes->setAttribute("MaxSupportedTextures", MaxSupportedTextures);
		DriverAttributes->setAttribute("MaxLights", MaxLights);
		DriverAttributes->setAttribute("MaxAnisotropy", MaxAnisotropy);
		DriverAttributes->setAttribute("MaxUserClipPlanes", MaxUserClipPlanes);
//		DriverAttributes->setAttribute("MaxAuxBuffers", MaxAuxBuffers);
//		DriverAttributes->setAttribute("MaxMultipleRenderTargets", MaxMultipleRenderTargets);
		DriverAttributes->setAttribute("MaxIndices", (s32)MaxIndices);
		DriverAttributes->setAttribute("MaxTextureSize", (s32)MaxTextureSize);
		DriverAttributes->setAttribute("MaxTextureLODBias", MaxTextureLODBias);
		DriverAttributes->setAttribute("Version", Version);
		DriverAttributes->setAttribute("AntiAlias", AntiAlias);

		FixedPipeline = new COGLES2FixedPipelineShader(this, FileSystem);
		FixedPipeline->useProgram(); //For setting the default uniforms (Alpha)

		TwoDRenderer = new COGLES2Renderer2d(this, FileSystem);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		// Reset The Current Viewport
		glViewport(0, 0, screenSize.Width, screenSize.Height);

		UserClipPlane.reallocate(0);

		setAmbientLight(SColorf(0.0f, 0.0f, 0.0f, 0.0f));
		glClearDepthf(1.0f);

		//TODO : OpenGL ES 2.0 Port : GL_PERSPECTIVE_CORRECTION_HINT
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
		glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
		glDepthFunc(GL_LEQUAL);
		glFrontFace(GL_CW);

		// create material renderers
		createMaterialRenderers();

		// set the renderstates
		setRenderStates3DMode();

		// set fog mode
		setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

		// create matrix for flipping textures
		TextureFlipMatrix.buildTextureTransform(0.0f, core::vector2df(0, 0), core::vector2df(0, 1.0f), core::vector2df(1.0f, -1.0f));

		// We need to reset once more at the beginning of the first rendering.
		// This fixes problems with intermediate changes to the material during texture load.
		ResetRenderStates = true;

		glUseProgram(0);
		testGLError();

		return true;
	}


	void COGLES2Driver::createMaterialRenderers()
	{
		// create OGLES1 material renderers

		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_SOLID(this));
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_SOLID_2_LAYER(this));

		// add the same renderer for all lightmap types
		COGLES2MaterialRenderer_LIGHTMAP* lmr = new COGLES2MaterialRenderer_LIGHTMAP(this);
		addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
		addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
		addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
		addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
		addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
		addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
		addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
		lmr->drop();

		// add remaining material renderer
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_DETAIL_MAP(this));
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_SPHERE_MAP(this));
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_REFLECTION_2_LAYER(this));
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_TRANSPARENT_ADD_COLOR(this));
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(this));
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(this));
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_TRANSPARENT_VERTEX_ALPHA(this));
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(this));

		// add normal map renderers
		s32 tmp = 0;
		video::IMaterialRenderer* renderer = 0;
		renderer = new COGLES2NormalMapRenderer(this, FileSystem, tmp, MaterialRenderers[EMT_SOLID].Renderer);
		renderer->drop();
		renderer = new COGLES2NormalMapRenderer(this, FileSystem, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
		renderer->drop();
		renderer = new COGLES2NormalMapRenderer(this, FileSystem, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
		renderer->drop();

		// add parallax map renderers
		renderer = new COGLES2ParallaxMapRenderer(this, FileSystem, tmp, MaterialRenderers[EMT_SOLID].Renderer);
		renderer->drop();
		renderer = new COGLES2ParallaxMapRenderer(this, FileSystem, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
		renderer->drop();
		renderer = new COGLES2ParallaxMapRenderer(this, FileSystem, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
		renderer->drop();

		// add basic 1 texture blending
		addAndDropMaterialRenderer(new COGLES2MaterialRenderer_ONETEXTURE_BLEND(this));
	}


	//! presents the rendered scene on the screen, returns false if failed
	bool COGLES2Driver::endScene()
	{
		CNullDriver::endScene();

		eglSwapBuffers(EglDisplay, EglSurface);
		EGLint g = eglGetError();
		if (EGL_SUCCESS != g)
		{
			if (EGL_CONTEXT_LOST == g)
			{
				// o-oh, ogl-es has lost contexts...
				os::Printer::log("Context lost, please restart your app.");
			}
			else
				os::Printer::log("Could not swap buffers for OpenGL-ES2 driver.");
			return false;
		}
		return true;
	}


	//! clears the zbuffer
	bool COGLES2Driver::beginScene(bool backBuffer, bool zBuffer, SColor color,
			const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
	{
		CNullDriver::beginScene(backBuffer, zBuffer, color);

		GLbitfield mask = 0;

		if (backBuffer)
		{
			const f32 inv = 1.0f / 255.0f;
			glClearColor(color.getRed() * inv, color.getGreen() * inv,
						color.getBlue() * inv, color.getAlpha() * inv);

			mask |= GL_COLOR_BUFFER_BIT;
		}

		if (zBuffer)
		{
			glDepthMask(GL_TRUE);
			LastMaterial.ZWriteEnable = true;
			mask |= GL_DEPTH_BUFFER_BIT;
		}

		glClear(mask);
		testGLError();
		return true;
	}


	//! Returns the transformation set by setTransform
	const core::matrix4& COGLES2Driver::getTransform(E_TRANSFORMATION_STATE state) const
	{
		return Matrices[state];
	}


	//! sets transformation
	void COGLES2Driver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
	{
		Matrices[state] = mat;
		Transformation3DChanged = true;
	}


	bool COGLES2Driver::updateVertexHardwareBuffer(SHWBufferLink_opengl *HWBuffer)
	{
		if (!HWBuffer)
			return false;

		const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
		const void* vertices = mb->getVertices();
		const u32 vertexCount = mb->getVertexCount();
		const E_VERTEX_TYPE vType = mb->getVertexType();
		const u32 vertexSize = getVertexPitchFromType(vType);

		//buffer vertex data, and convert colours...
		core::array<c8> buffer(vertexSize * vertexCount);
		memcpy(buffer.pointer(), vertices, vertexSize * vertexCount);

		//get or create buffer
		bool newBuffer = false;
		if (!HWBuffer->vbo_verticesID)
		{
			glGenBuffers(1, &HWBuffer->vbo_verticesID);
			if (!HWBuffer->vbo_verticesID) return false;
			newBuffer = true;
		}
		else if (HWBuffer->vbo_verticesSize < vertexCount*vertexSize)
		{
			newBuffer = true;
		}

		glBindBuffer(GL_ARRAY_BUFFER, HWBuffer->vbo_verticesID);

		//copy data to graphics card
		glGetError(); // clear error storage
		if (!newBuffer)
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * vertexSize, buffer.const_pointer());
		else
		{
			HWBuffer->vbo_verticesSize = vertexCount * vertexSize;

			if (HWBuffer->Mapped_Vertex == scene::EHM_STATIC)
				glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, buffer.const_pointer(), GL_STATIC_DRAW);
			else
				glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, buffer.const_pointer(), GL_DYNAMIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return (glGetError() == GL_NO_ERROR);
	}


	bool COGLES2Driver::updateIndexHardwareBuffer(SHWBufferLink_opengl *HWBuffer)
	{
		if (!HWBuffer)
			return false;

		const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;

		const void* indices = mb->getIndices();
		u32 indexCount = mb->getIndexCount();

		GLenum indexSize;
		switch (mb->getIndexType())
		{
			case(EIT_16BIT):
			{
				indexSize = sizeof(u16);
				break;
			}
			case(EIT_32BIT):
			{
				indexSize = sizeof(u32);
				break;
			}
			default:
			{
				return false;
			}
		}

		//get or create buffer
		bool newBuffer = false;
		if (!HWBuffer->vbo_indicesID)
		{
			glGenBuffers(1, &HWBuffer->vbo_indicesID);
			if (!HWBuffer->vbo_indicesID) return false;
			newBuffer = true;
		}
		else if (HWBuffer->vbo_indicesSize < indexCount*indexSize)
		{
			newBuffer = true;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HWBuffer->vbo_indicesID);

		//copy data to graphics card
		glGetError(); // clear error storage
		if (!newBuffer)
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexCount * indexSize, indices);
		else
		{
			HWBuffer->vbo_indicesSize = indexCount * indexSize;

			if (HWBuffer->Mapped_Index == scene::EHM_STATIC)
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_STATIC_DRAW);
			else
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_DYNAMIC_DRAW);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return (glGetError() == GL_NO_ERROR);
	}


	//! updates hardware buffer if needed
	bool COGLES2Driver::updateHardwareBuffer(SHWBufferLink *HWBuffer)
	{
		if (!HWBuffer)
			return false;

		if (HWBuffer->Mapped_Vertex != scene::EHM_NEVER)
		{
			if (HWBuffer->ChangedID_Vertex != HWBuffer->MeshBuffer->getChangedID_Vertex()
				|| !((SHWBufferLink_opengl*)HWBuffer)->vbo_verticesID)
			{

				HWBuffer->ChangedID_Vertex = HWBuffer->MeshBuffer->getChangedID_Vertex();

				if (!updateVertexHardwareBuffer((SHWBufferLink_opengl*)HWBuffer))
					return false;
			}
		}

		if (HWBuffer->Mapped_Index != scene::EHM_NEVER)
		{
			if (HWBuffer->ChangedID_Index != HWBuffer->MeshBuffer->getChangedID_Index()
				|| !((SHWBufferLink_opengl*)HWBuffer)->vbo_indicesID)
			{

				HWBuffer->ChangedID_Index = HWBuffer->MeshBuffer->getChangedID_Index();

				if (!updateIndexHardwareBuffer((SHWBufferLink_opengl*)HWBuffer))
					return false;
			}
		}

		return true;
	}


	//! Create hardware buffer from meshbuffer
	COGLES2Driver::SHWBufferLink *COGLES2Driver::createHardwareBuffer(const scene::IMeshBuffer* mb)
	{
		if (!mb || (mb->getHardwareMappingHint_Index() == scene::EHM_NEVER && mb->getHardwareMappingHint_Vertex() == scene::EHM_NEVER))
			return 0;

		SHWBufferLink_opengl *HWBuffer = new SHWBufferLink_opengl(mb);

		//add to map
		HWBufferMap.insert(HWBuffer->MeshBuffer, HWBuffer);

		HWBuffer->ChangedID_Vertex = HWBuffer->MeshBuffer->getChangedID_Vertex();
		HWBuffer->ChangedID_Index = HWBuffer->MeshBuffer->getChangedID_Index();
		HWBuffer->Mapped_Vertex = mb->getHardwareMappingHint_Vertex();
		HWBuffer->Mapped_Index = mb->getHardwareMappingHint_Index();
		HWBuffer->LastUsed = 0;
		HWBuffer->vbo_verticesID = 0;
		HWBuffer->vbo_indicesID = 0;
		HWBuffer->vbo_verticesSize = 0;
		HWBuffer->vbo_indicesSize = 0;

		if (!updateHardwareBuffer(HWBuffer))
		{
			deleteHardwareBuffer(HWBuffer);
			return 0;
		}

		return HWBuffer;
	}


	void COGLES2Driver::deleteHardwareBuffer(SHWBufferLink *_HWBuffer)
	{
		if (!_HWBuffer)
			return;

		SHWBufferLink_opengl *HWBuffer = (SHWBufferLink_opengl*)_HWBuffer;
		if (HWBuffer->vbo_verticesID)
		{
			glDeleteBuffers(1, &HWBuffer->vbo_verticesID);
			HWBuffer->vbo_verticesID = 0;
		}
		if (HWBuffer->vbo_indicesID)
		{
			glDeleteBuffers(1, &HWBuffer->vbo_indicesID);
			HWBuffer->vbo_indicesID = 0;
		}

		CNullDriver::deleteHardwareBuffer(_HWBuffer);
	}


	//! Draw hardware buffer
	void COGLES2Driver::drawHardwareBuffer(SHWBufferLink *_HWBuffer)
	{
		if (!_HWBuffer)
			return;

		SHWBufferLink_opengl *HWBuffer = (SHWBufferLink_opengl*)_HWBuffer;

		updateHardwareBuffer(HWBuffer); //check if update is needed

		HWBuffer->LastUsed = 0;//reset count

		const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
		const void *vertices = mb->getVertices();
		const void *indexList = mb->getIndices();

		if (HWBuffer->Mapped_Vertex != scene::EHM_NEVER)
		{
			glBindBuffer(GL_ARRAY_BUFFER, HWBuffer->vbo_verticesID);
			vertices = 0;
		}

		if (HWBuffer->Mapped_Index != scene::EHM_NEVER)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HWBuffer->vbo_indicesID);
			indexList = 0;
		}


		drawVertexPrimitiveList(vertices, mb->getVertexCount(),
				indexList, mb->getIndexCount() / 3,
				mb->getVertexType(), scene::EPT_TRIANGLES,
				mb->getIndexType());

		if (HWBuffer->Mapped_Vertex != scene::EHM_NEVER)
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (HWBuffer->Mapped_Index != scene::EHM_NEVER)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	// small helper function to create vertex buffer object adress offsets
	static inline u8* buffer_offset(const long offset)
	{
		return ((u8*)0 + offset);
	}


	//! draws a vertex primitive list
	void COGLES2Driver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
			const void* indexList, u32 primitiveCount,
			E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
	{
		testGLError();
		if (!checkPrimitiveCount(primitiveCount))
			return;

		setRenderStates3DMode();

		drawVertexPrimitiveList2d3d(vertices, vertexCount, (const u16*)indexList, primitiveCount, vType, pType, iType);

		if (static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType].Renderer->PostRender(this, video::EVT_STANDARD);
	}


	void COGLES2Driver::drawVertexPrimitiveList2d3d(const void* vertices, u32 vertexCount,
			const void* indexList, u32 primitiveCount,
			E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType, bool threed)
	{
		if (!primitiveCount || !vertexCount)
			return;

		if (!threed && !checkPrimitiveCount(primitiveCount))
			return;

		CNullDriver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);

		//TODO: treat #ifdef GL_OES_point_size_array outside this if
		if (NoHighLevelShader)
		{
			glEnableVertexAttribArray(EVA_COLOR);
			glEnableVertexAttribArray(EVA_POSITION);
			if ((pType != scene::EPT_POINTS) && (pType != scene::EPT_POINT_SPRITES))
			{
				glEnableVertexAttribArray(EVA_TCOORD0);
			}
#ifdef GL_OES_point_size_array
			else if (FeatureAvailable[IRR_OES_point_size_array] && (Material.Thickness == 0.0f))
				glEnableClientState(GL_POINT_SIZE_ARRAY_OES);
#endif
			if (threed && (pType != scene::EPT_POINTS) && (pType != scene::EPT_POINT_SPRITES))
			{
				glEnableVertexAttribArray(EVA_NORMAL);
			}

			switch (vType)
			{
			case EVT_STANDARD:
				if (vertices)
				{
#ifdef GL_OES_point_size_array
					if ((pType == scene::EPT_POINTS) || (pType == scene::EPT_POINT_SPRITES))
					{
						if (FeatureAvailable[IRR_OES_point_size_array] && (Material.Thickness == 0.0f))
							glPointSizePointerOES(GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Normal.X);
					}
					else
#endif
						glVertexAttribPointer(EVA_POSITION, (threed ? 3 : 2), GL_FLOAT, false, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Pos);
					if (threed)
						glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Normal);
					glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Color);
					glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);

				}
				else
				{
					glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex), 0);
					glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex), buffer_offset(12));
					glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex), buffer_offset(24));
					glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex), buffer_offset(28));
				}

				if (CurrentTexture[1])
				{
					// There must be some optimisation here as it uses the same texture coord !
					glEnableVertexAttribArray(EVA_TCOORD1);
					if (vertices)
						glVertexAttribPointer(EVA_TCOORD1, 2, GL_FLOAT, false, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);
					else
						glVertexAttribPointer(EVA_TCOORD1, 2, GL_FLOAT, false, sizeof(S3DVertex), buffer_offset(28));
				}
				break;
			case EVT_2TCOORDS:
				glEnableVertexAttribArray(EVA_TCOORD1);
				if (vertices)
				{
					glVertexAttribPointer(EVA_POSITION, (threed ? 3 : 2), GL_FLOAT, false, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Pos);
					if (threed)
						glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Normal);
					glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Color);
					glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords);
					glVertexAttribPointer(EVA_TCOORD1, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords2);
				}
				else
				{
					glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(0));
					glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(12));
					glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex2TCoords), buffer_offset(24));
					glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(28));
					glVertexAttribPointer(EVA_TCOORD1, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(36));

				}
				break;
			case EVT_TANGENTS:
				glEnableVertexAttribArray(EVA_TANGENT);
				glEnableVertexAttribArray(EVA_BINORMAL);
				if (vertices)
				{
					glVertexAttribPointer(EVA_POSITION, (threed ? 3 : 2), GL_FLOAT, false, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Pos);
					if (threed)
						glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Normal);
					glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Color);
					glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].TCoords);
					glVertexAttribPointer(EVA_TANGENT, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Tangent);
					glVertexAttribPointer(EVA_BINORMAL, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Binormal);
				}
				else
				{
					glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), buffer_offset(0));
					glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), buffer_offset(12));
					glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertexTangents), buffer_offset(24));
					glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertexTangents), buffer_offset(28));
					glVertexAttribPointer(EVA_TANGENT, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), buffer_offset(36));
					glVertexAttribPointer(EVA_BINORMAL, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), buffer_offset(48));
				}
				break;
			}
		}

		// draw everything
		GLenum indexSize = 0;

		switch (iType)
		{
			case(EIT_16BIT):
			{
				indexSize = GL_UNSIGNED_SHORT;
				break;
			}
			case(EIT_32BIT):
			{
#ifdef GL_OES_element_index_uint
#ifndef GL_UNSIGNED_INT
#define GL_UNSIGNED_INT 0x1405
#endif
				if (FeatureAvailable[IRR_OES_element_index_uint])
					indexSize = GL_UNSIGNED_INT;
				else
#endif
					indexSize = GL_UNSIGNED_SHORT;
				break;
			}
		}

		switch (pType)
		{
			case scene::EPT_POINTS:
			case scene::EPT_POINT_SPRITES:
			{
#ifdef GL_OES_point_sprite
				if (pType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_OES_point_sprite])
					glEnable(GL_POINT_SPRITE_OES);
#endif
				// if ==0 we use the point size array
				if (Material.Thickness != 0.f)
				{
//						float quadratic[] = {0.0f, 0.0f, 10.01f};
					//TODO : OpenGL ES 2.0 Port GL_POINT_DISTANCE_ATTENUATION
					//glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic);
//						float maxParticleSize = 1.0f;
					//TODO : OpenGL ES 2.0 Port GL_POINT_SIZE_MAX
					//glGetFloatv(GL_POINT_SIZE_MAX, &maxParticleSize);
//			maxParticleSize=maxParticleSize<Material.Thickness?maxParticleSize:Material.Thickness;
//			glPointParameterf(GL_POINT_SIZE_MAX,maxParticleSize);
//			glPointParameterf(GL_POINT_SIZE_MIN,Material.Thickness);
					//TODO : OpenGL ES 2.0 Port GL_POINT_FADE_THRESHOLD_SIZE
					//glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 60.0f);
					//glPointSize(Material.Thickness);
				}
#ifdef GL_OES_point_sprite
				if (pType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_OES_point_sprite])
					glTexEnvf(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
#endif
				glDrawArrays(GL_POINTS, 0, primitiveCount);
#ifdef GL_OES_point_sprite
				if (pType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_OES_point_sprite])
				{
					glDisable(GL_POINT_SPRITE_OES);
					glTexEnvf(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);
				}
#endif
			}
			break;
			case scene::EPT_LINE_STRIP:
				glDrawElements(GL_LINE_STRIP, primitiveCount + 1, indexSize, indexList);
				break;
			case scene::EPT_LINE_LOOP:
				glDrawElements(GL_LINE_LOOP, primitiveCount, indexSize, indexList);
				break;
			case scene::EPT_LINES:
				glDrawElements(GL_LINES, primitiveCount*2, indexSize, indexList);
				break;
			case scene::EPT_TRIANGLE_STRIP:
				glDrawElements(GL_TRIANGLE_STRIP, primitiveCount + 2, indexSize, indexList);
				break;
			case scene::EPT_TRIANGLE_FAN:
				glDrawElements(GL_TRIANGLE_FAN, primitiveCount + 2, indexSize, indexList);
				break;
			case scene::EPT_TRIANGLES:
				glDrawElements((LastMaterial.Wireframe) ? GL_LINES : (LastMaterial.PointCloud) ? GL_POINTS : GL_TRIANGLES, primitiveCount*3, indexSize, indexList);
				break;
			case scene::EPT_QUAD_STRIP:
// TODO ogl-es
//		glDrawElements(GL_QUAD_STRIP, primitiveCount*2+2, indexSize, indexList);
				break;
			case scene::EPT_QUADS:
// TODO ogl-es
//		glDrawElements(GL_QUADS, primitiveCount*4, indexSize, indexList);
				break;
			case scene::EPT_POLYGON:
// TODO ogl-es
//		glDrawElements(GL_POLYGON, primitiveCount, indexSize, indexList);
				break;
		}

		if (NoHighLevelShader)
		{
			if (vType == EVT_TANGENTS)
			{
				glDisableVertexAttribArray(EVA_TANGENT);
				glDisableVertexAttribArray(EVA_BINORMAL);
			}
			if ((vType != EVT_STANDARD) || CurrentTexture[1])
			{
				glDisableVertexAttribArray(EVA_TCOORD1);
			}

#ifdef GL_OES_point_size_array
			if (FeatureAvailable[IRR_OES_point_size_array] && (Material.Thickness == 0.0f))
				glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
#endif
			glDisableVertexAttribArray(EVA_POSITION);
			glDisableVertexAttribArray(EVA_NORMAL);
			glDisableVertexAttribArray(EVA_COLOR);
			glDisableVertexAttribArray(EVA_TCOORD0);
		}
		testGLError();
	}


	//! draws a 2d image, using a color and the alpha channel of the texture
	void COGLES2Driver::draw2DImage(const video::ITexture* texture,
			const core::position2d<s32>& pos,
			const core::rect<s32>& sourceRect,
			const core::rect<s32>* clipRect, SColor color,
			bool useAlphaChannelOfTexture)
	{
		if (!texture)
			return;

		if (!sourceRect.isValid())
			return;

		core::position2d<s32> targetPos(pos);
		core::position2d<s32> sourcePos(sourceRect.UpperLeftCorner);
		core::dimension2d<s32> sourceSize(sourceRect.getSize());
		if (clipRect)
		{
			if (targetPos.X < clipRect->UpperLeftCorner.X)
			{
				sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
				if (sourceSize.Width <= 0)
					return;

				sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
				targetPos.X = clipRect->UpperLeftCorner.X;
			}

			if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
			{
				sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
				if (sourceSize.Width <= 0)
					return;
			}

			if (targetPos.Y < clipRect->UpperLeftCorner.Y)
			{
				sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
				if (sourceSize.Height <= 0)
					return;

				sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
				targetPos.Y = clipRect->UpperLeftCorner.Y;
			}

			if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
			{
				sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
				if (sourceSize.Height <= 0)
					return;
			}
		}

		// clip these coordinates

		if (targetPos.X < 0)
		{
			sourceSize.Width += targetPos.X;
			if (sourceSize.Width <= 0)
				return;

			sourcePos.X -= targetPos.X;
			targetPos.X = 0;
		}

		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

		if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
			if (sourceSize.Width <= 0)
				return;
		}

		if (targetPos.Y < 0)
		{
			sourceSize.Height += targetPos.Y;
			if (sourceSize.Height <= 0)
				return;

			sourcePos.Y -= targetPos.Y;
			targetPos.Y = 0;
		}

		if (targetPos.Y + sourceSize.Height > (s32)renderTargetSize.Height)
		{
			sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
			if (sourceSize.Height <= 0)
				return;
		}

		// ok, we've clipped everything.
		// now draw it.

		// texcoords need to be flipped horizontally for RTTs
		const bool isRTT = texture->isRenderTarget();
		const core::dimension2d<u32>& ss = texture->getOriginalSize();
		const f32 invW = 1.f / static_cast<f32>(ss.Width);
		const f32 invH = 1.f / static_cast<f32>(ss.Height);
		const core::rect<f32> tcoords(
			sourcePos.X * invW,
			(isRTT ? (sourcePos.Y + sourceSize.Height) : sourcePos.Y) * invH,
			(sourcePos.X + sourceSize.Width) * invW,
			(isRTT ? sourcePos.Y : (sourcePos.Y + sourceSize.Height)) * invH);

		const core::rect<s32> poss(targetPos, sourceSize);

		disableTextures(1);
		if (!setActiveTexture(0, texture))
			return;
		setRenderStates2DMode(color.getAlpha() < 255, true, useAlphaChannelOfTexture);

		u16 indices[] = {0, 1, 2, 3};
		S3DVertex vertices[4];
		vertices[0] = S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		vertices[1] = S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		vertices[2] = S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		vertices[3] = S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
		drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);
	}


	void COGLES2Driver::draw2DImageBatch(const video::ITexture* texture,
			const core::array<core::position2d<s32> >& positions,
			const core::array<core::rect<s32> >& sourceRects,
			const core::rect<s32>* clipRect,
			SColor color, bool useAlphaChannelOfTexture)
	{
		if (!texture)
			return;

		if (!setActiveTexture(0, const_cast<video::ITexture*>(texture)))
			return;

		const irr::u32 drawCount = core::min_<u32>(positions.size(), sourceRects.size());

		core::array<S3DVertex> vtx(drawCount * 4);
		core::array<u16> indices(drawCount * 6);

		for (u32 i = 0; i < drawCount; i++)
		{
			core::position2d<s32> targetPos = positions[i];
			core::position2d<s32> sourcePos = sourceRects[i].UpperLeftCorner;
			// This needs to be signed as it may go negative.
			core::dimension2d<s32> sourceSize(sourceRects[i].getSize());

			if (clipRect)
			{
				if (targetPos.X < clipRect->UpperLeftCorner.X)
				{
					sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
					if (sourceSize.Width <= 0)
						continue;

					sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
					targetPos.X = clipRect->UpperLeftCorner.X;
				}

				if (targetPos.X + (s32)sourceSize.Width > clipRect->LowerRightCorner.X)
				{
					sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
					if (sourceSize.Width <= 0)
						continue;
				}

				if (targetPos.Y < clipRect->UpperLeftCorner.Y)
				{
					sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
					if (sourceSize.Height <= 0)
						continue;

					sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
					targetPos.Y = clipRect->UpperLeftCorner.Y;
				}

				if (targetPos.Y + (s32)sourceSize.Height > clipRect->LowerRightCorner.Y)
				{
					sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
					if (sourceSize.Height <= 0)
						continue;
				}
			}

			// clip these coordinates

			if (targetPos.X < 0)
			{
				sourceSize.Width += targetPos.X;
				if (sourceSize.Width <= 0)
					continue;

				sourcePos.X -= targetPos.X;
				targetPos.X = 0;
			}

			const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

			if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
			{
				sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
				if (sourceSize.Width <= 0)
					continue;
			}

			if (targetPos.Y < 0)
			{
				sourceSize.Height += targetPos.Y;
				if (sourceSize.Height <= 0)
					continue;

				sourcePos.Y -= targetPos.Y;
				targetPos.Y = 0;
			}

			if (targetPos.Y + sourceSize.Height > (s32)renderTargetSize.Height)
			{
				sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
				if (sourceSize.Height <= 0)
					continue;
			}

			// ok, we've clipped everything.
			// now draw it.

			core::rect<f32> tcoords;
			tcoords.UpperLeftCorner.X = (((f32)sourcePos.X)) / texture->getOriginalSize().Width ;
			tcoords.UpperLeftCorner.Y = (((f32)sourcePos.Y)) / texture->getOriginalSize().Height;
			tcoords.LowerRightCorner.X = tcoords.UpperLeftCorner.X + ((f32)(sourceSize.Width) / texture->getOriginalSize().Width);
			tcoords.LowerRightCorner.Y = tcoords.UpperLeftCorner.Y + ((f32)(sourceSize.Height) / texture->getOriginalSize().Height);

			const core::rect<s32> poss(targetPos, sourceSize);

			setRenderStates2DMode(color.getAlpha() < 255, true, useAlphaChannelOfTexture);

			vtx.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f,
					0.0f, 0.0f, 0.0f, color,
					tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));
			vtx.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f,
					0.0f, 0.0f, 0.0f, color,
					tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));
			vtx.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f,
					0.0f, 0.0f, 0.0f, color,
					tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));
			vtx.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f,
					0.0f, 0.0f, 0.0f, color,
					tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));

			const u32 curPos = vtx.size() - 4;
			indices.push_back(0 + curPos);
			indices.push_back(1 + curPos);
			indices.push_back(2 + curPos);

			indices.push_back(0 + curPos);
			indices.push_back(2 + curPos);
			indices.push_back(3 + curPos);
		}

		if (vtx.size())
		{
			drawVertexPrimitiveList2d3d(vtx.pointer(), vtx.size(),
				indices.pointer(), indices.size() / 3,
				EVT_STANDARD, scene::EPT_TRIANGLES,
				EIT_16BIT, false);
		}
	}


	//! The same, but with a four element array of colors, one for each vertex
	void COGLES2Driver::draw2DImage(const video::ITexture* texture,
			const core::rect<s32>& destRect,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
			const video::SColor* const colors, bool useAlphaChannelOfTexture)
	{
		if (!texture)
			return;

		// texcoords need to be flipped horizontally for RTTs
		const bool isRTT = texture->isRenderTarget();
		const core::dimension2du& ss = texture->getOriginalSize();
		const f32 invW = 1.f / static_cast<f32>(ss.Width);
		const f32 invH = 1.f / static_cast<f32>(ss.Height);
		const core::rect<f32> tcoords(
			sourceRect.UpperLeftCorner.X * invW,
			(isRTT ? sourceRect.LowerRightCorner.Y : sourceRect.UpperLeftCorner.Y) * invH,
			sourceRect.LowerRightCorner.X * invW,
			(isRTT ? sourceRect.UpperLeftCorner.Y : sourceRect.LowerRightCorner.Y) *invH);

		const video::SColor temp[4] =
		{
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF
		};

		const video::SColor* const useColor = colors ? colors : temp;

		disableTextures(1);
		setActiveTexture(0, texture);
		setRenderStates2DMode(useColor[0].getAlpha() < 255 || useColor[1].getAlpha() < 255 ||
							useColor[2].getAlpha() < 255 || useColor[3].getAlpha() < 255,
							true, useAlphaChannelOfTexture);

		if (clipRect)
		{
			if (!clipRect->isValid())
				return;

			glEnable(GL_SCISSOR_TEST);
			const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
			glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height - clipRect->LowerRightCorner.Y,
					clipRect->getWidth(), clipRect->getHeight());
		}

		u16 indices[] = {0, 1, 2, 3};
		S3DVertex vertices[4];
		vertices[0] = S3DVertex((f32)destRect.UpperLeftCorner.X, (f32)destRect.UpperLeftCorner.Y, 0, 0, 0, 1, useColor[0], tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		vertices[1] = S3DVertex((f32)destRect.LowerRightCorner.X, (f32)destRect.UpperLeftCorner.Y, 0, 0, 0, 1, useColor[3], tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		vertices[2] = S3DVertex((f32)destRect.LowerRightCorner.X, (f32)destRect.LowerRightCorner.Y, 0, 0, 0, 1, useColor[2], tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		vertices[3] = S3DVertex((f32)destRect.UpperLeftCorner.X, (f32)destRect.LowerRightCorner.Y, 0, 0, 0, 1, useColor[1], tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
		drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);

		if (clipRect)
			glDisable(GL_SCISSOR_TEST);
		testGLError();
	}


	//! draws a set of 2d images, using a color and the alpha channel
	void COGLES2Driver::draw2DImageBatch(const video::ITexture* texture,
			const core::position2d<s32>& pos,
			const core::array<core::rect<s32> >& sourceRects,
			const core::array<s32>& indices, s32 kerningWidth,
			const core::rect<s32>* clipRect, SColor color,
			bool useAlphaChannelOfTexture)
	{
		if (!texture)
			return;

		disableTextures(1);
		if (!setActiveTexture(0, texture))
			return;
		setRenderStates2DMode(color.getAlpha() < 255, true, useAlphaChannelOfTexture);

		if (clipRect)
		{
			if (!clipRect->isValid())
				return;

			glEnable(GL_SCISSOR_TEST);
			const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
			glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height - clipRect->LowerRightCorner.Y,
					clipRect->getWidth(), clipRect->getHeight());
		}

		const core::dimension2du& ss = texture->getOriginalSize();
		core::position2d<s32> targetPos(pos);
		// texcoords need to be flipped horizontally for RTTs
		const bool isRTT = texture->isRenderTarget();
		const f32 invW = 1.f / static_cast<f32>(ss.Width);
		const f32 invH = 1.f / static_cast<f32>(ss.Height);

		core::array<S3DVertex> vertices;
		core::array<u16> quadIndices;
		vertices.reallocate(indices.size()*4);
		quadIndices.reallocate(indices.size()*3);
		for (u32 i = 0; i < indices.size(); ++i)
		{
			const s32 currentIndex = indices[i];
			if (!sourceRects[currentIndex].isValid())
				break;

			const core::rect<f32> tcoords(
				sourceRects[currentIndex].UpperLeftCorner.X * invW,
				(isRTT ? sourceRects[currentIndex].LowerRightCorner.Y : sourceRects[currentIndex].UpperLeftCorner.Y) * invH,
				sourceRects[currentIndex].LowerRightCorner.X * invW,
				(isRTT ? sourceRects[currentIndex].UpperLeftCorner.Y : sourceRects[currentIndex].LowerRightCorner.Y) * invH);

			const core::rect<s32> poss(targetPos, sourceRects[currentIndex].getSize());

			vertices.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));
			vertices.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));
			vertices.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));
			vertices.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));

			targetPos.X += sourceRects[currentIndex].getWidth();
		}
		drawVertexPrimitiveList2d3d(vertices.pointer(), indices.size()*4, quadIndices.pointer(), 2*indices.size(), video::EVT_STANDARD, scene::EPT_TRIANGLES, EIT_16BIT, false);
		if (clipRect)
			glDisable(GL_SCISSOR_TEST);
		testGLError();
	}


	//! draw a 2d rectangle
	void COGLES2Driver::draw2DRectangle(SColor color,
			const core::rect<s32>& position,
			const core::rect<s32>* clip)
	{
		disableTextures();
		setRenderStates2DMode(color.getAlpha() < 255, false, false);

		core::rect<s32> pos = position;

		if (clip)
			pos.clipAgainst(*clip);

		if (!pos.isValid())
			return;

		u16 indices[] = {0, 1, 2, 3};
		S3DVertex vertices[4];
		vertices[0] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[1] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[2] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[3] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 0, 0);
		drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);
	}


	//! draw an 2d rectangle
	void COGLES2Driver::draw2DRectangle(const core::rect<s32>& position,
			SColor colorLeftUp, SColor colorRightUp,
			SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip)
	{
		core::rect<s32> pos = position;

		if (clip)
			pos.clipAgainst(*clip);

		if (!pos.isValid())
			return;

		disableTextures();

		setRenderStates2DMode(colorLeftUp.getAlpha() < 255 ||
				colorRightUp.getAlpha() < 255 ||
				colorLeftDown.getAlpha() < 255 ||
				colorRightDown.getAlpha() < 255, false, false);

		u16 indices[] = {0, 1, 2, 3};
		S3DVertex vertices[4];
		vertices[0] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0, 0, 1, colorLeftUp, 0, 0);
		vertices[1] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0, 0, 1, colorRightUp, 0, 0);
		vertices[2] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0, 0, 1, colorRightDown, 0, 0);
		vertices[3] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0, 0, 1, colorLeftDown, 0, 0);
		drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);
	}


	//! Draws a 2d line.
	void COGLES2Driver::draw2DLine(const core::position2d<s32>& start,
			const core::position2d<s32>& end, SColor color)
	{
		disableTextures();
		setRenderStates2DMode(color.getAlpha() < 255, false, false);

		u16 indices[] = {0, 1};
		S3DVertex vertices[2];
		vertices[0] = S3DVertex((f32)start.X, (f32)start.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[1] = S3DVertex((f32)end.X, (f32)end.Y, 0, 0, 0, 1, color, 1, 1);
		drawVertexPrimitiveList2d3d(vertices, 2, indices, 1, video::EVT_STANDARD, scene::EPT_LINES, EIT_16BIT, false);
	}


	//! Draws a pixel
	void COGLES2Driver::drawPixel(u32 x, u32 y, const SColor &color)
	{
		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
		if (x > (u32)renderTargetSize.Width || y > (u32)renderTargetSize.Height)
			return;

		disableTextures();
		setRenderStates2DMode(color.getAlpha() < 255, false, false);

		u16 indices[] = {0};
		S3DVertex vertices[1];
		vertices[0] = S3DVertex((f32)x, (f32)y, 0, 0, 0, 1, color, 0, 0);
		drawVertexPrimitiveList2d3d(vertices, 1, indices, 1, video::EVT_STANDARD, scene::EPT_POINTS, EIT_16BIT, false);
	}


	bool COGLES2Driver::setActiveTexture(u32 stage, const video::ITexture* texture)
	{
		if (stage >= MaxTextureUnits)
			return false;

		if (CurrentTexture[stage] == texture)
			return true;

		glActiveTexture(GL_TEXTURE0 + stage);

		CurrentTexture[stage] = texture;

		if (!texture)
		{
			return true;
		}
		else
		{
			if (texture->getDriverType() != EDT_OGLES2)
			{
				os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
				return false;
			}
			glBindTexture(GL_TEXTURE_2D,
						static_cast<const COGLES2Texture*>(texture)->getOGLES2TextureName());
		}
		testGLError();
		return true;
	}


	//! disables all textures beginning with the optional fromStage parameter.
	bool COGLES2Driver::disableTextures(u32 fromStage)
	{
		bool result = true;
		for (u32 i = fromStage; i < MaxTextureUnits; ++i)
			result &= setActiveTexture(i, 0);
		return result;
	}


	//! creates a matrix in supplied GLfloat array to pass to OGLES1
	inline void COGLES2Driver::createGLMatrix(float gl_matrix[16], const core::matrix4& m)
	{
		memcpy(gl_matrix, m.pointer(), 16 * sizeof(f32));
	}


	//! creates a opengltexturematrix from a D3D style texture matrix
	inline void COGLES2Driver::createGLTextureMatrix(float *o, const core::matrix4& m)
	{
		o[0] = m[0];
		o[1] = m[1];
		o[2] = 0.f;
		o[3] = 0.f;

		o[4] = m[4];
		o[5] = m[5];
		o[6] = 0.f;
		o[7] = 0.f;

		o[8] = 0.f;
		o[9] = 0.f;
		o[10] = 1.f;
		o[11] = 0.f;

		o[12] = m[8];
		o[13] = m[9];
		o[14] = 0.f;
		o[15] = 1.f;
	}


	//! returns a device dependent texture from a software surface (IImage)
	video::ITexture* COGLES2Driver::createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData)
	{
		return new COGLES2Texture(surface, name, this);
	}


	//! Sets a material.
	void COGLES2Driver::setMaterial(const SMaterial& material)
	{
		Material = material;
		OverrideMaterial.apply(Material);

		for (s32 i = MaxTextureUnits - 1; i >= 0; --i)
		{
			setActiveTexture(i, Material.getTexture(i));
			setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0 + i),
					Material.getTextureMatrix(i));
		}
	}

	//! prints error if an error happened.
	bool COGLES2Driver::testGLError()
	{
#ifdef _DEBUG
		GLenum g = glGetError();
		switch (g)
		{
			case GL_NO_ERROR:
				return false;
			case GL_INVALID_ENUM:
				os::Printer::log("GL_INVALID_ENUM", ELL_ERROR);
				break;
			case GL_INVALID_VALUE:
				os::Printer::log("GL_INVALID_VALUE", ELL_ERROR);
				break;
			case GL_INVALID_OPERATION:
				os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR);
				break;
			case GL_OUT_OF_MEMORY:
				os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR);
				break;
		};
		return true;
#else
		return false;
#endif
	}


	bool COGLES2Driver::testEGLError()
	{
#if defined(EGL_VERSION_1_0) && defined(_DEBUG)
		EGLint g = eglGetError();
		switch (g)
		{
			case EGL_SUCCESS:
				return false;
			case EGL_NOT_INITIALIZED :
				os::Printer::log("Not Initialized", ELL_ERROR);
				break;
			case EGL_BAD_ACCESS:
				os::Printer::log("Bad Access", ELL_ERROR);
				break;
			case EGL_BAD_ALLOC:
				os::Printer::log("Bad Alloc", ELL_ERROR);
				break;
			case EGL_BAD_ATTRIBUTE:
				os::Printer::log("Bad Attribute", ELL_ERROR);
				break;
			case EGL_BAD_CONTEXT:
				os::Printer::log("Bad Context", ELL_ERROR);
				break;
			case EGL_BAD_CONFIG:
				os::Printer::log("Bad Config", ELL_ERROR);
				break;
			case EGL_BAD_CURRENT_SURFACE:
				os::Printer::log("Bad Current Surface", ELL_ERROR);
				break;
			case EGL_BAD_DISPLAY:
				os::Printer::log("Bad Display", ELL_ERROR);
				break;
			case EGL_BAD_SURFACE:
				os::Printer::log("Bad Surface", ELL_ERROR);
				break;
			case EGL_BAD_MATCH:
				os::Printer::log("Bad Match", ELL_ERROR);
				break;
			case EGL_BAD_PARAMETER:
				os::Printer::log("Bad Parameter", ELL_ERROR);
				break;
			case EGL_BAD_NATIVE_PIXMAP:
				os::Printer::log("Bad Native Pixmap", ELL_ERROR);
				break;
			case EGL_BAD_NATIVE_WINDOW:
				os::Printer::log("Bad Native Window", ELL_ERROR);
				break;
			case EGL_CONTEXT_LOST:
				os::Printer::log("Context Lost", ELL_ERROR);
				break;
		};
		return true;
#else
		return false;
#endif
	}


	//! sets the needed renderstates
	void COGLES2Driver::setRenderStates3DMode()
	{
		if (CurrentRenderMode != ERM_3D)
		{
			// Reset Texture Stages
			if (BlendEnabled)
			{
				glDisable(GL_BLEND);
				BlendEnabled = false;
			}

			ResetRenderStates = true;
		}

		if (ResetRenderStates || LastMaterial != Material)
		{
			// unset old material

			if (LastMaterial.MaterialType != Material.MaterialType &&
				static_cast<u32>(LastMaterial.MaterialType) < MaterialRenderers.size())
				MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();

			// set new material.
			if (static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
				MaterialRenderers[Material.MaterialType].Renderer->OnSetMaterial(
					Material, LastMaterial, ResetRenderStates, this);

			LastMaterial = Material;
			ResetRenderStates = false;
		}

		if (static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType].Renderer->OnRender(this, video::EVT_STANDARD);

		testGLError();

		CurrentRenderMode = ERM_3D;
	}


	GLint COGLES2Driver::getTextureWrapMode(u8 clamp) const
	{
		switch (clamp)
		{
			case ETC_CLAMP:
				// mode=GL_CLAMP; not supported in ogl-es
				return GL_CLAMP_TO_EDGE;
			case ETC_CLAMP_TO_EDGE:
				return GL_CLAMP_TO_EDGE;
			case ETC_CLAMP_TO_BORDER:
				// mode=GL_CLAMP_TO_BORDER; not supported in ogl-es
				return GL_CLAMP_TO_EDGE;
			case ETC_MIRROR:
#ifdef GL_OES_texture_mirrored_repeat
				if (FeatureAvailable[IRR_OES_texture_mirrored_repeat])
					return GL_MIRRORED_REPEAT_OES;
				else
#endif
					return GL_REPEAT;
			default:
				return GL_REPEAT;
		}
	}


	void COGLES2Driver::setWrapMode(const SMaterial& material)
	{
		testGLError();
		// texture address mode
		// Has to be checked always because it depends on the textures
		for (u32 u = 0; u < MaxTextureUnits; ++u)
		{
			if (MultiTextureExtension)
				glActiveTexture(GL_TEXTURE0 + u);
			else if (u>0)
				break; // stop loop

			// the APPLE npot restricted extension needs some care as it only supports CLAMP_TO_EDGE
			if (queryFeature(EVDF_TEXTURE_NPOT) && !FeatureAvailable[IRR_OES_texture_npot] &&
					CurrentTexture[u] && (CurrentTexture[u]->getSize() != CurrentTexture[u]->getOriginalSize()))
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrapMode(material.TextureLayer[u].TextureWrapU));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrapMode(material.TextureLayer[u].TextureWrapV));
			}
		}
	}


	//! Can be called by an IMaterialRenderer to make its work easier.
	void COGLES2Driver::setBasicRenderStates(const SMaterial& material,
			const SMaterial& lastmaterial, bool resetAllRenderStates)
	{
		testGLError();
		// Texture filter
		// Has to be checked always because it depends on the textures
		// Filtering has to be set for each texture layer
		for (u32 i = 0; i < MaxTextureUnits; ++i)
		{
			if (!CurrentTexture[i])
				continue;
			glActiveTexture(GL_TEXTURE0 + i);

#ifdef GL_EXT_texture_lod_bias
			if (FeatureAvailable[IRR_EXT_texture_lod_bias])
			{
				if (material.TextureLayer[i].LODBias)
				{
					const float tmp = core::clamp(material.TextureLayer[i].LODBias * 0.125f, -MaxTextureLODBias, MaxTextureLODBias);
					glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, tmp);
				}
				else
					glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0.f);
			}
#endif
			if (material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

			if (material.getTexture(i) && CurrentTexture[i]->hasMipMaps())
			{
				if (material.TextureLayer[i].TrilinearFilter)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				}
				else if (material.TextureLayer[i].BilinearFilter)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				}
			}
			else if (material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}

#ifdef GL_EXT_texture_filter_anisotropic
			if (FeatureAvailable[IRR_EXT_texture_filter_anisotropic])
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
						static_cast<GLfloat>(material.TextureLayer[i].AnisotropicFilter > 1 ? core::min_(MaxAnisotropy, material.TextureLayer[i].AnisotropicFilter) : 1));
#endif
		}
		testGLError();

		// fillmode
		// for ogl-es this is emulated by other polygon primitives during rendering

		// shademode
		if (resetAllRenderStates || (lastmaterial.GouraudShading != material.GouraudShading))
		{
			//TODO : OpenGL ES 2.0 Port glShadeModel
			//if (material.GouraudShading)
			// glShadeModel(GL_SMOOTH);
			//else
			// glShadeModel(GL_FLAT);
		}
		testGLError();

		// zbuffer
		if (resetAllRenderStates || lastmaterial.ZBuffer != material.ZBuffer)
		{
			switch (material.ZBuffer)
			{
				case ECFN_NEVER:
					glDisable(GL_DEPTH_TEST);
					break;
				case ECFN_LESSEQUAL:
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LEQUAL);
					break;
				case ECFN_EQUAL:
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_EQUAL);
					break;
				case ECFN_LESS:
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LESS);
					break;
				case ECFN_NOTEQUAL:
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_NOTEQUAL);
					break;
				case ECFN_GREATEREQUAL:
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_GEQUAL);
					break;
				case ECFN_GREATER:
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_GREATER);
					break;
				case ECFN_ALWAYS:
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_ALWAYS);
					break;
			}
		}
		testGLError();

		// zwrite
//		if (resetAllRenderStates || lastmaterial.ZWriteEnable != material.ZWriteEnable)
		{
			if (material.ZWriteEnable && (AllowZWriteOnTransparent || !material.isTransparent()))
			{
				glDepthMask(GL_TRUE);
			}
			else
				glDepthMask(GL_FALSE);
		}

		// back face culling
		if (resetAllRenderStates || (lastmaterial.FrontfaceCulling != material.FrontfaceCulling) || (lastmaterial.BackfaceCulling != material.BackfaceCulling))
		{
			if ((material.FrontfaceCulling) && (material.BackfaceCulling))
			{
				glCullFace(GL_FRONT_AND_BACK);
				glEnable(GL_CULL_FACE);
			}
			else if (material.BackfaceCulling)
			{
				glCullFace(GL_BACK);
				glEnable(GL_CULL_FACE);
			}
			else if (material.FrontfaceCulling)
			{
				glCullFace(GL_FRONT);
				glEnable(GL_CULL_FACE);
			}
			else
				glDisable(GL_CULL_FACE);
		}
		testGLError();

		// Color Mask
		if (resetAllRenderStates || lastmaterial.ColorMask != material.ColorMask)
		{
			glColorMask(
				(material.ColorMask & ECP_RED) ? GL_TRUE : GL_FALSE,
				(material.ColorMask & ECP_GREEN) ? GL_TRUE : GL_FALSE,
				(material.ColorMask & ECP_BLUE) ? GL_TRUE : GL_FALSE,
				(material.ColorMask & ECP_ALPHA) ? GL_TRUE : GL_FALSE);
		}
		testGLError();

		if (resetAllRenderStates|| lastmaterial.BlendOperation != material.BlendOperation)
		{
			if (EBO_NONE)
				glDisable(GL_BLEND);
			else
			{
				glEnable(GL_BLEND);
				switch (material.BlendOperation)
				{
				case EBO_SUBTRACT:
					glBlendEquation(GL_FUNC_SUBTRACT);
					break;
				case EBO_REVSUBTRACT:
					glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
					break;
				case EBO_MIN:
	#if defined(GL_EXT_blend_minmax)
					if (FeatureAvailable[IRR_EXT_blend_minmax])
						glBlendEquation(GL_MIN_EXT);
	#endif
					break;
				case EBO_MAX:
	#if defined(GL_EXT_blend_minmax)
					if (FeatureAvailable[IRR_EXT_blend_minmax])
						glBlendEquation(GL_MAX_EXT);
	#endif
					break;
				case EBO_MIN_FACTOR:
					// fallback in case of missing extension
	#if defined(GL_EXT_blend_minmax)
					if (FeatureAvailable[IRR_EXT_blend_minmax])
						glBlendEquation(GL_MIN_EXT);
	#endif
					break;
				case EBO_MAX_FACTOR:
					// fallback in case of missing extension
	#if defined(GL_EXT_blend_minmax)
					if (FeatureAvailable[IRR_EXT_blend_minmax])
						glBlendEquation(GL_MAX_EXT);
	#endif
					break;
				case EBO_MIN_ALPHA:
	#if defined(GL_EXT_blend_minmax)
					if (FeatureAvailable[IRR_EXT_blend_minmax])
						glBlendEquation(GL_MIN_EXT);
	#endif
					break;
				case EBO_MAX_ALPHA:
	#if defined(GL_EXT_blend_minmax)
					if (FeatureAvailable[IRR_EXT_blend_minmax])
						glBlendEquation(GL_MAX_EXT);
	#endif
					break;
				default:
					glBlendEquation(GL_FUNC_ADD);
					break;
				}
			}
		}

		// Polygon Offset
		if (queryFeature(EVDF_POLYGON_OFFSET) && (resetAllRenderStates ||
			lastmaterial.PolygonOffsetDirection != material.PolygonOffsetDirection ||
			lastmaterial.PolygonOffsetFactor != material.PolygonOffsetFactor))
		{
			if (material.PolygonOffsetFactor)
				glEnable(GL_POLYGON_OFFSET_FILL);
			else
				glDisable(GL_POLYGON_OFFSET_FILL);
			if (material.PolygonOffsetDirection==EPO_BACK)
				glPolygonOffset(1.0f, (GLfloat)material.PolygonOffsetFactor);
			else
				glPolygonOffset(-1.0f, (GLfloat)-material.PolygonOffsetFactor);
		}

		// thickness
		if (resetAllRenderStates || lastmaterial.Thickness != material.Thickness)
		{
			//TODO : OpenGL ES 2.0 Port glPointSize
			//glPointSize(material.Thickness);
			glLineWidth(material.Thickness == 0 ? 1 : material.Thickness);
			//glLineWidth with 0 generate GL_INVALID_VALUE on real hardware.
		}
		testGLError();

		// Anti aliasing
		if (resetAllRenderStates || lastmaterial.AntiAliasing != material.AntiAliasing)
		{
			{
				if (material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
					glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				else if (lastmaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
					glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				// other settings cannot be changed in ogl-es
			}
		}
		testGLError();

		setWrapMode(material);

		glActiveTexture(GL_TEXTURE0);
		testGLError();
	}


	//! sets the needed renderstates
	void COGLES2Driver::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
	{
		if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
		{
			// unset last 3d material
			if (CurrentRenderMode == ERM_3D)
			{
				if (static_cast<u32>(LastMaterial.MaterialType) < MaterialRenderers.size())
					MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();
			}
			if (!OverrideMaterial2DEnabled)
			{
				setBasicRenderStates(InitMaterial2D, LastMaterial, true);
				LastMaterial = InitMaterial2D;
			}

			TwoDRenderer->useProgram(); //Fixed Pipeline Shader needed to render 2D

			if (Transformation3DChanged)
			{
				const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
				core::matrix4 m;
				m.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0, 1.0);
				m.setTranslation(core::vector3df(-1, 1, 0));

				TwoDRenderer->setOrthoMatrix(m);

				Transformation3DChanged = false;
			}
		}
		if (OverrideMaterial2DEnabled)
		{
			OverrideMaterial2D.Lighting=false;
			setBasicRenderStates(OverrideMaterial2D, LastMaterial, false);
			LastMaterial = OverrideMaterial2D;
		}
		if (alphaChannel || alpha)
		{
			if (! BlendEnabled)
			{
				glEnable(GL_BLEND);
				BlendEnabled = true;
			}
			blendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA);
			TwoDRenderer->useAlphaTest(true);
			TwoDRenderer->setAlphaTestValue(0.f);
		}
		else
		{
			if (BlendEnabled)
			{
				glDisable(GL_BLEND);
				BlendEnabled = false;
			}
			TwoDRenderer->useAlphaTest(false);
		}

		if (texture)
		{
			if (!OverrideMaterial2DEnabled)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
		}
		TwoDRenderer->useTexture(texture);

		CurrentRenderMode = ERM_2D;
		testGLError();
	}


	//! \return Returns the name of the video driver.
	const wchar_t* COGLES2Driver::getName() const
	{
		return Name.c_str();
	}


	//! deletes all dynamic lights there are
	void COGLES2Driver::deleteAllDynamicLights()
	{
		RequestedLights.clear();
		CNullDriver::deleteAllDynamicLights();
	}


	//! adds a dynamic light
	s32 COGLES2Driver::addDynamicLight(const SLight& light)
	{
		CNullDriver::addDynamicLight(light);

		RequestedLights.push_back(RequestedLight(light));

		u32 newLightIndex = RequestedLights.size() - 1;

		return (s32)newLightIndex;
	}

	//! Turns a dynamic light on or off
	//! \param lightIndex: the index returned by addDynamicLight
	//! \param turnOn: true to turn the light on, false to turn it off
	void COGLES2Driver::turnLightOn(s32 lightIndex, bool turnOn)
	{
		if (lightIndex < 0 || lightIndex >= (s32)RequestedLights.size())
			return;

		RequestedLight & requestedLight = RequestedLights[lightIndex];
		requestedLight.DesireToBeOn = turnOn;
	}


	//! returns the maximal amount of dynamic lights the device can handle
	u32 COGLES2Driver::getMaximalDynamicLightAmount() const
	{
		return MaxLights;
	}


	//! Sets the dynamic ambient light color.
	void COGLES2Driver::setAmbientLight(const SColorf& color)
	{
		AmbientLight = color;
	}

	//! returns the dynamic ambient light color.
	const SColorf& COGLES2Driver::getAmbientLight() const
	{
		return AmbientLight;
	}

	// this code was sent in by Oliver Klems, thank you
	void COGLES2Driver::setViewPort(const core::rect<s32>& area)
	{
		core::rect<s32> vp = area;
		core::rect<s32> rendert(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
		vp.clipAgainst(rendert);

		if (vp.getHeight() > 0 && vp.getWidth() > 0)
		{
			glViewport(vp.UpperLeftCorner.X,
						getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight(),
						vp.getWidth(), vp.getHeight());
		}

		ViewPort = vp;
		testGLError();
	}


	//! Draws a shadow volume into the stencil buffer.
	void COGLES2Driver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
	{
		if (!StencilBuffer || !count)
			return;

		// unset last 3d material
		if (CurrentRenderMode == ERM_3D &&
			static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
		{
			MaterialRenderers[Material.MaterialType].Renderer->OnUnsetMaterial();
			ResetRenderStates = true;
		}

		// store current OGLES state
		const GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
		GLint cullFaceMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);
		GLint depthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
		GLboolean depthMask;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE); // no depth buffer writing
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // no color buffer drawing
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.0f, 1.0f);

		glEnableVertexAttribArray(EVA_POSITION);

		glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(core::vector3df), &triangles[0]);
		glStencilMask(~0);
		glStencilFunc(GL_ALWAYS, 0, ~0);

		GLenum decr = GL_DECR;
		GLenum incr = GL_INCR;
#if defined(GL_OES_stencil_wrap)
		if (FeatureAvailable[IRR_OES_stencil_wrap])
		{
			decr = GL_DECR_WRAP_OES;
			incr = GL_INCR_WRAP_OES;
		}
#endif
		glEnable(GL_CULL_FACE);
		if (!zfail)
		{
			// ZPASS Method

			glCullFace(GL_BACK);
			glStencilOp(GL_KEEP, GL_KEEP, incr);
			glDrawArrays(GL_TRIANGLES, 0, count);

			glCullFace(GL_FRONT);
			glStencilOp(GL_KEEP, GL_KEEP, decr);
			glDrawArrays(GL_TRIANGLES, 0, count);
		}
		else
		{
			// ZFAIL Method

			glStencilOp(GL_KEEP, incr, GL_KEEP);
			glCullFace(GL_FRONT);
			glDrawArrays(GL_TRIANGLES, 0, count);

			glStencilOp(GL_KEEP, decr, GL_KEEP);
			glCullFace(GL_BACK);
			glDrawArrays(GL_TRIANGLES, 0, count);
		}

		glDisableVertexAttribArray(EVA_POSITION);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDisable(GL_STENCIL_TEST);
		if (cullFaceEnabled)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		glCullFace(cullFaceMode);
		glDepthFunc(depthFunc);
		glDepthMask(depthMask);
		testGLError();
	}


	void COGLES2Driver::drawStencilShadow(bool clearStencilBuffer,
			video::SColor leftUpEdge, video::SColor rightUpEdge,
			video::SColor leftDownEdge, video::SColor rightDownEdge)
	{
		if (!StencilBuffer)
			return;

		disableTextures();

		// store attributes
		GLboolean depthMask;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
//			GLint shadeModel;
		//TODO : OpenGL ES 2.0 Port glGetIntegerv
		//glGetIntegerv(GL_SHADE_MODEL, &shadeModel);

		glDepthMask(GL_FALSE);

		//TODO : OpenGL ES 2.0 Port glShadeModel
		//glShadeModel(GL_FLAT);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		if (! BlendEnabled)
			glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 0, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		// draw a shadow rectangle covering the entire screen using stencil buffer
		//Wrapper->glMatrixMode(GL_MODELVIEW);
		//TODO : OpenGL ES 2.0 Port glPushMatrix
		//glPushMatrix();
		//Wrapper->glLoadIdentity();
		//Wrapper->glMatrixMode(GL_PROJECTION);
		//TODO : OpenGL ES 2.0 Port glPushMatrix
		//glPushMatrix();
		//Wrapper->glLoadIdentity();

		u16 indices[] = {0, 1, 2, 3};
		S3DVertex vertices[4];
		vertices[0] = S3DVertex(-1.f, -1.f, 0.9f, 0, 0, 1, leftDownEdge, 0, 0);
		vertices[1] = S3DVertex(-1.f, 1.f, 0.9f, 0, 0, 1, leftUpEdge, 0, 0);
		vertices[2] = S3DVertex(1.f, 1.f, 0.9f, 0, 0, 1, rightUpEdge, 0, 0);
		vertices[3] = S3DVertex(1.f, -1.f, 0.9f, 0, 0, 1, rightDownEdge, 0, 0);
		drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);

		if (clearStencilBuffer)
			glClear(GL_STENCIL_BUFFER_BIT);

		// restore settings
		//TODO : OpenGL ES 2.0 Port glPopMatrix
		//glPopMatrix();
		//Wrapper->glMatrixMode(GL_MODELVIEW);
		//TODO : OpenGL ES 2.0 Port glPopMatrix
		//glPopMatrix();
		glDisable(GL_STENCIL_TEST);

		glDepthMask(depthMask);
		//TODO : OpenGL ES 2.0 Port glShadeModel
		//glShadeModel(shadeModel);
		if (!BlendEnabled)
			glDisable(GL_BLEND);
		glBlendFunc(getGLBlend(SourceFactor), getGLBlend(DestFactor));
		testGLError();
	}


	//! Draws a 3d line.
	void COGLES2Driver::draw3DLine(const core::vector3df& start,
			const core::vector3df& end, SColor color)
	{
		setRenderStates3DMode();

		u16 indices[] = {0, 1};
		S3DVertex vertices[2];
		vertices[0] = S3DVertex(start.X, start.Y, start.Z, 0, 0, 1, color, 0, 0);
		vertices[1] = S3DVertex(end.X, end.Y, end.Z, 0, 0, 1, color, 0, 0);
		drawVertexPrimitiveList2d3d(vertices, 2, indices, 1, video::EVT_STANDARD, scene::EPT_LINES);
	}


	//! Only used by the internal engine. Used to notify the driver that
	//! the window was resized.
	void COGLES2Driver::OnResize(const core::dimension2d<u32>& size)
	{
		CNullDriver::OnResize(size);
		glViewport(0, 0, size.Width, size.Height);
		testGLError();
	}


	//! Returns type of video driver
	E_DRIVER_TYPE COGLES2Driver::getDriverType() const
	{
		return EDT_OGLES2;
	}


	//! returns color format
	ECOLOR_FORMAT COGLES2Driver::getColorFormat() const
	{
		return ColorFormat;
	}


	//! Sets a vertex shader constant.
	void COGLES2Driver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
	{
#ifdef GL_vertex_program
		for (s32 i = 0; i < constantAmount; ++i)
			glProgramLocalParameter4fv(GL_VERTEX_PROGRAM, startRegister + i, &data[i*4]);
#endif
	}

	//! Sets a pixel shader constant.
	void COGLES2Driver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
	{
#ifdef GL_fragment_program
		for (s32 i = 0; i < constantAmount; ++i)
			glProgramLocalParameter4fv(GL_FRAGMENT_PROGRAM, startRegister + i, &data[i*4]);
#endif
	}

	//! Sets a constant for the vertex shader based on a name.
	bool COGLES2Driver::setVertexShaderConstant(const c8* name, const f32* floats, int count)
	{
		//pass this along, as in GLSL the same routine is used for both vertex and fragment shaders
		return setPixelShaderConstant(name, floats, count);
	}

	//! Sets a constant for the pixel shader based on a name.
	bool COGLES2Driver::setPixelShaderConstant(const c8* name, const f32* floats, int count)
	{
		os::Printer::log("Error: Please call services->setPixelShaderConstant(), not VideoDriver->setPixelShaderConstant().");
		return false;
	}

	//! Sets a vertex pointer the vertex shader based on a name.
	bool COGLES2Driver::setVertexShaderPointer(const c8*, const void*, s32, bool, u16)
	{
		os::Printer::log("Error: Please call services->setVertexPointer(), not VideoDriver->setVertexPointer().");
		return false;
	}


	//! Adds a new material renderer to the VideoDriver, using pixel and/or
	//! vertex shaders to render geometry.
	s32 COGLES2Driver::addShaderMaterial(const c8* vertexShaderProgram,
			const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback,
			E_MATERIAL_TYPE baseMaterial, s32 userData)
	{
		os::Printer::log("No shader support.");
		return -1;
	}


	//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
	s32 COGLES2Driver::addHighLevelShaderMaterial(
			const c8* vertexShaderProgram,
			const c8* vertexShaderEntryPointName,
			E_VERTEX_SHADER_TYPE vsCompileTarget,
			const c8* pixelShaderProgram,
			const c8* pixelShaderEntryPointName,
			E_PIXEL_SHADER_TYPE psCompileTarget,
			const c8* geometryShaderProgram,
			const c8* geometryShaderEntryPointName,
			E_GEOMETRY_SHADER_TYPE gsCompileTarget,
			scene::E_PRIMITIVE_TYPE inType,
			scene::E_PRIMITIVE_TYPE outType,
			u32 verticesOut,
			IShaderConstantSetCallBack* callback,
			E_MATERIAL_TYPE baseMaterial,
			s32 userData)
	{
		s32 nr = -1;
		COGLES2SLMaterialRenderer* r = new COGLES2SLMaterialRenderer(
			this, nr, vertexShaderProgram,
			pixelShaderProgram,
			callback, getMaterialRenderer(baseMaterial), userData);

		r->drop();
		return nr;
	}

	//! Returns a pointer to the IVideoDriver interface. (Implementation for
	//! IMaterialRendererServices)
	IVideoDriver* COGLES2Driver::getVideoDriver()
	{
		return this;
	}


	//! Returns pointer to the IGPUProgrammingServices interface.
	IGPUProgrammingServices* COGLES2Driver::getGPUProgrammingServices()
	{
		return this;
	}


	ITexture* COGLES2Driver::addRenderTargetTexture(
			const core::dimension2d<u32>& size,
			const io::path& name, const ECOLOR_FORMAT format)
	{
		//disable mip-mapping
		const bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
		setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

		video::ITexture* rtt = 0;

		// if driver supports FrameBufferObjects, use them
		if (queryFeature(EVDF_FRAMEBUFFER_OBJECT))
		{
			rtt = new COGLES2FBOTexture(size, name, this, format);
			if (rtt)
			{
				bool success = false;
				addTexture(rtt);

				ITexture* tex = createDepthTexture(rtt);
				if (tex)
				{
					success = static_cast<video::COGLES2FBODepthTexture*>(tex)->attach(rtt);
					if (!success)
					{
						removeDepthTexture(tex);
					}
					tex->drop();
				}
				rtt->drop();
				if (!success)
				{
					removeTexture(rtt);
					rtt=0;
				}
			}
		}
		else
		{
			// the simple texture is only possible for size <= screensize
			// we try to find an optimal size with the original constraints
			core::dimension2du destSize(core::min_(size.Width, ScreenSize.Width), core::min_(size.Height, ScreenSize.Height));
			destSize = destSize.getOptimalSize((size == size.getOptimalSize()), false, false);
			rtt = addTexture(destSize, name, ECF_A8R8G8B8);
			if (rtt)
				static_cast<video::COGLES2Texture*>(rtt)->setIsRenderTarget(true);
		}

		//restore mip-mapping
		setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, generateMipLevels);

		return rtt;
	}


	//! Returns the maximum amount of primitives
	u32 COGLES2Driver::getMaximalPrimitiveCount() const
	{
		return 65535;
	}


	//! set or reset render target
	bool COGLES2Driver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
			bool clearZBuffer, SColor color)
	{
		// check for right driver type

		if (texture && texture->getDriverType() != EDT_OGLES2)
		{
			os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
			return false;
		}

		// check if we should set the previous RT back

		setActiveTexture(0, 0);
		ResetRenderStates = true;
		if (RenderTargetTexture != 0)
		{
			RenderTargetTexture->unbindRTT();
		}

		if (texture)
		{
			// we want to set a new target. so do this.
			RenderTargetTexture = static_cast<COGLES2Texture*>(texture);
			RenderTargetTexture->bindRTT();
			CurrentRendertargetSize = texture->getSize();
		}
		else
		{
			glViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
			RenderTargetTexture = 0;
			CurrentRendertargetSize = core::dimension2d<u32>(0, 0);
		}

		GLbitfield mask = 0;
		if (clearBackBuffer)
		{
			const f32 inv = 1.0f / 255.0f;
			glClearColor(color.getRed() * inv, color.getGreen() * inv,
						color.getBlue() * inv, color.getAlpha() * inv);

			mask |= GL_COLOR_BUFFER_BIT;
		}
		if (clearZBuffer)
		{
			glDepthMask(GL_TRUE);
			LastMaterial.ZWriteEnable = true;
			mask |= GL_DEPTH_BUFFER_BIT;
		}

		glClear(mask);
		testGLError();

		return true;
	}


	// returns the current size of the screen or rendertarget
	const core::dimension2d<u32>& COGLES2Driver::getCurrentRenderTargetSize() const
	{
		if (CurrentRendertargetSize.Width == 0)
			return ScreenSize;
		else
			return CurrentRendertargetSize;
	}


	//! Clears the ZBuffer.
	void COGLES2Driver::clearZBuffer()
	{
		GLboolean enabled = GL_TRUE;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &enabled);

		glDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);

		glDepthMask(enabled);
		testGLError();
	}


	//! Returns an image created from the last rendered frame.
	// We want to read the front buffer to get the latest render finished.
	// This is not possible under ogl-es, though, so one has to call this method
	// outside of the render loop only.
	IImage* COGLES2Driver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
	{
		if (target==video::ERT_MULTI_RENDER_TEXTURES || target==video::ERT_RENDER_TEXTURE || target==video::ERT_STEREO_BOTH_BUFFERS)
			return 0;

		GLint internalformat = GL_RGBA;
		GLint type = GL_UNSIGNED_BYTE;
		{
//			glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &internalformat);
//			glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
			// there's a format we don't support ATM
			if (GL_UNSIGNED_SHORT_4_4_4_4 == type)
			{
				internalformat = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
			}
		}

		IImage* newImage = 0;
		if (GL_RGBA == internalformat)
		{
			if (GL_UNSIGNED_BYTE == type)
				newImage = new CImage(ECF_A8R8G8B8, ScreenSize);
			else
				newImage = new CImage(ECF_A1R5G5B5, ScreenSize);
		}
		else
		{
			if (GL_UNSIGNED_BYTE == type)
				newImage = new CImage(ECF_R8G8B8, ScreenSize);
			else
				newImage = new CImage(ECF_R5G6B5, ScreenSize);
		}

		if (!newImage)
			return 0;

		u8* pixels = static_cast<u8*>(newImage->lock());
		if (!pixels)
		{
			newImage->unlock();
			newImage->drop();
			return 0;
		}

		glReadPixels(0, 0, ScreenSize.Width, ScreenSize.Height, internalformat, type, pixels);
		testGLError();

		// opengl images are horizontally flipped, so we have to fix that here.
		const s32 pitch = newImage->getPitch();
		u8* p2 = pixels + (ScreenSize.Height - 1) * pitch;
		u8* tmpBuffer = new u8[pitch];
		for (u32 i = 0; i < ScreenSize.Height; i += 2)
		{
			memcpy(tmpBuffer, pixels, pitch);
			memcpy(pixels, p2, pitch);
			memcpy(p2, tmpBuffer, pitch);
			pixels += pitch;
			p2 -= pitch;
		}
		delete [] tmpBuffer;

		newImage->unlock();

		if (testGLError())
		{
			newImage->drop();
			return 0;
		}
		testGLError();
		return newImage;
	}


	//! get depth texture for the given render target texture
	ITexture* COGLES2Driver::createDepthTexture(ITexture* texture, bool shared)
	{
		if ((texture->getDriverType() != EDT_OGLES2) || (!texture->isRenderTarget()))
			return 0;
		COGLES2Texture* tex = static_cast<COGLES2Texture*>(texture);

		if (!tex->isFrameBufferObject())
			return 0;

		if (shared)
		{
			for (u32 i = 0; i < DepthTextures.size(); ++i)
			{
				if (DepthTextures[i]->getSize() == texture->getSize())
				{
					DepthTextures[i]->grab();
					return DepthTextures[i];
				}
			}
			DepthTextures.push_back(new COGLES2FBODepthTexture(texture->getSize(), "depth1", this));
			return DepthTextures.getLast();
		}
		return (new COGLES2FBODepthTexture(texture->getSize(), "depth1", this));
	}


	void COGLES2Driver::removeDepthTexture(ITexture* texture)
	{
		for (u32 i = 0; i < DepthTextures.size(); ++i)
		{
			if (texture == DepthTextures[i])
			{
				DepthTextures.erase(i);
				return;
			}
		}
	}

	void COGLES2Driver::reloadShaders()
	{
		FixedPipeline->reload();
	}

	void COGLES2Driver::deleteFramebuffers(s32 n, const u32 *framebuffers)
	{
		glDeleteFramebuffers(n, framebuffers);
	}

	void COGLES2Driver::deleteRenderbuffers(s32 n, const u32 *renderbuffers)
	{
		glDeleteRenderbuffers(n, renderbuffers);
	}

	void COGLES2Driver::enableBlend()
	{
		if (! BlendEnabled)
		{
			BlendEnabled = true;
			glEnable(GL_BLEND);
		}
	}

	void COGLES2Driver::disableBlend()
	{
		if (BlendEnabled)
		{
			BlendEnabled = false;
			glDisable(GL_BLEND);
		}
	}

	u32 COGLES2Driver::getGLBlend(E_BLEND_FACTOR factor)
	{
		u32 r = 0;
		switch (factor)
		{
			case EBF_ZERO:
				r = GL_ZERO;
				break;
			case EBF_ONE:
				r = GL_ONE;
				break;
			case EBF_DST_COLOR:
				r = GL_DST_COLOR;
				break;
			case EBF_ONE_MINUS_DST_COLOR:
				r = GL_ONE_MINUS_DST_COLOR;
				break;
			case EBF_SRC_COLOR:
				r = GL_SRC_COLOR;
				break;
			case EBF_ONE_MINUS_SRC_COLOR:
				r = GL_ONE_MINUS_SRC_COLOR;
				break;
			case EBF_SRC_ALPHA:
				r = GL_SRC_ALPHA;
				break;
			case EBF_ONE_MINUS_SRC_ALPHA:
				r = GL_ONE_MINUS_SRC_ALPHA;
				break;
			case EBF_DST_ALPHA:
				r = GL_DST_ALPHA;
				break;
			case EBF_ONE_MINUS_DST_ALPHA:
				r = GL_ONE_MINUS_DST_ALPHA;
				break;
			case EBF_SRC_ALPHA_SATURATE:
				r = GL_SRC_ALPHA_SATURATE;
				break;
		}
		return r;
	}

	void COGLES2Driver::blendFunc(E_BLEND_FACTOR sFactor, E_BLEND_FACTOR dFactor)
	{
		if (sFactor != SourceFactor || dFactor != DestFactor)
		{
			SourceFactor = sFactor;
			DestFactor = dFactor;
			glBlendFunc(getGLBlend(sFactor), getGLBlend(dFactor));
		}
	}


	//! Set/unset a clipping plane.
	bool COGLES2Driver::setClipPlane(u32 index, const core::plane3df& plane, bool enable)
	{
		if (index >= UserClipPlane.size())
			UserClipPlane.push_back(SUserClipPlane());

		UserClipPlane[index].Plane = plane;
		UserClipPlane[index].Enabled = enable;
		return true;
	}

	//! Enable/disable a clipping plane.
	void COGLES2Driver::enableClipPlane(u32 index, bool enable)
	{
		if (index >= MaxUserClipPlanes)
			return;

		UserClipPlane[index].Enabled = enable;
	}

	//! Get the ClipPlane Count
	u32 COGLES2Driver::getClipPlaneCount() const
	{
		return UserClipPlane.size();
	}

	const core::plane3df& COGLES2Driver::getClipPlane(irr::u32 index) const
	{
		if (index < UserClipPlane.size())
			return UserClipPlane[index].Plane;
		else
			return *((core::plane3df*)0);
	}

	core::dimension2du COGLES2Driver::getMaxTextureSize() const
	{
		return core::dimension2du(MaxTextureSize, MaxTextureSize);
	}


} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_OGLES2_

namespace irr
{
namespace video
{

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_COMPILE_WITH_SDL_DEVICE_) || defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_) || defined(_IRR_COMPILE_WITH_CONSOLE_DEVICE_)
	IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params,
			video::SExposedVideoData& data, io::IFileSystem* io)
	{
#ifdef _IRR_COMPILE_WITH_OGLES2_
		return new COGLES2Driver(params, data, io);
#else
		return 0;
#endif // _IRR_COMPILE_WITH_OGLES2_
	}
#endif

// -----------------------------------
// MACOSX VERSION
// -----------------------------------
#if defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
	IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params,
			io::IFileSystem* io, CIrrDeviceMacOSX *device)
	{
#ifdef _IRR_COMPILE_WITH_OGLES2_
		return new COGLES2Driver(params, io, device);
#else
		return 0;
#endif // _IRR_COMPILE_WITH_OGLES2_
	}
#endif // _IRR_COMPILE_WITH_OSX_DEVICE_

// -----------------------------------
// IPHONE VERSION
// -----------------------------------
#if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
	IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params,
			video::SExposedVideoData& data, io::IFileSystem* io,
			MIrrIPhoneDevice const & device)
	{
#ifdef _IRR_COMPILE_WITH_OGLES2_
		return new COGLES2Driver(params, data, io, device);
#else
		return 0;
#endif // _IRR_COMPILE_WITH_OGLES2_
	}
#endif // _IRR_COMPILE_WITH_IPHONE_DEVICE_

} // end namespace
} // end namespace
