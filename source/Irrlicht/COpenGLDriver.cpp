// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COpenGLDriver.h"
// needed here also because of the create methods' parameters
#include "CNullDriver.h"
#include "IContextManager.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLTexture.h"
#include "COpenGLMaterialRenderer.h"
#include "COpenGLShaderMaterialRenderer.h"
#include "COpenGLSLMaterialRenderer.h"
#include "COpenGLCgMaterialRenderer.h"
#include "COpenGLNormalMapRenderer.h"
#include "COpenGLParallaxMapRenderer.h"
#include "os.h"
#include "IrrlichtDevice.h"

#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_
#include "MacOSX/CIrrDeviceMacOSX.h"
#endif

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
#include <SDL/SDL.h>
#endif

#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_
// If You use MacOSX SDK which support MacOSX 10.6 "__MAC_10_6" and
// custom XCode project for build Irrlicht make sure that
// COpenGLDriver.cpp is signed as "Objective-C++ Source"
// in "Identity and Type" section in XCode.
#include "MacOSX/CIrrDeviceMacOSX.h"
#endif

namespace irr
{
namespace video
{

// Statics variables
const u16 COpenGLDriver::Quad2DIndices[4] = { 0, 1, 2, 3 };

// -----------------------------------------------------------------------
// WINDOWS CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#include "CWGLManager.h"

//! Windows constructor and init code
COpenGLDriver::COpenGLDriver(const ::irr::SIrrlichtCreationParameters& params,
		io::IFileSystem* io, IContextManager* contextManager)
: CNullDriver(io, params.WindowSize), COpenGLExtensionHandler(),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), Transformation3DChanged(true),
	AntiAlias(params.AntiAlias), RenderTargetTexture(0),
	CurrentRendertargetSize(0,0), ColorFormat(ECF_R8G8B8),
	FixedPipelineState(EOFPS_ENABLE),
	CurrentTarget(ERT_FRAME_BUFFER), Params(params), BridgeCalls(0),
	ContextManager(contextManager), DeviceType(EIDT_WIN32)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	#ifdef _IRR_COMPILE_WITH_CG_
	CgContext = 0;
	#endif
}


//! inits the open gl driver
bool COpenGLDriver::initDriver(CIrrDeviceWin32* device)
{
	ContextManager->generateSurface();
	ContextManager->generateContext();
	ExposedData=ContextManager->getContext();
	ContextManager->activateContext(ExposedData);

	genericDriverInit();

	extGlSwapInterval(Params.Vsync ? 1 : 0);
	return true;
}

#endif // _IRR_COMPILE_WITH_WINDOWS_DEVICE_

// -----------------------------------------------------------------------
// MacOSX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_
COpenGLDriver::COpenGLDriver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, IContextManager* contextManager)
: CNullDriver(io, params.WindowSize), COpenGLExtensionHandler(),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), Transformation3DChanged(true),
	AntiAlias(params.AntiAlias), RenderTargetTexture(0),
	CurrentRendertargetSize(0,0), ColorFormat(ECF_R8G8B8),
	FixedPipelineState(EOFPS_ENABLE),
	CurrentTarget(ERT_FRAME_BUFFER), Params(params), BridgeCalls(0),
	ContextManager(contextManager), DeviceType(EIDT_OSX)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	#ifdef _IRR_COMPILE_WITH_CG_
	CgContext = 0;
	#endif
}

    //! inits the open gl driver
bool COpenGLDriver::initDriver()
{
    ContextManager->generateSurface();
    ContextManager->generateContext();
    ExposedData=ContextManager->getContext();
    ContextManager->activateContext(ExposedData);

    genericDriverInit();

    return true;
}

#endif

// -----------------------------------------------------------------------
// LINUX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
//! Linux constructor and init code
COpenGLDriver::COpenGLDriver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, IContextManager* contextManager)
: CNullDriver(io, params.WindowSize), COpenGLExtensionHandler(),
	BridgeCalls(0), CurrentRenderMode(ERM_NONE), ResetRenderStates(true),
	Transformation3DChanged(true), AntiAlias(params.AntiAlias),
	RenderTargetTexture(0), CurrentRendertargetSize(0,0),
	ColorFormat(ECF_R8G8B8), FixedPipelineState(EOFPS_ENABLE),
	CurrentTarget(ERT_FRAME_BUFFER), Params(params),
	ContextManager(contextManager), DeviceType(EIDT_X11)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	#ifdef _IRR_COMPILE_WITH_CG_
	CgContext = 0;
	#endif
}


//! inits the open gl driver
bool COpenGLDriver::initDriver(CIrrDeviceLinux* device)
{
	ContextManager->generateSurface();
	ContextManager->generateContext();
	ExposedData=ContextManager->getContext();
	ContextManager->activateContext(ExposedData);

	genericDriverInit();

	extGlSwapInterval(Params.Vsync ? 1 : 0);
	return true;
}

#endif // _IRR_COMPILE_WITH_X11_DEVICE_


// -----------------------------------------------------------------------
// SDL CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
//! SDL constructor and init code
COpenGLDriver::COpenGLDriver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, CIrrDeviceSDL* device)
: CNullDriver(io, params.WindowSize), COpenGLExtensionHandler(),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true),
	Transformation3DChanged(true), AntiAlias(params.AntiAlias),
	RenderTargetTexture(0), CurrentRendertargetSize(0,0),
	ColorFormat(ECF_R8G8B8), FixedPipelineState(EOFPS_ENABLE),
	CurrentTarget(ERT_FRAME_BUFFER), Params(params),
	BridgeCalls(0), SDLDevice(device), DeviceType(EIDT_SDL)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	#ifdef _IRR_COMPILE_WITH_CG_
	CgContext = 0;
	#endif

	genericDriverInit();
}

#endif // _IRR_COMPILE_WITH_SDL_DEVICE_


//! destructor
COpenGLDriver::~COpenGLDriver()
{
	#ifdef _IRR_COMPILE_WITH_CG_
	if (CgContext)
		cgDestroyContext(CgContext);
	#endif

	if (BridgeCalls)
		delete BridgeCalls;

	RequestedLights.clear();

	deleteMaterialRenders();

	CurrentTexture.clear();
	// I get a blue screen on my laptop, when I do not delete the
	// textures manually before releasing the dc. Oh how I love this.
	deleteAllTextures();
	removeAllOcclusionQueries();
	removeAllHardwareBuffers();

	if (ContextManager)
	{
		ContextManager->destroyContext();
		ContextManager->destroySurface();
		ContextManager->terminate();
		ContextManager->drop();
	}
}

// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

bool COpenGLDriver::genericDriverInit()
{
	if (ContextManager)
		ContextManager->grab();
	Name=L"OpenGL ";
	Name.append(glGetString(GL_VERSION));
	s32 pos=Name.findNext(L' ', 7);
	if (pos != -1)
		Name=Name.subString(0, pos);
	printVersion();

	// print renderer information
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	if (renderer && vendor)
	{
		os::Printer::log(reinterpret_cast<const c8*>(renderer), reinterpret_cast<const c8*>(vendor), ELL_INFORMATION);
		VendorName = reinterpret_cast<const c8*>(vendor);
	}

	u32 i;
	CurrentTexture.clear();
	// load extensions
	initExtensions(Params.Stencilbuffer);

	if (!BridgeCalls)
		BridgeCalls = new COpenGLCallBridge(this);

	if (queryFeature(EVDF_ARB_GLSL))
	{
		char buf[32];
		const u32 maj = ShaderLanguageVersion/100;
		snprintf(buf, 32, "%u.%u", maj, ShaderLanguageVersion-maj*100);
		os::Printer::log("GLSL version", buf, ELL_INFORMATION);
	}
	else
		os::Printer::log("GLSL not available.", ELL_INFORMATION);
	DriverAttributes->setAttribute("MaxTextures", MaxTextureUnits);
	DriverAttributes->setAttribute("MaxSupportedTextures", MaxSupportedTextures);
	DriverAttributes->setAttribute("MaxLights", MaxLights);
	DriverAttributes->setAttribute("MaxAnisotropy", MaxAnisotropy);
	DriverAttributes->setAttribute("MaxUserClipPlanes", MaxUserClipPlanes);
	DriverAttributes->setAttribute("MaxAuxBuffers", MaxAuxBuffers);
	DriverAttributes->setAttribute("MaxMultipleRenderTargets", MaxMultipleRenderTargets);
	DriverAttributes->setAttribute("MaxIndices", (s32)MaxIndices);
	DriverAttributes->setAttribute("MaxTextureSize", (s32)MaxTextureSize);
	DriverAttributes->setAttribute("MaxGeometryVerticesOut", (s32)MaxGeometryVerticesOut);
	DriverAttributes->setAttribute("MaxTextureLODBias", MaxTextureLODBias);
	DriverAttributes->setAttribute("Version", Version);
	DriverAttributes->setAttribute("ShaderLanguageVersion", ShaderLanguageVersion);
	DriverAttributes->setAttribute("AntiAlias", AntiAlias);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// Reset The Current Viewport
	glViewport(0, 0, Params.WindowSize.Width, Params.WindowSize.Height);

	UserClipPlanes.reallocate(MaxUserClipPlanes);
	for (i=0; i<MaxUserClipPlanes; ++i)
		UserClipPlanes.push_back(SUserClipPlane());

	for (i=0; i<ETS_COUNT; ++i)
		setTransform(static_cast<E_TRANSFORMATION_STATE>(i), core::IdentityMatrix);

	setAmbientLight(SColorf(0.0f,0.0f,0.0f,0.0f));
#ifdef GL_EXT_separate_specular_color
	if (FeatureAvailable[IRR_EXT_separate_specular_color])
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	Params.HandleSRGB &= ((FeatureAvailable[IRR_ARB_framebuffer_sRGB] || FeatureAvailable[IRR_EXT_framebuffer_sRGB]) &&
		FeatureAvailable[IRR_EXT_texture_sRGB]);
#if defined(GL_ARB_framebuffer_sRGB)
	if (Params.HandleSRGB)
		glEnable(GL_FRAMEBUFFER_SRGB);
#elif defined(GL_EXT_framebuffer_sRGB)
	if (Params.HandleSRGB)
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);
#endif

// This is a fast replacement for NORMALIZE_NORMALS
//	if ((Version>101) || FeatureAvailable[IRR_EXT_rescale_normal])
//		glEnable(GL_RESCALE_NORMAL_EXT);

	glClearDepth(1.0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	glFrontFace(GL_CW);
	// adjust flat coloring scheme to DirectX version
#if defined(GL_ARB_provoking_vertex) || defined(GL_EXT_provoking_vertex)
	extGlProvokingVertex(GL_FIRST_VERTEX_CONVENTION_EXT);
#endif

	// Create built-in 2D quad for 2D rendering (both quads and lines).
	Quad2DVertices[0] = S3DVertex(core::vector3df(-1.0f, 1.0f, 0.0f), core::vector3df(0.0f, 0.0f, 0.0f), SColor(255,255,255,255), core::vector2df(0.0f, 1.0f));
	Quad2DVertices[1] = S3DVertex(core::vector3df(1.0f, 1.0f, 0.0f), core::vector3df(0.0f, 0.0f, 0.0f), SColor(255,255,255,255), core::vector2df(1.0f, 1.0f));
	Quad2DVertices[2] = S3DVertex(core::vector3df(1.0f, -1.0f, 0.0f), core::vector3df(0.0f, 0.0f, 0.0f), SColor(255,255,255,255), core::vector2df(1.0f, 0.0f));
	Quad2DVertices[3] = S3DVertex(core::vector3df(-1.0f, -1.0f, 0.0f), core::vector3df(0.0f, 0.0f, 0.0f), SColor(255,255,255,255), core::vector2df(0.0f, 0.0f));

	// create material renderers
	createMaterialRenderers();

	// set the renderstates
	setRenderStates3DMode();

	// set fog mode
	setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// create matrix for flipping textures
	TextureFlipMatrix.buildTextureTransform(0.0f, core::vector2df(0,0), core::vector2df(0,1.0f), core::vector2df(1.0f,-1.0f));

	// We need to reset once more at the beginning of the first rendering.
	// This fixes problems with intermediate changes to the material during texture load.
	ResetRenderStates = true;

	#ifdef _IRR_COMPILE_WITH_CG_
	CgContext = cgCreateContext();
	#endif

	return true;
}


void COpenGLDriver::createMaterialRenderers()
{
	// create OpenGL material renderers

	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID_2_LAYER(this));

	// add the same renderer for all lightmap types
	COpenGLMaterialRenderer_LIGHTMAP* lmr = new COpenGLMaterialRenderer_LIGHTMAP(this);
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
	lmr->drop();

	// add remaining material renderer
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_DETAIL_MAP(this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SPHERE_MAP(this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_REFLECTION_2_LAYER(this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR(this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(this));

	// add normal map renderers
	s32 tmp = 0;
	video::IMaterialRenderer* renderer = 0;
	renderer = new COpenGLNormalMapRenderer(this, tmp, EMT_SOLID);
	renderer->drop();
	renderer = new COpenGLNormalMapRenderer(this, tmp, EMT_TRANSPARENT_ADD_COLOR);
	renderer->drop();
	renderer = new COpenGLNormalMapRenderer(this, tmp, EMT_TRANSPARENT_VERTEX_ALPHA);
	renderer->drop();

	// add parallax map renderers
	renderer = new COpenGLParallaxMapRenderer(this, tmp, EMT_SOLID);
	renderer->drop();
	renderer = new COpenGLParallaxMapRenderer(this, tmp, EMT_TRANSPARENT_ADD_COLOR);
	renderer->drop();
	renderer = new COpenGLParallaxMapRenderer(this, tmp, EMT_TRANSPARENT_VERTEX_ALPHA);
	renderer->drop();

	// add basic 1 texture blending
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_ONETEXTURE_BLEND(this));
}


//! presents the rendered scene on the screen, returns false if failed
bool COpenGLDriver::endScene()
{
	CNullDriver::endScene();

	glFlush();

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
	if (DeviceType == EIDT_WIN32)
	{
		return ContextManager->swapBuffers();
	}
#endif

#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
	if (DeviceType == EIDT_X11)
	{
		return ContextManager->swapBuffers();
	}
#endif

#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_
	if (DeviceType == EIDT_OSX)
	{
		return ContextManager->swapBuffers();
	}
#endif

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
	if (DeviceType == EIDT_SDL)
	{
		SDL_GL_SwapBuffers();
		return true;
	}
#endif

	// todo: console device present

	return false;
}


//! clears the zbuffer and color buffer
void COpenGLDriver::clearBuffers(bool backBuffer, bool zBuffer, bool stencilBuffer, SColor color)
{
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
		BridgeCalls->setDepthMask(true);

		LastMaterial.ZWriteEnable=true;
 		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (stencilBuffer)
		mask |= GL_STENCIL_BUFFER_BIT;

	if (mask)
		glClear(mask);
}


//! init call for rendering start
bool COpenGLDriver::beginScene(bool backBuffer, bool zBuffer, SColor color,
		const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
{
	CNullDriver::beginScene(backBuffer, zBuffer, color, videoData, sourceRect);
	ContextManager->activateContext(videoData);

#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
	if (DeviceType == EIDT_SDL)
	{
		// todo: SDL sets glFrontFace(GL_CCW) after driver creation,
		// it would be better if this was fixed elsewhere.
		glFrontFace(GL_CW);
	}
#endif

	clearBuffers(backBuffer, zBuffer, false, color);
	return true;
}


//! Returns the transformation set by setTransform
const core::matrix4& COpenGLDriver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return Matrices[state];
}


//! sets transformation
void COpenGLDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	Matrices[state] = mat;
	Transformation3DChanged = true;

	switch (state)
	{
	case ETS_VIEW:
	case ETS_WORLD:
		{
			// OpenGL only has a model matrix, view and world is not existent. so lets fake these two.
			BridgeCalls->setMatrixMode(GL_MODELVIEW);

			// first load the viewing transformation for user clip planes
			glLoadMatrixf((Matrices[ETS_VIEW]).pointer());

			// we have to update the clip planes to the latest view matrix
			for (u32 i=0; i<MaxUserClipPlanes; ++i)
				if (UserClipPlanes[i].Enabled)
					uploadClipPlane(i);

			// now the real model-view matrix
			glMultMatrixf(Matrices[ETS_WORLD].pointer());
		}
		break;
	case ETS_PROJECTION:
		{
			BridgeCalls->setMatrixMode(GL_PROJECTION);
			glLoadMatrixf(mat.pointer());
		}
		break;
	case ETS_COUNT:
		return;
	default:
		{
			const u32 i = state - ETS_TEXTURE_0;
			if (i >= MATERIAL_MAX_TEXTURES)
				break;

			const bool isRTT = Material.getTexture(i) && Material.getTexture(i)->isRenderTarget();

			BridgeCalls->setActiveTexture(GL_TEXTURE0_ARB + i);

			BridgeCalls->setMatrixMode(GL_TEXTURE);
			if (!isRTT && mat.isIdentity() )
				glLoadIdentity();
			else
			{
				GLfloat glmat[16];
				if (isRTT)
					getGLTextureMatrix(glmat, mat * TextureFlipMatrix);
				else
					getGLTextureMatrix(glmat, mat);
				glLoadMatrixf(glmat);
			}
			break;
		}
	}
}


bool COpenGLDriver::updateVertexHardwareBuffer(SHWBufferLink_opengl *HWBuffer)
{
	if (!HWBuffer)
		return false;

	if (!FeatureAvailable[IRR_ARB_vertex_buffer_object])
		return false;

#if defined(GL_ARB_vertex_buffer_object)
	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
	const void* vertices=mb->getVertices();
	const u32 vertexCount=mb->getVertexCount();
	const E_VERTEX_TYPE vType=mb->getVertexType();
	const u32 vertexSize = getVertexPitchFromType(vType);

	const c8* vbuf = static_cast<const c8*>(vertices);
	core::array<c8> buffer;
	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
	{
		//buffer vertex data, and convert colors...
		buffer.set_used(vertexSize * vertexCount);
		memcpy(buffer.pointer(), vertices, vertexSize * vertexCount);
		vbuf = buffer.const_pointer();

		// in order to convert the colors into opengl format (RGBA)
		switch (vType)
		{
			case EVT_STANDARD:
			{
				S3DVertex* pb = reinterpret_cast<S3DVertex*>(buffer.pointer());
				const S3DVertex* po = static_cast<const S3DVertex*>(vertices);
				for (u32 i=0; i<vertexCount; i++)
				{
					po[i].Color.toOpenGLColor((u8*)&(pb[i].Color));
				}
			}
			break;
			case EVT_2TCOORDS:
			{
				S3DVertex2TCoords* pb = reinterpret_cast<S3DVertex2TCoords*>(buffer.pointer());
				const S3DVertex2TCoords* po = static_cast<const S3DVertex2TCoords*>(vertices);
				for (u32 i=0; i<vertexCount; i++)
				{
					po[i].Color.toOpenGLColor((u8*)&(pb[i].Color));
				}
			}
			break;
			case EVT_TANGENTS:
			{
				S3DVertexTangents* pb = reinterpret_cast<S3DVertexTangents*>(buffer.pointer());
				const S3DVertexTangents* po = static_cast<const S3DVertexTangents*>(vertices);
				for (u32 i=0; i<vertexCount; i++)
				{
					po[i].Color.toOpenGLColor((u8*)&(pb[i].Color));
				}
			}
			break;
			default:
			{
				return false;
			}
		}
	}

	//get or create buffer
	bool newBuffer=false;
	if (!HWBuffer->vbo_verticesID)
	{
		extGlGenBuffers(1, &HWBuffer->vbo_verticesID);
		if (!HWBuffer->vbo_verticesID)
			return false;
		newBuffer=true;
	}
	else if (HWBuffer->vbo_verticesSize < vertexCount*vertexSize)
	{
		newBuffer=true;
	}

	extGlBindBuffer(GL_ARRAY_BUFFER, HWBuffer->vbo_verticesID);

	//copy data to graphics card
	glGetError(); // clear error storage
	if (!newBuffer)
		extGlBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * vertexSize, vbuf);
	else
	{
		HWBuffer->vbo_verticesSize = vertexCount*vertexSize;

		if (HWBuffer->Mapped_Vertex==scene::EHM_STATIC)
			extGlBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, vbuf, GL_STATIC_DRAW);
		else if (HWBuffer->Mapped_Vertex==scene::EHM_DYNAMIC)
			extGlBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, vbuf, GL_DYNAMIC_DRAW);
		else //scene::EHM_STREAM
			extGlBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, vbuf, GL_STREAM_DRAW);
	}

	extGlBindBuffer(GL_ARRAY_BUFFER, 0);

	return (glGetError() == GL_NO_ERROR);
#else
	return false;
#endif
}


bool COpenGLDriver::updateIndexHardwareBuffer(SHWBufferLink_opengl *HWBuffer)
{
	if (!HWBuffer)
		return false;

	if (!FeatureAvailable[IRR_ARB_vertex_buffer_object])
		return false;

#if defined(GL_ARB_vertex_buffer_object)
	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;

	const void* indices=mb->getIndices();
	u32 indexCount= mb->getIndexCount();

	GLenum indexSize;
	switch (mb->getIndexType())
	{
		case EIT_16BIT:
		{
			indexSize=sizeof(u16);
			break;
		}
		case EIT_32BIT:
		{
			indexSize=sizeof(u32);
			break;
		}
		default:
		{
			return false;
		}
	}


	//get or create buffer
	bool newBuffer=false;
	if (!HWBuffer->vbo_indicesID)
	{
		extGlGenBuffers(1, &HWBuffer->vbo_indicesID);
		if (!HWBuffer->vbo_indicesID)
			return false;
		newBuffer=true;
	}
	else if (HWBuffer->vbo_indicesSize < indexCount*indexSize)
	{
		newBuffer=true;
	}

	extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HWBuffer->vbo_indicesID);

	//copy data to graphics card
	glGetError(); // clear error storage
	if (!newBuffer)
		extGlBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexCount * indexSize, indices);
	else
	{
		HWBuffer->vbo_indicesSize = indexCount*indexSize;

		if (HWBuffer->Mapped_Index==scene::EHM_STATIC)
			extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_STATIC_DRAW);
		else if (HWBuffer->Mapped_Index==scene::EHM_DYNAMIC)
			extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_DYNAMIC_DRAW);
		else //scene::EHM_STREAM
			extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_STREAM_DRAW);
	}

	extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return (glGetError() == GL_NO_ERROR);
#else
	return false;
#endif
}


//! updates hardware buffer if needed
bool COpenGLDriver::updateHardwareBuffer(SHWBufferLink *HWBuffer)
{
	if (!HWBuffer)
		return false;

	if (HWBuffer->Mapped_Vertex!=scene::EHM_NEVER)
	{
		if (HWBuffer->ChangedID_Vertex != HWBuffer->MeshBuffer->getChangedID_Vertex()
			|| !((SHWBufferLink_opengl*)HWBuffer)->vbo_verticesID)
		{

			HWBuffer->ChangedID_Vertex = HWBuffer->MeshBuffer->getChangedID_Vertex();

			if (!updateVertexHardwareBuffer((SHWBufferLink_opengl*)HWBuffer))
				return false;
		}
	}

	if (HWBuffer->Mapped_Index!=scene::EHM_NEVER)
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
COpenGLDriver::SHWBufferLink *COpenGLDriver::createHardwareBuffer(const scene::IMeshBuffer* mb)
{
#if defined(GL_ARB_vertex_buffer_object)
	if (!mb || (mb->getHardwareMappingHint_Index()==scene::EHM_NEVER && mb->getHardwareMappingHint_Vertex()==scene::EHM_NEVER))
		return 0;

	SHWBufferLink_opengl *HWBuffer=new SHWBufferLink_opengl(mb);

	//add to map
	HWBufferMap.insert(HWBuffer->MeshBuffer, HWBuffer);

	HWBuffer->ChangedID_Vertex=HWBuffer->MeshBuffer->getChangedID_Vertex();
	HWBuffer->ChangedID_Index=HWBuffer->MeshBuffer->getChangedID_Index();
	HWBuffer->Mapped_Vertex=mb->getHardwareMappingHint_Vertex();
	HWBuffer->Mapped_Index=mb->getHardwareMappingHint_Index();
	HWBuffer->LastUsed=0;
	HWBuffer->vbo_verticesID=0;
	HWBuffer->vbo_indicesID=0;
	HWBuffer->vbo_verticesSize=0;
	HWBuffer->vbo_indicesSize=0;

	if (!updateHardwareBuffer(HWBuffer))
	{
		deleteHardwareBuffer(HWBuffer);
		return 0;
	}

	return HWBuffer;
#else
	return 0;
#endif
}


void COpenGLDriver::deleteHardwareBuffer(SHWBufferLink *_HWBuffer)
{
	if (!_HWBuffer)
		return;

#if defined(GL_ARB_vertex_buffer_object)
	SHWBufferLink_opengl *HWBuffer=(SHWBufferLink_opengl*)_HWBuffer;
	if (HWBuffer->vbo_verticesID)
	{
		extGlDeleteBuffers(1, &HWBuffer->vbo_verticesID);
		HWBuffer->vbo_verticesID=0;
	}
	if (HWBuffer->vbo_indicesID)
	{
		extGlDeleteBuffers(1, &HWBuffer->vbo_indicesID);
		HWBuffer->vbo_indicesID=0;
	}
#endif

	CNullDriver::deleteHardwareBuffer(_HWBuffer);
}


//! Draw hardware buffer
void COpenGLDriver::drawHardwareBuffer(SHWBufferLink *_HWBuffer)
{
	if (!_HWBuffer)
		return;

	updateHardwareBuffer(_HWBuffer); //check if update is needed
	_HWBuffer->LastUsed=0; //reset count

#if defined(GL_ARB_vertex_buffer_object)
	SHWBufferLink_opengl *HWBuffer=(SHWBufferLink_opengl*)_HWBuffer;

	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
	const void *vertices=mb->getVertices();
	const void *indexList=mb->getIndices();

	if (HWBuffer->Mapped_Vertex!=scene::EHM_NEVER)
	{
		extGlBindBuffer(GL_ARRAY_BUFFER, HWBuffer->vbo_verticesID);
		vertices=0;
	}

	if (HWBuffer->Mapped_Index!=scene::EHM_NEVER)
	{
		extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HWBuffer->vbo_indicesID);
		indexList=0;
	}

	drawVertexPrimitiveList(vertices, mb->getVertexCount(), indexList, mb->getIndexCount()/3, mb->getVertexType(), scene::EPT_TRIANGLES, mb->getIndexType());

	if (HWBuffer->Mapped_Vertex!=scene::EHM_NEVER)
		extGlBindBuffer(GL_ARRAY_BUFFER, 0);
	if (HWBuffer->Mapped_Index!=scene::EHM_NEVER)
		extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
}


//! Create occlusion query.
/** Use node for identification and mesh for occlusion test. */
void COpenGLDriver::addOcclusionQuery(scene::ISceneNode* node,
		const scene::IMesh* mesh)
{
	if (!queryFeature(EVDF_OCCLUSION_QUERY))
		return;

	CNullDriver::addOcclusionQuery(node, mesh);
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if ((index != -1) && (OcclusionQueries[index].UID == 0))
		extGlGenQueries(1, reinterpret_cast<GLuint*>(&OcclusionQueries[index].UID));
}


//! Remove occlusion query.
void COpenGLDriver::removeOcclusionQuery(scene::ISceneNode* node)
{
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
	{
		if (OcclusionQueries[index].UID != 0)
			extGlDeleteQueries(1, reinterpret_cast<GLuint*>(&OcclusionQueries[index].UID));
		CNullDriver::removeOcclusionQuery(node);
	}
}


//! Run occlusion query. Draws mesh stored in query.
/** If the mesh shall not be rendered visible, use
overrideMaterial to disable the color and depth buffer. */
void COpenGLDriver::runOcclusionQuery(scene::ISceneNode* node, bool visible)
{
	if (!node)
		return;

	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
	{
		if (OcclusionQueries[index].UID)
			extGlBeginQuery(
#ifdef GL_ARB_occlusion_query
				GL_SAMPLES_PASSED_ARB,
#else
				0,
#endif
				OcclusionQueries[index].UID);
		CNullDriver::runOcclusionQuery(node,visible);
		if (OcclusionQueries[index].UID)
			extGlEndQuery(
#ifdef GL_ARB_occlusion_query
				GL_SAMPLES_PASSED_ARB);
#else
				0);
#endif
		if ( testGLError() )
		{
			os::Printer::log("Occlusion Query failed", ELL_ERROR);
		}
	}
}


//! Update occlusion query. Retrieves results from GPU.
/** If the query shall not block, set the flag to false.
Update might not occur in this case, though */
void COpenGLDriver::updateOcclusionQuery(scene::ISceneNode* node, bool block)
{
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
	{
		// not yet started
		if (OcclusionQueries[index].Run==u32(~0))
			return;
		GLint available = block?GL_TRUE:GL_FALSE;
		if (!block)
        {
			extGlGetQueryObjectiv(OcclusionQueries[index].UID,
#ifdef GL_ARB_occlusion_query
						GL_QUERY_RESULT_AVAILABLE_ARB,
#elif defined(GL_NV_occlusion_query)
						GL_PIXEL_COUNT_AVAILABLE_NV,
#else
						0,
#endif
						&available);
			if ( testGLError() )
			{
				os::Printer::log("extGlGetQueryObjectiv failed", ELL_ERROR);
			}
		}
		if (available==GL_TRUE)
		{
			extGlGetQueryObjectiv(OcclusionQueries[index].UID,
#ifdef GL_ARB_occlusion_query
						GL_QUERY_RESULT_ARB,
#elif defined(GL_NV_occlusion_query)
						GL_PIXEL_COUNT_NV,
#else
						0,
#endif
						&available);
			if (queryFeature(EVDF_OCCLUSION_QUERY))
				OcclusionQueries[index].Result = available;
		}
		if ( testGLError() )
		{
			os::Printer::log("extGlGetQueryObjectiv failed", ELL_ERROR);
		}
	}
}


//! Return query result.
/** Return value is the number of visible pixels/fragments.
The value is a safe approximation, i.e. can be larger than the
actual value of pixels. */
u32 COpenGLDriver::getOcclusionQueryResult(scene::ISceneNode* node) const
{
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
		return OcclusionQueries[index].Result;
	else
		return ~0;
}


// small helper function to create vertex buffer object adress offsets
static inline u8* buffer_offset(const long offset)
{
	return ((u8*)0 + offset);
}


//! draws a vertex primitive list
void COpenGLDriver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
		const void* indexList, u32 primitiveCount,
		E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!primitiveCount || !vertexCount)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);

	if (vertices && !FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(vertices, vertexCount, vType);

	// draw everything
	setRenderStates3DMode();

	if ((pType!=scene::EPT_POINTS) && (pType!=scene::EPT_POINT_SPRITES))
		BridgeCalls->setClientState(true, true, true, true);
	else
		BridgeCalls->setClientState(true, false, true, false);

//due to missing defines in OSX headers, we have to be more specific with this check
//#if defined(GL_ARB_vertex_array_bgra) || defined(GL_EXT_vertex_array_bgra)
#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (vertices)
	{
		if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		{
			switch (vType)
			{
				case EVT_STANDARD:
					glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Color);
					break;
				case EVT_2TCOORDS:
					glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Color);
					break;
				case EVT_TANGENTS:
					glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Color);
					break;
			}
		}
		else
		{
			// avoid passing broken pointer to OpenGL
			_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
			glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
		}
	}

	switch (vType)
	{
		case EVT_STANDARD:
			if (vertices)
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Normal);
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Pos);
			}
			else
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertex), buffer_offset(12));
				glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), buffer_offset(28));
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex), 0);
			}

			if (MultiTextureExtension && CurrentTexture[1])
			{
				BridgeCalls->setClientActiveTexture(GL_TEXTURE1_ARB);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);
				else
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), buffer_offset(28));
			}
			break;
		case EVT_2TCOORDS:
			if (vertices)
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Normal);
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords);
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Pos);
			}
			else
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(12));
				glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex2TCoords), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(28));
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(0));
			}


			if (MultiTextureExtension)
			{
				BridgeCalls->setClientActiveTexture(GL_TEXTURE1_ARB);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords2);
				else
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(36));
			}
			break;
		case EVT_TANGENTS:
			if (vertices)
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Normal);
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].TCoords);
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Pos);
			}
			else
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(12));
				glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertexTangents), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(28));
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(0));
			}

			if (MultiTextureExtension)
			{
				BridgeCalls->setClientActiveTexture(GL_TEXTURE1_ARB);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Tangent);
				else
					glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(36));

				BridgeCalls->setClientActiveTexture(GL_TEXTURE2_ARB);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Binormal);
				else
					glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(48));
			}
			break;
	}

	renderArray(indexList, primitiveCount, pType, iType);

	if (MultiTextureExtension)
	{
		if (vType==EVT_TANGENTS)
		{
			BridgeCalls->setClientActiveTexture(GL_TEXTURE2_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if ((vType!=EVT_STANDARD) || CurrentTexture[1])
		{
			BridgeCalls->setClientActiveTexture(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		BridgeCalls->setClientActiveTexture(GL_TEXTURE0_ARB);
	}
}


void COpenGLDriver::getColorBuffer(const void* vertices, u32 vertexCount, E_VERTEX_TYPE vType)
{
	// convert colors to gl color format.
	vertexCount *= 4; //reused as color component count
	ColorBuffer.set_used(vertexCount);
	u32 i;

	switch (vType)
	{
		case EVT_STANDARD:
		{
			const S3DVertex* p = static_cast<const S3DVertex*>(vertices);
			for (i=0; i<vertexCount; i+=4)
			{
				p->Color.toOpenGLColor(&ColorBuffer[i]);
				++p;
			}
		}
		break;
		case EVT_2TCOORDS:
		{
			const S3DVertex2TCoords* p = static_cast<const S3DVertex2TCoords*>(vertices);
			for (i=0; i<vertexCount; i+=4)
			{
				p->Color.toOpenGLColor(&ColorBuffer[i]);
				++p;
			}
		}
		break;
		case EVT_TANGENTS:
		{
			const S3DVertexTangents* p = static_cast<const S3DVertexTangents*>(vertices);
			for (i=0; i<vertexCount; i+=4)
			{
				p->Color.toOpenGLColor(&ColorBuffer[i]);
				++p;
			}
		}
		break;
	}
}


void COpenGLDriver::renderArray(const void* indexList, u32 primitiveCount,
		scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	GLenum indexSize=0;

	switch (iType)
	{
		case EIT_16BIT:
		{
			indexSize=GL_UNSIGNED_SHORT;
			break;
		}
		case EIT_32BIT:
		{
			indexSize=GL_UNSIGNED_INT;
			break;
		}
	}

	switch (pType)
	{
		case scene::EPT_POINTS:
		case scene::EPT_POINT_SPRITES:
		{
#ifdef GL_ARB_point_sprite
			if (pType==scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
				glEnable(GL_POINT_SPRITE_ARB);
#endif

			// prepare size and attenuation (where supported)
			GLfloat particleSize=Material.Thickness;
//			if (AntiAlias)
//				particleSize=core::clamp(particleSize, DimSmoothedPoint[0], DimSmoothedPoint[1]);
//			else
				particleSize=core::clamp(particleSize, DimAliasedPoint[0], DimAliasedPoint[1]);
#if defined(GL_VERSION_1_4) || defined(GL_ARB_point_parameters) || defined(GL_EXT_point_parameters) || defined(GL_SGIS_point_parameters)
			const float att[] = {1.0f, 1.0f, 0.0f};
#if defined(GL_VERSION_1_4)
			extGlPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att);
//			extGlPointParameterf(GL_POINT_SIZE_MIN,1.f);
			extGlPointParameterf(GL_POINT_SIZE_MAX, particleSize);
			extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 1.0f);
#elif defined(GL_ARB_point_parameters)
			extGlPointParameterfv(GL_POINT_DISTANCE_ATTENUATION_ARB, att);
//			extGlPointParameterf(GL_POINT_SIZE_MIN_ARB,1.f);
			extGlPointParameterf(GL_POINT_SIZE_MAX_ARB, particleSize);
			extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f);
#elif defined(GL_EXT_point_parameters)
			extGlPointParameterfv(GL_DISTANCE_ATTENUATION_EXT, att);
//			extGlPointParameterf(GL_POINT_SIZE_MIN_EXT,1.f);
			extGlPointParameterf(GL_POINT_SIZE_MAX_EXT, particleSize);
			extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_EXT, 1.0f);
#elif defined(GL_SGIS_point_parameters)
			extGlPointParameterfv(GL_DISTANCE_ATTENUATION_SGIS, att);
//			extGlPointParameterf(GL_POINT_SIZE_MIN_SGIS,1.f);
			extGlPointParameterf(GL_POINT_SIZE_MAX_SGIS, particleSize);
			extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_SGIS, 1.0f);
#endif
#endif
			glPointSize(particleSize);

#ifdef GL_ARB_point_sprite
			if (pType==scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
				glTexEnvf(GL_POINT_SPRITE_ARB,GL_COORD_REPLACE, GL_TRUE);
#endif
			glDrawArrays(GL_POINTS, 0, primitiveCount);
#ifdef GL_ARB_point_sprite
			if (pType==scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
			{
				glDisable(GL_POINT_SPRITE_ARB);
				glTexEnvf(GL_POINT_SPRITE_ARB,GL_COORD_REPLACE, GL_FALSE);
			}
#endif
		}
			break;
		case scene::EPT_LINE_STRIP:
			glDrawElements(GL_LINE_STRIP, primitiveCount+1, indexSize, indexList);
			break;
		case scene::EPT_LINE_LOOP:
			glDrawElements(GL_LINE_LOOP, primitiveCount, indexSize, indexList);
			break;
		case scene::EPT_LINES:
			glDrawElements(GL_LINES, primitiveCount*2, indexSize, indexList);
			break;
		case scene::EPT_TRIANGLE_STRIP:
			glDrawElements(GL_TRIANGLE_STRIP, primitiveCount+2, indexSize, indexList);
			break;
		case scene::EPT_TRIANGLE_FAN:
			glDrawElements(GL_TRIANGLE_FAN, primitiveCount+2, indexSize, indexList);
			break;
		case scene::EPT_TRIANGLES:
			glDrawElements(GL_TRIANGLES, primitiveCount*3, indexSize, indexList);
			break;
		case scene::EPT_QUAD_STRIP:
			glDrawElements(GL_QUAD_STRIP, primitiveCount*2+2, indexSize, indexList);
			break;
		case scene::EPT_QUADS:
			glDrawElements(GL_QUADS, primitiveCount*4, indexSize, indexList);
			break;
		case scene::EPT_POLYGON:
			glDrawElements(GL_POLYGON, primitiveCount, indexSize, indexList);
			break;
	}
}


//! draws a vertex primitive list in 2d
void COpenGLDriver::draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
		const void* indexList, u32 primitiveCount,
		E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!primitiveCount || !vertexCount)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::draw2DVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);

	if (vertices && !FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(vertices, vertexCount, vType);

	// draw everything
	this->setActiveTexture(0, Material.getTexture(0));
	if (Material.MaterialType==EMT_ONETEXTURE_BLEND)
	{
		E_BLEND_FACTOR srcFact;
		E_BLEND_FACTOR dstFact;
		E_MODULATE_FUNC modulo;
		u32 alphaSource;
		unpack_textureBlendFunc ( srcFact, dstFact, modulo, alphaSource, Material.MaterialTypeParam);
		setRenderStates2DMode(alphaSource&video::EAS_VERTEX_COLOR, (Material.getTexture(0) != 0), (alphaSource&video::EAS_TEXTURE) != 0);
	}
	else
		setRenderStates2DMode(Material.MaterialType==EMT_TRANSPARENT_VERTEX_ALPHA, (Material.getTexture(0) != 0), Material.MaterialType==EMT_TRANSPARENT_ALPHA_CHANNEL);

	if ((pType!=scene::EPT_POINTS) && (pType!=scene::EPT_POINT_SPRITES))
		BridgeCalls->setClientState(true, false, true, true);
	else
		BridgeCalls->setClientState(true, false, true, false);

//due to missing defines in OSX headers, we have to be more specific with this check
//#if defined(GL_ARB_vertex_array_bgra) || defined(GL_EXT_vertex_array_bgra)
#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (vertices)
	{
		if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		{
			switch (vType)
			{
				case EVT_STANDARD:
					glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Color);
					break;
				case EVT_2TCOORDS:
					glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Color);
					break;
				case EVT_TANGENTS:
					glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Color);
					break;
			}
		}
		else
		{
			// avoid passing broken pointer to OpenGL
			_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
			glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
		}
	}

	switch (vType)
	{
		case EVT_STANDARD:
			if (vertices)
			{
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);
				glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Pos);
			}
			else
			{
				glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), buffer_offset(28));
				glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), 0);
			}

			if (MultiTextureExtension && CurrentTexture[1])
			{
				BridgeCalls->setClientActiveTexture(GL_TEXTURE1_ARB);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);
				else
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), buffer_offset(28));
			}
			break;
		case EVT_2TCOORDS:
			if (vertices)
			{
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords);
				glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Pos);
			}
			else
			{
				glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex2TCoords), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(28));
				glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(0));
			}

			if (MultiTextureExtension)
			{
				BridgeCalls->setClientActiveTexture(GL_TEXTURE1_ARB);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords2);
				else
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(36));
			}
			break;
		case EVT_TANGENTS:
			if (vertices)
			{
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].TCoords);
				glVertexPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Pos);
			}
			else
			{
				glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertexTangents), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(28));
				glVertexPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(0));
			}

			break;
	}

	renderArray(indexList, primitiveCount, pType, iType);

	if (MultiTextureExtension)
	{
		if ((vType!=EVT_STANDARD) || CurrentTexture[1])
		{
			BridgeCalls->setClientActiveTexture(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		BridgeCalls->setClientActiveTexture(GL_TEXTURE0_ARB);
	}
}


//! draws a set of 2d images, using a color and the alpha channel of the
//! texture if desired.
void COpenGLDriver::draw2DImageBatch(const video::ITexture* texture,
				const core::array<core::position2d<s32> >& positions,
				const core::array<core::rect<s32> >& sourceRects,
				const core::rect<s32>* clipRect,
				SColor color,
				bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	const u32 drawCount = core::min_<u32>(positions.size(), sourceRects.size());

	const core::dimension2d<u32>& ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	disableTextures(1);
	if (!setActiveTexture(0, texture))
		return;
	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

	Quad2DVertices[0].Color = color;
	Quad2DVertices[1].Color = color;
	Quad2DVertices[2].Color = color;
	Quad2DVertices[3].Color = color;

	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(Quad2DVertices, 4, EVT_STANDARD);

	BridgeCalls->setClientState(true, false, true, true);

	glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].TCoords);
	glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
	else
	{
		_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	}

	for (u32 i=0; i<drawCount; ++i)
	{
		if (!sourceRects[i].isValid())
			continue;

		core::position2d<s32> targetPos(positions[i]);
		core::position2d<s32> sourcePos(sourceRects[i].UpperLeftCorner);
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

			if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
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

			if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
			{
				sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
				if (sourceSize.Height <= 0)
					continue;
			}
		}

		// clip these coordinates

		if (targetPos.X<0)
		{
			sourceSize.Width += targetPos.X;
			if (sourceSize.Width <= 0)
				continue;

			sourcePos.X -= targetPos.X;
			targetPos.X = 0;
		}

		if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
			if (sourceSize.Width <= 0)
				continue;
		}

		if (targetPos.Y<0)
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

		const core::rect<f32> tcoords(
				sourcePos.X * invW,
				sourcePos.Y * invH,
				(sourcePos.X + sourceSize.Width) * invW,
				(sourcePos.Y + sourceSize.Height) * invH);

		const core::rect<s32> poss(targetPos, sourceSize);

		Quad2DVertices[0].Pos = core::vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f);
		Quad2DVertices[1].Pos = core::vector3df((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f);
		Quad2DVertices[2].Pos = core::vector3df((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f);
		Quad2DVertices[3].Pos = core::vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f);

		Quad2DVertices[0].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		Quad2DVertices[1].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		Quad2DVertices[2].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		Quad2DVertices[3].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, Quad2DIndices);
	}
}


//! draws a 2d image, using a color and the alpha channel of the texture if
//! desired. The image is drawn at pos, clipped against clipRect (if != 0).
//! Only the subtexture defined by sourceRect is used.
void COpenGLDriver::draw2DImage(const video::ITexture* texture,
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
	// This needs to be signed as it may go negative.
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

	if (targetPos.X<0)
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

	if (targetPos.Y<0)
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

	const core::dimension2d<u32>& ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::rect<f32> tcoords(
			sourcePos.X * invW,
			sourcePos.Y * invH,
			(sourcePos.X + sourceSize.Width) * invW,
			(sourcePos.Y + sourceSize.Height) * invH);

	const core::rect<s32> poss(targetPos, sourceSize);

	disableTextures(1);
	if (!setActiveTexture(0, texture))
		return;
	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

	Quad2DVertices[0].Color = color;
	Quad2DVertices[1].Color = color;
	Quad2DVertices[2].Color = color;
	Quad2DVertices[3].Color = color;

	Quad2DVertices[0].Pos = core::vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f);

	Quad2DVertices[0].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[1].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[2].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	Quad2DVertices[3].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(Quad2DVertices, 4, EVT_STANDARD);

	BridgeCalls->setClientState(true, false, true, true);

	glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].TCoords);
	glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
	else
	{
		_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	}

	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, Quad2DIndices);
}


//! The same, but with a four element array of colors, one for each vertex
void COpenGLDriver::draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
		const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
		const video::SColor* const colors, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	const core::dimension2d<u32>& ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::rect<f32> tcoords(
			sourceRect.UpperLeftCorner.X * invW,
			sourceRect.UpperLeftCorner.Y * invH,
			sourceRect.LowerRightCorner.X * invW,
			sourceRect.LowerRightCorner.Y *invH);

	const video::SColor temp[4] =
	{
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF
	};

	const video::SColor* const useColor = colors ? colors : temp;

	disableTextures(1);
	if (!setActiveTexture(0, texture))
		return;
	setRenderStates2DMode(useColor[0].getAlpha()<255 || useColor[1].getAlpha()<255 ||
			useColor[2].getAlpha()<255 || useColor[3].getAlpha()<255,
			true, useAlphaChannelOfTexture);

	if (clipRect)
	{
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height-clipRect->LowerRightCorner.Y,
			clipRect->getWidth(), clipRect->getHeight());
	}

	Quad2DVertices[0].Color = useColor[0];
	Quad2DVertices[1].Color = useColor[3];
	Quad2DVertices[2].Color = useColor[2];
	Quad2DVertices[3].Color = useColor[1];

	Quad2DVertices[0].Pos = core::vector3df((f32)destRect.UpperLeftCorner.X, (f32)destRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)destRect.LowerRightCorner.X, (f32)destRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)destRect.LowerRightCorner.X, (f32)destRect.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)destRect.UpperLeftCorner.X, (f32)destRect.LowerRightCorner.Y, 0.0f);

	Quad2DVertices[0].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[1].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[2].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	Quad2DVertices[3].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(Quad2DVertices, 4, EVT_STANDARD);

	BridgeCalls->setClientState(true, false, true, true);

	glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].TCoords);
	glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
	else
	{
		_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	}

	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, Quad2DIndices);

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}


//! draws a set of 2d images, using a color and the alpha channel of the
//! texture if desired. The images are drawn beginning at pos and concatenated
//! in one line. All drawings are clipped against clipRect (if != 0).
//! The subtextures are defined by the array of sourceRects and are chosen
//! by the indices given.
void COpenGLDriver::draw2DImageBatch(const video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::array<core::rect<s32> >& sourceRects,
				const core::array<s32>& indices,
				s32 kerningWidth,
				const core::rect<s32>* clipRect, SColor color,
				bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	disableTextures(1);
	if (!setActiveTexture(0, texture))
		return;
	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

	if (clipRect)
	{
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height-clipRect->LowerRightCorner.Y,
			clipRect->getWidth(),clipRect->getHeight());
	}

	const core::dimension2d<u32>& ss = texture->getOriginalSize();
	core::position2d<s32> targetPos(pos);
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);

	Quad2DVertices[0].Color = color;
	Quad2DVertices[1].Color = color;
	Quad2DVertices[2].Color = color;
	Quad2DVertices[3].Color = color;

	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(Quad2DVertices, 4, EVT_STANDARD);

	BridgeCalls->setClientState(true, false, true, true);

	glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].TCoords);
	glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
	else
	{
		_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	}

	for (u32 i=0; i<indices.size(); ++i)
	{
		const s32 currentIndex = indices[i];
		if (!sourceRects[currentIndex].isValid())
			break;

		const core::rect<f32> tcoords(
				sourceRects[currentIndex].UpperLeftCorner.X * invW,
				sourceRects[currentIndex].UpperLeftCorner.Y * invH,
				sourceRects[currentIndex].LowerRightCorner.X * invW,
				sourceRects[currentIndex].LowerRightCorner.Y * invH);

		const core::rect<s32> poss(targetPos, sourceRects[currentIndex].getSize());

		Quad2DVertices[0].Pos = core::vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f);
		Quad2DVertices[1].Pos = core::vector3df((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f);
		Quad2DVertices[2].Pos = core::vector3df((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f);
		Quad2DVertices[3].Pos = core::vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f);

		Quad2DVertices[0].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		Quad2DVertices[1].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		Quad2DVertices[2].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		Quad2DVertices[3].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, Quad2DIndices);

		targetPos.X += sourceRects[currentIndex].getWidth();
	}

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}


//! draw a 2d rectangle
void COpenGLDriver::draw2DRectangle(SColor color, const core::rect<s32>& position,
		const core::rect<s32>* clip)
{
	disableTextures();
	setRenderStates2DMode(color.getAlpha() < 255, false, false);

	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glRectf(GLfloat(pos.UpperLeftCorner.X), GLfloat(pos.UpperLeftCorner.Y),
		GLfloat(pos.LowerRightCorner.X), GLfloat(pos.LowerRightCorner.Y));
}


//! draw an 2d rectangle
void COpenGLDriver::draw2DRectangle(const core::rect<s32>& position,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
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

	Quad2DVertices[0].Color = colorLeftUp;
	Quad2DVertices[1].Color = colorRightUp;
	Quad2DVertices[2].Color = colorRightDown;
	Quad2DVertices[3].Color = colorLeftDown;

	Quad2DVertices[0].Pos = core::vector3df((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0.0f);

	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(Quad2DVertices, 4, EVT_STANDARD);

	BridgeCalls->setClientState(true, false, true, false);

	glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
	else
	{
		_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	}

	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, Quad2DIndices);
}


//! Draws a 2d line.
void COpenGLDriver::draw2DLine(const core::position2d<s32>& start,
				const core::position2d<s32>& end, SColor color)
{
	if (start==end)
		drawPixel(start.X, start.Y, color);
	else
	{
		disableTextures();
		setRenderStates2DMode(color.getAlpha() < 255, false, false);

		Quad2DVertices[0].Color = color;
		Quad2DVertices[1].Color = color;

		Quad2DVertices[0].Pos = core::vector3df((f32)start.X, (f32)start.Y, 0.0f);
		Quad2DVertices[1].Pos = core::vector3df((f32)end.X, (f32)end.Y, 0.0f);

		if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
			getColorBuffer(Quad2DVertices, 2, EVT_STANDARD);

		BridgeCalls->setClientState(true, false, true, false);

		glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
		const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
		const GLint colorSize=4;
#endif
		if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
			glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
		else
		{
			_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
			glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
		}

		glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, Quad2DIndices);
	}
}

//! Draws a pixel
void COpenGLDriver::drawPixel(u32 x, u32 y, const SColor &color)
{
	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
	if (x > (u32)renderTargetSize.Width || y > (u32)renderTargetSize.Height)
		return;

	disableTextures();
	setRenderStates2DMode(color.getAlpha() < 255, false, false);

	Quad2DVertices[0].Color = color;

	Quad2DVertices[0].Pos = core::vector3df((f32)x, (f32)y, 0.0f);

	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(Quad2DVertices, 1, EVT_STANDARD);

	BridgeCalls->setClientState(true, false, true, false);

	glVertexPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
	else
	{
		_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	}

	glDrawArrays(GL_POINTS, 0, 1);
}

bool COpenGLDriver::setActiveTexture(u32 stage, const video::ITexture* texture)
{
	if (stage >= MaxSupportedTextures)
		return false;

	if (CurrentTexture[stage]==texture)
		return true;

	CurrentTexture.set(stage,texture);

	if (!texture)
		return true;
	else if (texture->getDriverType() != EDT_OPENGL)
	{
		CurrentTexture.set(stage, 0);
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	return true;
}


//! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
//! Returns whether disabling was successful or not.
bool COpenGLDriver::disableTextures(u32 fromStage)
{
	bool result=true;
	for (u32 i=fromStage; i<MaxSupportedTextures; ++i)
	{
		result &= setActiveTexture(i, 0);
		BridgeCalls->setTexture(i, GL_TEXTURE_2D, true);
	}
	return result;
}


//! creates a matrix in supplied GLfloat array to pass to OpenGL
inline void COpenGLDriver::getGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m)
{
	memcpy(gl_matrix, m.pointer(), 16 * sizeof(f32));
}


//! creates a opengltexturematrix from a D3D style texture matrix
inline void COpenGLDriver::getGLTextureMatrix(GLfloat *o, const core::matrix4& m)
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
ITexture* COpenGLDriver::createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData)
{
	COpenGLTexture* texture = 0;

	if (surface && checkColorFormat(surface->getColorFormat(), surface->getDimension()))
		texture = new COpenGLTexture(surface, name, mipmapData, this);

	return texture;
}


//! returns a device dependent texture from a software surface (IImage)
ITexture* COpenGLDriver::createDeviceDependentTextureCube(const io::path& name, IImage* posXImage, IImage* negXImage,
	IImage* posYImage, IImage* negYImage, IImage* posZImage, IImage* negZImage)
{
	COpenGLTexture* texture = 0;

	if (posXImage && negXImage && posYImage && negYImage && posZImage && negZImage &&
		checkTextureCube(posXImage, negXImage, posYImage, negYImage, posZImage, negZImage) &&
		checkColorFormat(posXImage->getColorFormat(), posXImage->getDimension()))
	{
		texture = new COpenGLTexture(name, posXImage, negXImage, posYImage, negYImage, posZImage, negZImage, this);
	}

	return texture;
}


//! Sets a material. All 3d drawing functions draw geometry now using this material.
void COpenGLDriver::setMaterial(const SMaterial& material)
{
	Material = material;
	OverrideMaterial.apply(Material);

	for (u32 i = 0; i < MaxTextureUnits; ++i)
		setActiveTexture(i, material.getTexture(i));
}


//! prints error if an error happened.
bool COpenGLDriver::testGLError()
{
#ifdef _DEBUG
	GLenum g = glGetError();
	switch (g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		os::Printer::log("GL_INVALID_ENUM", ELL_ERROR); break;
	case GL_INVALID_VALUE:
		os::Printer::log("GL_INVALID_VALUE", ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR); break;
	case GL_STACK_OVERFLOW:
		os::Printer::log("GL_STACK_OVERFLOW", ELL_ERROR); break;
	case GL_STACK_UNDERFLOW:
		os::Printer::log("GL_STACK_UNDERFLOW", ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR); break;
	case GL_TABLE_TOO_LARGE:
		os::Printer::log("GL_TABLE_TOO_LARGE", ELL_ERROR); break;
#if defined(GL_EXT_framebuffer_object)
	case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
		os::Printer::log("GL_INVALID_FRAMEBUFFER_OPERATION", ELL_ERROR); break;
#endif
	};
//	_IRR_DEBUG_BREAK_IF(true);
	return true;
#else
	return false;
#endif
}


//! sets the needed renderstates
void COpenGLDriver::setRenderStates3DMode()
{
	if (CurrentRenderMode != ERM_3D)
	{
		// Reset Texture Stages
		BridgeCalls->setBlend(false);
		BridgeCalls->setAlphaTest(false);
		BridgeCalls->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// switch back the matrices
		BridgeCalls->setMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());

		BridgeCalls->setMatrixMode(GL_PROJECTION);
		glLoadMatrixf(Matrices[ETS_PROJECTION].pointer());

		ResetRenderStates = true;
#ifdef GL_EXT_clip_volume_hint
		if (FeatureAvailable[IRR_EXT_clip_volume_hint])
			glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_NICEST);
#endif
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

	CurrentRenderMode = ERM_3D;
}


//! Get native wrap mode value
GLint COpenGLDriver::getTextureWrapMode(const u8 clamp)
{
	GLint mode=GL_REPEAT;
	switch (clamp)
	{
		case ETC_REPEAT:
			mode=GL_REPEAT;
			break;
		case ETC_CLAMP:
			mode=GL_CLAMP;
			break;
		case ETC_CLAMP_TO_EDGE:
#ifdef GL_VERSION_1_2
			if (Version>101)
				mode=GL_CLAMP_TO_EDGE;
			else
#endif
#ifdef GL_SGIS_texture_edge_clamp
			if (FeatureAvailable[IRR_SGIS_texture_edge_clamp])
				mode=GL_CLAMP_TO_EDGE_SGIS;
			else
#endif
				// fallback
				mode=GL_CLAMP;
			break;
		case ETC_CLAMP_TO_BORDER:
#ifdef GL_VERSION_1_3
			if (Version>102)
				mode=GL_CLAMP_TO_BORDER;
			else
#endif
#ifdef GL_ARB_texture_border_clamp
			if (FeatureAvailable[IRR_ARB_texture_border_clamp])
				mode=GL_CLAMP_TO_BORDER_ARB;
			else
#endif
#ifdef GL_SGIS_texture_border_clamp
			if (FeatureAvailable[IRR_SGIS_texture_border_clamp])
				mode=GL_CLAMP_TO_BORDER_SGIS;
			else
#endif
				// fallback
				mode=GL_CLAMP;
			break;
		case ETC_MIRROR:
#ifdef GL_VERSION_1_4
			if (Version>103)
				mode=GL_MIRRORED_REPEAT;
			else
#endif
#ifdef GL_ARB_texture_border_clamp
			if (FeatureAvailable[IRR_ARB_texture_mirrored_repeat])
				mode=GL_MIRRORED_REPEAT_ARB;
			else
#endif
#ifdef GL_IBM_texture_mirrored_repeat
			if (FeatureAvailable[IRR_IBM_texture_mirrored_repeat])
				mode=GL_MIRRORED_REPEAT_IBM;
			else
#endif
				mode=GL_REPEAT;
			break;
		case ETC_MIRROR_CLAMP:
#ifdef GL_EXT_texture_mirror_clamp
			if (FeatureAvailable[IRR_EXT_texture_mirror_clamp])
				mode=GL_MIRROR_CLAMP_EXT;
			else
#endif
#if defined(GL_ATI_texture_mirror_once)
			if (FeatureAvailable[IRR_ATI_texture_mirror_once])
				mode=GL_MIRROR_CLAMP_ATI;
			else
#endif
				mode=GL_CLAMP;
			break;
		case ETC_MIRROR_CLAMP_TO_EDGE:
#ifdef GL_EXT_texture_mirror_clamp
			if (FeatureAvailable[IRR_EXT_texture_mirror_clamp])
				mode=GL_MIRROR_CLAMP_TO_EDGE_EXT;
			else
#endif
#if defined(GL_ATI_texture_mirror_once)
			if (FeatureAvailable[IRR_ATI_texture_mirror_once])
				mode=GL_MIRROR_CLAMP_TO_EDGE_ATI;
			else
#endif
				mode=GL_CLAMP;
			break;
		case ETC_MIRROR_CLAMP_TO_BORDER:
#ifdef GL_EXT_texture_mirror_clamp
			if (FeatureAvailable[IRR_EXT_texture_mirror_clamp])
				mode=GL_MIRROR_CLAMP_TO_BORDER_EXT;
			else
#endif
				mode=GL_CLAMP;
			break;
	}
	return mode;
}


//! Can be called by an IMaterialRenderer to make its work easier.
void COpenGLDriver::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
	bool resetAllRenderStates)
{
	// Fixed pipeline isn't important for shader based materials

	E_OPENGL_FIXED_PIPELINE_STATE tempState = FixedPipelineState;

	if (resetAllRenderStates || tempState == EOFPS_ENABLE || tempState == EOFPS_DISABLE_TO_ENABLE)
	{
		// material colors
		if (resetAllRenderStates || tempState == EOFPS_DISABLE_TO_ENABLE ||
			lastmaterial.ColorMaterial != material.ColorMaterial)
		{
			switch (material.ColorMaterial)
			{
			case ECM_NONE:
				glDisable(GL_COLOR_MATERIAL);
				break;
			case ECM_DIFFUSE:
				glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
				break;
			case ECM_AMBIENT:
				glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
				break;
			case ECM_EMISSIVE:
				glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
				break;
			case ECM_SPECULAR:
				glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
				break;
			case ECM_DIFFUSE_AND_AMBIENT:
				glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
				break;
			}
			if (material.ColorMaterial != ECM_NONE)
				glEnable(GL_COLOR_MATERIAL);
		}

		if (resetAllRenderStates || tempState == EOFPS_DISABLE_TO_ENABLE ||
			lastmaterial.AmbientColor != material.AmbientColor ||
			lastmaterial.DiffuseColor != material.DiffuseColor ||
			lastmaterial.EmissiveColor != material.EmissiveColor ||
			lastmaterial.ColorMaterial != material.ColorMaterial)
		{
			GLfloat color[4];

			const f32 inv = 1.0f / 255.0f;

			if ((material.ColorMaterial != video::ECM_AMBIENT) &&
				(material.ColorMaterial != video::ECM_DIFFUSE_AND_AMBIENT))
			{
				color[0] = material.AmbientColor.getRed() * inv;
				color[1] = material.AmbientColor.getGreen() * inv;
				color[2] = material.AmbientColor.getBlue() * inv;
				color[3] = material.AmbientColor.getAlpha() * inv;
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
			}

			if ((material.ColorMaterial != video::ECM_DIFFUSE) &&
				(material.ColorMaterial != video::ECM_DIFFUSE_AND_AMBIENT))
			{
				color[0] = material.DiffuseColor.getRed() * inv;
				color[1] = material.DiffuseColor.getGreen() * inv;
				color[2] = material.DiffuseColor.getBlue() * inv;
				color[3] = material.DiffuseColor.getAlpha() * inv;
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
			}

			if (material.ColorMaterial != video::ECM_EMISSIVE)
			{
				color[0] = material.EmissiveColor.getRed() * inv;
				color[1] = material.EmissiveColor.getGreen() * inv;
				color[2] = material.EmissiveColor.getBlue() * inv;
				color[3] = material.EmissiveColor.getAlpha() * inv;
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
			}
		}

		if (resetAllRenderStates || tempState == EOFPS_DISABLE_TO_ENABLE ||
			lastmaterial.SpecularColor != material.SpecularColor ||
			lastmaterial.Shininess != material.Shininess ||
			lastmaterial.ColorMaterial != material.ColorMaterial)
		{
			GLfloat color[4]={0.f,0.f,0.f,1.f};
			const f32 inv = 1.0f / 255.0f;

			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.Shininess);
			// disable Specular colors if no shininess is set
			if ((material.Shininess != 0.0f) &&
				(material.ColorMaterial != video::ECM_SPECULAR))
			{
#ifdef GL_EXT_separate_specular_color
				if (FeatureAvailable[IRR_EXT_separate_specular_color])
					glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif
				color[0] = material.SpecularColor.getRed() * inv;
				color[1] = material.SpecularColor.getGreen() * inv;
				color[2] = material.SpecularColor.getBlue() * inv;
				color[3] = material.SpecularColor.getAlpha() * inv;
			}
#ifdef GL_EXT_separate_specular_color
			else if (FeatureAvailable[IRR_EXT_separate_specular_color])
				glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
#endif
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
		}

		// shademode
		if (resetAllRenderStates || tempState == EOFPS_DISABLE_TO_ENABLE ||
			lastmaterial.GouraudShading != material.GouraudShading)
		{
			if (material.GouraudShading)
				glShadeModel(GL_SMOOTH);
			else
				glShadeModel(GL_FLAT);
		}

		// lighting
		if (resetAllRenderStates || tempState == EOFPS_DISABLE_TO_ENABLE ||
			lastmaterial.Lighting != material.Lighting)
		{
			if (material.Lighting)
				glEnable(GL_LIGHTING);
			else
				glDisable(GL_LIGHTING);
		}

		// fog
		if (resetAllRenderStates || tempState == EOFPS_DISABLE_TO_ENABLE ||
			lastmaterial.FogEnable != material.FogEnable)
		{
			if (material.FogEnable)
				glEnable(GL_FOG);
			else
				glDisable(GL_FOG);
		}

		// normalization
		if (resetAllRenderStates || tempState == EOFPS_DISABLE_TO_ENABLE ||
			lastmaterial.NormalizeNormals != material.NormalizeNormals)
		{
			if (material.NormalizeNormals)
				glEnable(GL_NORMALIZE);
			else
				glDisable(GL_NORMALIZE);
		}

		// Set fixed pipeline as active.
		tempState = EOFPS_ENABLE;
	}
	else if (tempState == EOFPS_ENABLE_TO_DISABLE)
	{
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		glDisable(GL_FOG);
		glDisable(GL_NORMALIZE);

		// Set programmable pipeline as active.
		tempState = EOFPS_DISABLE;
	}

	// tempState == EOFPS_DISABLE - driver doesn't calls functions related to fixed pipeline.

	// fillmode - fixed pipeline call, but it emulate GL_LINES behaviour in rendering, so it stay here.
	if (resetAllRenderStates || (lastmaterial.Wireframe != material.Wireframe) || (lastmaterial.PointCloud != material.PointCloud))
		glPolygonMode(GL_FRONT_AND_BACK, material.Wireframe ? GL_LINE : material.PointCloud? GL_POINT : GL_FILL);

	// zbuffer
	if (resetAllRenderStates || lastmaterial.ZBuffer != material.ZBuffer)
	{
		switch (material.ZBuffer)
		{
			case ECFN_DISABLED:
				BridgeCalls->setDepthTest(false);
				break;
			case ECFN_LESSEQUAL:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_LEQUAL);
				break;
			case ECFN_EQUAL:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_EQUAL);
				break;
			case ECFN_LESS:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_LESS);
				break;
			case ECFN_NOTEQUAL:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_NOTEQUAL);
				break;
			case ECFN_GREATEREQUAL:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_GEQUAL);
				break;
			case ECFN_GREATER:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_GREATER);
				break;
			case ECFN_ALWAYS:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_ALWAYS);
				break;
			case ECFN_NEVER:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_NEVER);
				break;
		}
	}

	// zwrite
//	if (resetAllRenderStates || lastmaterial.ZWriteEnable != material.ZWriteEnable)
	{
		if (material.ZWriteEnable && (AllowZWriteOnTransparent || !material.isTransparent()))
			BridgeCalls->setDepthMask(true);
		else
			BridgeCalls->setDepthMask(false);
	}

	// back face culling
	if (resetAllRenderStates || (lastmaterial.FrontfaceCulling != material.FrontfaceCulling) || (lastmaterial.BackfaceCulling != material.BackfaceCulling))
	{
		if ((material.FrontfaceCulling) && (material.BackfaceCulling))
		{
			BridgeCalls->setCullFaceFunc(GL_FRONT_AND_BACK);
			BridgeCalls->setCullFace(true);
		}
		else
		if (material.BackfaceCulling)
		{
			BridgeCalls->setCullFaceFunc(GL_BACK);
			BridgeCalls->setCullFace(true);
		}
		else
		if (material.FrontfaceCulling)
		{
			BridgeCalls->setCullFaceFunc(GL_FRONT);
			BridgeCalls->setCullFace(true);
		}
		else
			BridgeCalls->setCullFace(false);
	}

	// Color Mask
	if (resetAllRenderStates || lastmaterial.ColorMask != material.ColorMask)
	{
		glColorMask(
			(material.ColorMask & ECP_RED)?GL_TRUE:GL_FALSE,
			(material.ColorMask & ECP_GREEN)?GL_TRUE:GL_FALSE,
			(material.ColorMask & ECP_BLUE)?GL_TRUE:GL_FALSE,
			(material.ColorMask & ECP_ALPHA)?GL_TRUE:GL_FALSE);
	}

	if (queryFeature(EVDF_BLEND_OPERATIONS) &&
		(resetAllRenderStates|| lastmaterial.BlendOperation != material.BlendOperation))
	{
		if (material.BlendOperation==EBO_NONE)
			BridgeCalls->setBlend(false);
		else
		{
			BridgeCalls->setBlend(true);
#if defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op) || defined(GL_VERSION_1_2)
			switch (material.BlendOperation)
			{
			case EBO_SUBTRACT:
#if defined(GL_EXT_blend_subtract)
				if (FeatureAvailable[IRR_EXT_blend_subtract] || (Version>=120))
					extGlBlendEquation(GL_FUNC_SUBTRACT_EXT);
#elif defined(GL_VERSION_1_2)
				if (Version>=120)
					extGlBlendEquation(GL_FUNC_SUBTRACT);
#endif
				break;
			case EBO_REVSUBTRACT:
#if defined(GL_EXT_blend_subtract)
				if (FeatureAvailable[IRR_EXT_blend_subtract] || (Version>=120))
					extGlBlendEquation(GL_FUNC_REVERSE_SUBTRACT_EXT);
#elif defined(GL_VERSION_1_2)
				if (Version>=120)
					extGlBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
#endif
				break;
			case EBO_MIN:
#if defined(GL_EXT_blend_minmax)
				if (FeatureAvailable[IRR_EXT_blend_minmax] || (Version>=120))
					extGlBlendEquation(GL_MIN_EXT);
#elif defined(GL_VERSION_1_2)
				if (Version>=120)
					extGlBlendEquation(GL_MIN);
#endif
				break;
			case EBO_MAX:
#if defined(GL_EXT_blend_minmax)
				if (FeatureAvailable[IRR_EXT_blend_minmax] || (Version>=120))
					extGlBlendEquation(GL_MAX_EXT);
#elif defined(GL_VERSION_1_2)
				if (Version>=120)
					extGlBlendEquation(GL_MAX);
#endif
				break;
			case EBO_MIN_FACTOR:
#if defined(GL_AMD_blend_minmax_factor)
				if (FeatureAvailable[IRR_AMD_blend_minmax_factor])
					extGlBlendEquation(GL_FACTOR_MIN_AMD);
#endif
				// fallback in case of missing extension
#if defined(GL_VERSION_1_2)
#if defined(GL_AMD_blend_minmax_factor)
				else
#endif
				if (Version>=120)
					extGlBlendEquation(GL_MIN);
#endif
				break;
			case EBO_MAX_FACTOR:
#if defined(GL_AMD_blend_minmax_factor)
				if (FeatureAvailable[IRR_AMD_blend_minmax_factor])
					extGlBlendEquation(GL_FACTOR_MAX_AMD);
#endif
				// fallback in case of missing extension
#if defined(GL_VERSION_1_2)
#if defined(GL_AMD_blend_minmax_factor)
				else
#endif
				if (Version>=120)
					extGlBlendEquation(GL_MAX);
#endif
				break;
			case EBO_MIN_ALPHA:
#if defined(GL_SGIX_blend_alpha_minmax)
				if (FeatureAvailable[IRR_SGIX_blend_alpha_minmax])
					extGlBlendEquation(GL_ALPHA_MIN_SGIX);
				// fallback in case of missing extension
				else
					if (FeatureAvailable[IRR_EXT_blend_minmax])
						extGlBlendEquation(GL_MIN_EXT);
#endif
				break;
			case EBO_MAX_ALPHA:
#if defined(GL_SGIX_blend_alpha_minmax)
				if (FeatureAvailable[IRR_SGIX_blend_alpha_minmax])
					extGlBlendEquation(GL_ALPHA_MAX_SGIX);
				// fallback in case of missing extension
				else
					if (FeatureAvailable[IRR_EXT_blend_minmax])
						extGlBlendEquation(GL_MAX_EXT);
#endif
				break;
			default:
#if defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op)
				extGlBlendEquation(GL_FUNC_ADD_EXT);
#elif defined(GL_VERSION_1_2)
				extGlBlendEquation(GL_FUNC_ADD);
#endif
				break;
			}
#endif
		}
	}

	// Polygon Offset
	if (queryFeature(EVDF_POLYGON_OFFSET) && (resetAllRenderStates ||
		lastmaterial.PolygonOffsetDirection != material.PolygonOffsetDirection ||
		lastmaterial.PolygonOffsetFactor != material.PolygonOffsetFactor))
	{
		glDisable(lastmaterial.Wireframe?GL_POLYGON_OFFSET_LINE:lastmaterial.PointCloud?GL_POLYGON_OFFSET_POINT:GL_POLYGON_OFFSET_FILL);
		if (material.PolygonOffsetFactor)
		{
			glDisable(material.Wireframe?GL_POLYGON_OFFSET_LINE:material.PointCloud?GL_POLYGON_OFFSET_POINT:GL_POLYGON_OFFSET_FILL);
			glEnable(material.Wireframe?GL_POLYGON_OFFSET_LINE:material.PointCloud?GL_POLYGON_OFFSET_POINT:GL_POLYGON_OFFSET_FILL);
		}
		if (material.PolygonOffsetDirection==EPO_BACK)
			glPolygonOffset(1.0f, (GLfloat)material.PolygonOffsetFactor);
		else
			glPolygonOffset(-1.0f, (GLfloat)-material.PolygonOffsetFactor);
	}

	// thickness
	if (resetAllRenderStates || lastmaterial.Thickness != material.Thickness)
	{
		if (AntiAlias)
		{
//			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimSmoothedPoint[0], DimSmoothedPoint[1]));
			// we don't use point smoothing
			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedPoint[0], DimAliasedPoint[1]));
			glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimSmoothedLine[0], DimSmoothedLine[1]));
		}
		else
		{
			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedPoint[0], DimAliasedPoint[1]));
			glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedLine[0], DimAliasedLine[1]));
		}
	}

	// Anti aliasing
	if (resetAllRenderStates || lastmaterial.AntiAliasing != material.AntiAliasing)
	{
		if (FeatureAvailable[IRR_ARB_multisample])
		{
			if (material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
			else if (lastmaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

			if ((AntiAlias >= 2) && (material.AntiAliasing & (EAAM_SIMPLE|EAAM_QUALITY)))
			{
				glEnable(GL_MULTISAMPLE_ARB);
#ifdef GL_NV_multisample_filter_hint
				if (FeatureAvailable[IRR_NV_multisample_filter_hint])
				{
					if ((material.AntiAliasing & EAAM_QUALITY) == EAAM_QUALITY)
						glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
					else
						glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
				}
#endif
			}
			else
				glDisable(GL_MULTISAMPLE_ARB);
		}
		if ((material.AntiAliasing & EAAM_LINE_SMOOTH) != (lastmaterial.AntiAliasing & EAAM_LINE_SMOOTH))
		{
			if (material.AntiAliasing & EAAM_LINE_SMOOTH)
				glEnable(GL_LINE_SMOOTH);
			else if (lastmaterial.AntiAliasing & EAAM_LINE_SMOOTH)
				glDisable(GL_LINE_SMOOTH);
		}
		if ((material.AntiAliasing & EAAM_POINT_SMOOTH) != (lastmaterial.AntiAliasing & EAAM_POINT_SMOOTH))
		{
			if (material.AntiAliasing & EAAM_POINT_SMOOTH)
				// often in software, and thus very slow
				glEnable(GL_POINT_SMOOTH);
			else if (lastmaterial.AntiAliasing & EAAM_POINT_SMOOTH)
				glDisable(GL_POINT_SMOOTH);
		}
	}

	setTextureRenderStates(material, resetAllRenderStates);

	// set current fixed pipeline state
	FixedPipelineState = tempState;
}

//! Compare in SMaterial doesn't check texture parameters, so we should call this on each OnRender call.
void COpenGLDriver::setTextureRenderStates(const SMaterial& material, bool resetAllRenderstates)
{
	// Set textures to TU/TIU and apply filters to them

	for (s32 i = MaxTextureUnits-1; i>= 0; --i)
	{
		const COpenGLTexture* tmpTexture = static_cast<const COpenGLTexture*>(CurrentTexture[i]);
		GLenum tmpTextureType = (tmpTexture) ? tmpTexture->getOpenGLTextureType() : GL_TEXTURE_2D;

		if(FixedPipelineState == EOFPS_ENABLE || FixedPipelineState == EOFPS_DISABLE_TO_ENABLE)
		{
			if (i>0 && !MultiTextureExtension)
				break;

			if (!CurrentTexture[i])
			{
				BridgeCalls->setTexture(i, tmpTextureType, true);

				continue;
			}
			else
			{
				BridgeCalls->setTexture(i, tmpTextureType, true);

				setTransform ((E_TRANSFORMATION_STATE) (ETS_TEXTURE_0 + i), material.getTextureMatrix(i));
			}
		}
		else
		{
			if (CurrentTexture[i])
			{
				BridgeCalls->setTexture(i, tmpTextureType, false);
			}
			else
				continue;
		}

		if(resetAllRenderstates)
			tmpTexture->getStatesCache().IsCached = false;

#ifdef GL_VERSION_2_1
		if (Version>=210)
		{
			if(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].LODBias != tmpTexture->getStatesCache().LODBias)
			{
				if (material.TextureLayer[i].LODBias)
				{
					const float tmp = core::clamp(material.TextureLayer[i].LODBias * 0.125f, -MaxTextureLODBias, MaxTextureLODBias);
					glTexParameterf(tmpTextureType, GL_TEXTURE_LOD_BIAS, tmp);
				}
				else
					glTexParameterf(tmpTextureType, GL_TEXTURE_LOD_BIAS, 0.f);

				tmpTexture->getStatesCache().LODBias = material.TextureLayer[i].LODBias;
			}
		}
		else if (FeatureAvailable[IRR_EXT_texture_lod_bias])
		{
			if (material.TextureLayer[i].LODBias)
			{
				const float tmp = core::clamp(material.TextureLayer[i].LODBias * 0.125f, -MaxTextureLODBias, MaxTextureLODBias);
				glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, tmp);
			}
			else
				glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0.f);
		}
#elif defined(GL_EXT_texture_lod_bias)
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

		if(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
			material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter)
		{
			glTexParameteri(tmpTextureType, GL_TEXTURE_MAG_FILTER,
				(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

			tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
			tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
		}

		if (material.UseMipMaps && CurrentTexture[i]->hasMipMaps())
		{
			if(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
				material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter || !tmpTexture->getStatesCache().MipMapStatus)
			{
				glTexParameteri(tmpTextureType, GL_TEXTURE_MIN_FILTER,
					material.TextureLayer[i].TrilinearFilter ? GL_LINEAR_MIPMAP_LINEAR :
					material.TextureLayer[i].BilinearFilter ? GL_LINEAR_MIPMAP_NEAREST :
					GL_NEAREST_MIPMAP_NEAREST);

				tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
				tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
				tmpTexture->getStatesCache().MipMapStatus = true;
			}
		}
		else
		{
			if(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
				material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter || tmpTexture->getStatesCache().MipMapStatus)
			{
				glTexParameteri(tmpTextureType, GL_TEXTURE_MIN_FILTER,
					(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

				tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
				tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
				tmpTexture->getStatesCache().MipMapStatus = false;
			}
		}

#ifdef GL_EXT_texture_filter_anisotropic
		if (FeatureAvailable[IRR_EXT_texture_filter_anisotropic] &&
			(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].AnisotropicFilter != tmpTexture->getStatesCache().AnisotropicFilter))
		{
			glTexParameteri(tmpTextureType, GL_TEXTURE_MAX_ANISOTROPY_EXT,
				material.TextureLayer[i].AnisotropicFilter>1 ? core::min_(MaxAnisotropy, material.TextureLayer[i].AnisotropicFilter) : 1);

			tmpTexture->getStatesCache().AnisotropicFilter = material.TextureLayer[i].AnisotropicFilter;
		}
#endif

		if(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].TextureWrapU != tmpTexture->getStatesCache().WrapU)
		{
			glTexParameteri(tmpTextureType, GL_TEXTURE_WRAP_S, getTextureWrapMode(material.TextureLayer[i].TextureWrapU));
			tmpTexture->getStatesCache().WrapU = material.TextureLayer[i].TextureWrapU;
		}

		if(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].TextureWrapV != tmpTexture->getStatesCache().WrapV)
		{
			glTexParameteri(tmpTextureType, GL_TEXTURE_WRAP_T, getTextureWrapMode(material.TextureLayer[i].TextureWrapV));
			tmpTexture->getStatesCache().WrapV = material.TextureLayer[i].TextureWrapV;
		}

		tmpTexture->getStatesCache().IsCached = true;
	}

	// be sure to leave in texture stage 0
	BridgeCalls->setActiveTexture(GL_TEXTURE0_ARB);
}


//! Enable the 2d override material
void COpenGLDriver::enableMaterial2D(bool enable)
{
	if (!enable)
		CurrentRenderMode = ERM_NONE;
	CNullDriver::enableMaterial2D(enable);
}


//! sets the needed renderstates
void COpenGLDriver::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
	// 2d methods uses fixed pipeline
	if (FixedPipelineState == COpenGLDriver::EOFPS_DISABLE)
		FixedPipelineState = COpenGLDriver::EOFPS_DISABLE_TO_ENABLE;
	else
		FixedPipelineState = COpenGLDriver::EOFPS_ENABLE;

	if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
	{
		// unset last 3d material
		if (CurrentRenderMode == ERM_3D)
		{
			if (static_cast<u32>(LastMaterial.MaterialType) < MaterialRenderers.size())
				MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();
		}
		if (Transformation3DChanged)
		{
			BridgeCalls->setMatrixMode(GL_PROJECTION);

			const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
			core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
			m.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0f, 1.0f);
			m.setTranslation(core::vector3df(-1,1,0));
			glLoadMatrixf(m.pointer());

			BridgeCalls->setMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(0.375f, 0.375f, 0.0f);

			// Make sure we set first texture matrix
			BridgeCalls->setActiveTexture(GL_TEXTURE0_ARB);

			Transformation3DChanged = false;
		}
		if (!OverrideMaterial2DEnabled)
		{
			setBasicRenderStates(InitMaterial2D, LastMaterial, true);
			LastMaterial = InitMaterial2D;
		}
		BridgeCalls->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef GL_EXT_clip_volume_hint
		if (FeatureAvailable[IRR_EXT_clip_volume_hint])
			glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_FASTEST);
#endif

	}
	if (OverrideMaterial2DEnabled)
	{
		OverrideMaterial2D.Lighting=false;
		setBasicRenderStates(OverrideMaterial2D, LastMaterial, false);
		LastMaterial = OverrideMaterial2D;
	}

	// no alphaChannel without texture
	alphaChannel &= texture;

	if (alphaChannel || alpha)
	{
		BridgeCalls->setBlend(true);
		BridgeCalls->setAlphaTest(true);
		BridgeCalls->setAlphaFunc(GL_GREATER, 0.f);
	}
	else
	{
		BridgeCalls->setBlend(false);
		BridgeCalls->setAlphaTest(false);
	}

	if (texture)
	{
		if (OverrideMaterial2DEnabled)
			setTextureRenderStates(OverrideMaterial2D, false);
		else
			setTextureRenderStates(InitMaterial2D, false);

		Material.setTexture(0, const_cast<video::ITexture*>(CurrentTexture[0]));
		setTransform(ETS_TEXTURE_0, core::IdentityMatrix);
		// Due to the transformation change, the previous line would call a reset each frame
		// but we can safely reset the variable as it was false before
		Transformation3DChanged=false;

		if (alphaChannel)
		{
			// if alpha and alpha texture just modulate, otherwise use only the alpha channel
			if (alpha)
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			else
			{
#if defined(GL_ARB_texture_env_combine) || defined(GL_EXT_texture_env_combine)
				if (FeatureAvailable[IRR_ARB_texture_env_combine]||FeatureAvailable[IRR_EXT_texture_env_combine])
				{
#ifdef GL_ARB_texture_env_combine
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
					// rgb always modulates
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
#else
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
					// rgb always modulates
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT);
#endif
				}
				else
#endif
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
		}
		else
		{
			if (alpha)
			{
#if defined(GL_ARB_texture_env_combine) || defined(GL_EXT_texture_env_combine)
				if (FeatureAvailable[IRR_ARB_texture_env_combine]||FeatureAvailable[IRR_EXT_texture_env_combine])
				{
#ifdef GL_ARB_texture_env_combine
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
					// rgb always modulates
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
#else
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PRIMARY_COLOR_EXT);
					// rgb always modulates
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT);
#endif
				}
				else
#endif
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			else
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
		}
	}

	CurrentRenderMode = ERM_2D;
}


//! \return Returns the name of the video driver.
const wchar_t* COpenGLDriver::getName() const
{
	return Name.c_str();
}


//! deletes all dynamic lights there are
void COpenGLDriver::deleteAllDynamicLights()
{
	for (s32 i=0; i<MaxLights; ++i)
		glDisable(GL_LIGHT0 + i);

	RequestedLights.clear();

	CNullDriver::deleteAllDynamicLights();
}


//! adds a dynamic light
s32 COpenGLDriver::addDynamicLight(const SLight& light)
{
	CNullDriver::addDynamicLight(light);

	RequestedLights.push_back(RequestedLight(light));

	u32 newLightIndex = RequestedLights.size() - 1;

	// Try and assign a hardware light just now, but don't worry if I can't
	assignHardwareLight(newLightIndex);

	return (s32)newLightIndex;
}


void COpenGLDriver::assignHardwareLight(u32 lightIndex)
{
	setTransform(ETS_WORLD, core::matrix4());

	s32 lidx;
	for (lidx=GL_LIGHT0; lidx < GL_LIGHT0 + MaxLights; ++lidx)
	{
		if(!glIsEnabled(lidx))
		{
			RequestedLights[lightIndex].HardwareLightIndex = lidx;
			break;
		}
	}

	if(lidx == GL_LIGHT0 + MaxLights) // There's no room for it just now
		return;

	GLfloat data[4];
	const SLight & light = RequestedLights[lightIndex].LightData;

	switch (light.Type)
	{
	case video::ELT_SPOT:
		data[0] = light.Direction.X;
		data[1] = light.Direction.Y;
		data[2] = light.Direction.Z;
		data[3] = 0.0f;
		glLightfv(lidx, GL_SPOT_DIRECTION, data);

		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f; // 1.0f for positional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, light.Falloff);
		glLightf(lidx, GL_SPOT_CUTOFF, light.OuterCone);
	break;
	case video::ELT_POINT:
		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f; // 1.0f for positional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
	break;
	case video::ELT_DIRECTIONAL:
		// set direction
		data[0] = -light.Direction.X;
		data[1] = -light.Direction.Y;
		data[2] = -light.Direction.Z;
		data[3] = 0.0f; // 0.0f for directional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
	break;
	default:
	break;
	}

	// set diffuse color
	data[0] = light.DiffuseColor.r;
	data[1] = light.DiffuseColor.g;
	data[2] = light.DiffuseColor.b;
	data[3] = light.DiffuseColor.a;
	glLightfv(lidx, GL_DIFFUSE, data);

	// set specular color
	data[0] = light.SpecularColor.r;
	data[1] = light.SpecularColor.g;
	data[2] = light.SpecularColor.b;
	data[3] = light.SpecularColor.a;
	glLightfv(lidx, GL_SPECULAR, data);

	// set ambient color
	data[0] = light.AmbientColor.r;
	data[1] = light.AmbientColor.g;
	data[2] = light.AmbientColor.b;
	data[3] = light.AmbientColor.a;
	glLightfv(lidx, GL_AMBIENT, data);

	// 1.0f / (constant + linear * d + quadratic*(d*d);

	// set attenuation
	glLightf(lidx, GL_CONSTANT_ATTENUATION, light.Attenuation.X);
	glLightf(lidx, GL_LINEAR_ATTENUATION, light.Attenuation.Y);
	glLightf(lidx, GL_QUADRATIC_ATTENUATION, light.Attenuation.Z);

	glEnable(lidx);
}


//! Turns a dynamic light on or off
//! \param lightIndex: the index returned by addDynamicLight
//! \param turnOn: true to turn the light on, false to turn it off
void COpenGLDriver::turnLightOn(s32 lightIndex, bool turnOn)
{
	if(lightIndex < 0 || lightIndex >= (s32)RequestedLights.size())
		return;

	RequestedLight & requestedLight = RequestedLights[lightIndex];

	requestedLight.DesireToBeOn = turnOn;

	if(turnOn)
	{
		if(-1 == requestedLight.HardwareLightIndex)
			assignHardwareLight(lightIndex);
	}
	else
	{
		if(-1 != requestedLight.HardwareLightIndex)
		{
			// It's currently assigned, so free up the hardware light
			glDisable(requestedLight.HardwareLightIndex);
			requestedLight.HardwareLightIndex = -1;

			// Now let the first light that's waiting on a free hardware light grab it
			for(u32 requested = 0; requested < RequestedLights.size(); ++requested)
				if(RequestedLights[requested].DesireToBeOn
					&&
					-1 == RequestedLights[requested].HardwareLightIndex)
				{
					assignHardwareLight(requested);
					break;
				}
		}
	}
}


//! returns the maximal amount of dynamic lights the device can handle
u32 COpenGLDriver::getMaximalDynamicLightAmount() const
{
	return MaxLights;
}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void COpenGLDriver::setAmbientLight(const SColorf& color)
{
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}


// this code was sent in by Oliver Klems, thank you! (I modified the glViewport
// method just a bit.
void COpenGLDriver::setViewPort(const core::rect<s32>& area)
{
	if (area == ViewPort)
		return;
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0,0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
	vp.clipAgainst(rendert);

	if (vp.getHeight()>0 && vp.getWidth()>0)
	{
		glViewport(vp.UpperLeftCorner.X,
				getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight(),
				vp.getWidth(), vp.getHeight());

		ViewPort = vp;
	}
}


//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: First, draw all geometry. Then use this method, to draw the shadow
//! volume. Next use IVideoDriver::drawStencilShadow() to visualize the shadow.
void COpenGLDriver::drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible)
{
	const u32 count=triangles.size();
	if (!StencilBuffer || !count)
		return;

	// unset last 3d material
	if (CurrentRenderMode == ERM_3D &&
		static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
	{
		MaterialRenderers[Material.MaterialType].Renderer->OnUnsetMaterial();
		ResetRenderStates = true;
	}

	// store current OpenGL state
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT |
		GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_FALSE);

	if (debugDataVisible & scene::EDS_MESH_WIRE_OVERLAY)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (!(debugDataVisible & (scene::EDS_SKELETON|scene::EDS_MESH_WIRE_OVERLAY)))
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // no color buffer drawing
		glEnable(GL_STENCIL_TEST);
	}

	BridgeCalls->setClientState(true, false, false, false);
	glVertexPointer(3,GL_FLOAT,sizeof(core::vector3df),triangles.const_pointer());
	glStencilMask(~0);
	glStencilFunc(GL_ALWAYS, 0, ~0);

	GLenum incr = GL_INCR;
	GLenum decr = GL_DECR;
#ifdef GL_EXT_stencil_wrap
	if (FeatureAvailable[IRR_EXT_stencil_wrap])
	{
		incr = GL_INCR_WRAP_EXT;
		decr = GL_DECR_WRAP_EXT;
	}
#endif
#ifdef GL_NV_depth_clamp
	if (FeatureAvailable[IRR_NV_depth_clamp])
		glEnable(GL_DEPTH_CLAMP_NV);
#endif

	// The first parts are not correctly working, yet.
#if 0
#ifdef GL_EXT_stencil_two_side
	if (FeatureAvailable[IRR_EXT_stencil_two_side])
	{
		glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
		glDisable(GL_CULL_FACE);
		if (zfail)
		{
			extGlActiveStencilFace(GL_BACK);
			glStencilOp(GL_KEEP, incr, GL_KEEP);
			glStencilMask(~0);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			extGlActiveStencilFace(GL_FRONT);
			glStencilOp(GL_KEEP, decr, GL_KEEP);
		}
		else // zpass
		{
			extGlActiveStencilFace(GL_BACK);
			glStencilOp(GL_KEEP, GL_KEEP, decr);
			glStencilMask(~0);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			extGlActiveStencilFace(GL_FRONT);
			glStencilOp(GL_KEEP, GL_KEEP, incr);
		}
		glStencilMask(~0);
		glStencilFunc(GL_ALWAYS, 0, ~0);
		glDrawArrays(GL_TRIANGLES,0,count);
		glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
	}
	else
#endif
	if (FeatureAvailable[IRR_ATI_separate_stencil])
	{
		glDisable(GL_CULL_FACE);
		if (zfail)
		{
			extGlStencilOpSeparate(GL_BACK, GL_KEEP, incr, GL_KEEP);
			extGlStencilOpSeparate(GL_FRONT, GL_KEEP, decr, GL_KEEP);
		}
		else // zpass
		{
			extGlStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, decr);
			extGlStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, incr);
		}
		extGlStencilFuncSeparate(GL_ALWAYS, GL_ALWAYS, 0, ~0);
		glStencilMask(~0);
		glDrawArrays(GL_TRIANGLES,0,count);
	}
	else
#endif
	{
		glEnable(GL_CULL_FACE);
		if (zfail)
		{
			glCullFace(GL_FRONT);
			glStencilOp(GL_KEEP, incr, GL_KEEP);
			glDrawArrays(GL_TRIANGLES,0,count);

			glCullFace(GL_BACK);
			glStencilOp(GL_KEEP, decr, GL_KEEP);
			glDrawArrays(GL_TRIANGLES,0,count);
		}
		else // zpass
		{
			glCullFace(GL_BACK);
			glStencilOp(GL_KEEP, GL_KEEP, incr);
			glDrawArrays(GL_TRIANGLES,0,count);

			glCullFace(GL_FRONT);
			glStencilOp(GL_KEEP, GL_KEEP, decr);
			glDrawArrays(GL_TRIANGLES,0,count);
		}
	}
#ifdef GL_NV_depth_clamp
	if (FeatureAvailable[IRR_NV_depth_clamp])
		glDisable(GL_DEPTH_CLAMP_NV);
#endif

	glDisable(GL_POLYGON_OFFSET_FILL);
	glPopAttrib();
}

//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow.
void COpenGLDriver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
	video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	disableTextures();

	// store attributes
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT | GL_LIGHTING_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);

	glShadeModel(GL_FLAT);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// draw a shadow rectangle covering the entire screen using stencil buffer
	BridgeCalls->setMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	BridgeCalls->setMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	Quad2DVertices[0].Color = leftDownEdge;
	Quad2DVertices[1].Color = leftUpEdge;
	Quad2DVertices[2].Color = rightUpEdge;
	Quad2DVertices[3].Color = rightDownEdge;

	Quad2DVertices[0].Pos = core::vector3df(-1.0f, -1.0f, -0.9f);
	Quad2DVertices[1].Pos = core::vector3df(-1.0f, 1.0f, -0.9f);
	Quad2DVertices[2].Pos = core::vector3df(1.0f, 1.0f, -0.9f);
	Quad2DVertices[3].Pos = core::vector3df(1.0f, -1.0f, -0.9f);

	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(Quad2DVertices, 4, EVT_STANDARD);

	BridgeCalls->setClientState(true, false, true, false);

	glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
	else
	{
		_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	}

	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, Quad2DIndices);

	clearBuffers(false, false, clearStencilBuffer, 0x0);

	// restore settings
	glPopMatrix();
	BridgeCalls->setMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}


//! Sets the fog mode.
void COpenGLDriver::setFog(SColor c, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CNullDriver::setFog(c, fogType, start, end, density, pixelFog, rangeFog);

	glFogf(GL_FOG_MODE, GLfloat((fogType==EFT_FOG_LINEAR)? GL_LINEAR : (fogType==EFT_FOG_EXP)?GL_EXP:GL_EXP2));

#ifdef GL_EXT_fog_coord
	if (FeatureAvailable[IRR_EXT_fog_coord])
		glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);
#endif
#ifdef GL_NV_fog_distance
	if (FeatureAvailable[IRR_NV_fog_distance])
	{
		if (rangeFog)
			glFogi(GL_FOG_DISTANCE_MODE_NV, GL_EYE_RADIAL_NV);
		else
			glFogi(GL_FOG_DISTANCE_MODE_NV, GL_EYE_PLANE_ABSOLUTE_NV);
	}
#endif

	if (fogType==EFT_FOG_LINEAR)
	{
		glFogf(GL_FOG_START, start);
		glFogf(GL_FOG_END, end);
	}
	else
		glFogf(GL_FOG_DENSITY, density);

	if (pixelFog)
		glHint(GL_FOG_HINT, GL_NICEST);
	else
		glHint(GL_FOG_HINT, GL_FASTEST);

	SColorf color(c);
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glFogfv(GL_FOG_COLOR, data);
}


//! Draws a 3d line.
void COpenGLDriver::draw3DLine(const core::vector3df& start,
				const core::vector3df& end, SColor color)
{
	setRenderStates3DMode();

	Quad2DVertices[0].Color = color;
	Quad2DVertices[1].Color = color;

	Quad2DVertices[0].Pos = core::vector3df((f32)start.X, (f32)start.Y, (f32)start.Z);
	Quad2DVertices[1].Pos = core::vector3df((f32)end.X, (f32)end.Y, (f32)end.Z);

	if (!FeatureAvailable[IRR_ARB_vertex_array_bgra] && !FeatureAvailable[IRR_EXT_vertex_array_bgra])
		getColorBuffer(Quad2DVertices, 2, EVT_STANDARD);

	BridgeCalls->setClientState(true, false, true, false);

	glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Pos);

#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif
	if (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(Quad2DVertices))[0].Color);
	else
	{
		_IRR_DEBUG_BREAK_IF(ColorBuffer.size()==0);
		glColorPointer(colorSize, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	}

	glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, Quad2DIndices);
}


//! Removes a texture from the texture cache and deletes it, freeing lot of memory.
void COpenGLDriver::removeTexture(ITexture* texture)
{
	if (!texture)
		return;

	CNullDriver::removeTexture(texture);

	// Remove this texture from CurrentTexture as well
	CurrentTexture.remove(texture);
}


//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void COpenGLDriver::OnResize(const core::dimension2d<u32>& size)
{
	CNullDriver::OnResize(size);
	glViewport(0, 0, size.Width, size.Height);
	Transformation3DChanged = true;
}


//! Returns type of video driver
E_DRIVER_TYPE COpenGLDriver::getDriverType() const
{
	return EDT_OPENGL;
}


//! returns color format
ECOLOR_FORMAT COpenGLDriver::getColorFormat() const
{
	return ColorFormat;
}


//! Get a vertex shader constant index.
s32 COpenGLDriver::getVertexShaderConstantID(const c8* name)
{
	return getPixelShaderConstantID(name);
}

//! Get a pixel shader constant index.
s32 COpenGLDriver::getPixelShaderConstantID(const c8* name)
{
	os::Printer::log("Error: Please call services->getPixelShaderConstantID(), not VideoDriver->getPixelShaderConstantID().");
	return -1;
}

//! Sets a vertex shader constant.
void COpenGLDriver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
#ifdef GL_ARB_vertex_program
	for (s32 i=0; i<constantAmount; ++i)
		extGlProgramLocalParameter4fv(GL_VERTEX_PROGRAM_ARB, startRegister+i, &data[i*4]);
#endif
}

//! Sets a pixel shader constant.
void COpenGLDriver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
#ifdef GL_ARB_fragment_program
	for (s32 i=0; i<constantAmount; ++i)
		extGlProgramLocalParameter4fv(GL_FRAGMENT_PROGRAM_ARB, startRegister+i, &data[i*4]);
#endif
}

//! Sets a constant for the vertex shader based on an index.
bool COpenGLDriver::setVertexShaderConstant(s32 index, const f32* floats, int count)
{
	//pass this along, as in GLSL the same routine is used for both vertex and fragment shaders
	return setPixelShaderConstant(index, floats, count);
}

//! Int interface for the above.
bool COpenGLDriver::setVertexShaderConstant(s32 index, const s32* ints, int count)
{
	return setPixelShaderConstant(index, ints, count);
}

//! Sets a constant for the pixel shader based on an index.
bool COpenGLDriver::setPixelShaderConstant(s32 index, const f32* floats, int count)
{
	os::Printer::log("Error: Please call services->setPixelShaderConstant(), not VideoDriver->setPixelShaderConstant().");
	return false;
}

//! Int interface for the above.
bool COpenGLDriver::setPixelShaderConstant(s32 index, const s32* ints, int count)
{
	os::Printer::log("Error: Please call services->setPixelShaderConstant(), not VideoDriver->setPixelShaderConstant().");
	return false;
}


//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32 COpenGLDriver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial, s32 userData)
{
	s32 nr = -1;
	COpenGLShaderMaterialRenderer* r = new COpenGLShaderMaterialRenderer(
		this, nr, vertexShaderProgram, pixelShaderProgram,
		callback, baseMaterial, userData);

	r->drop();
	return nr;
}


//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
s32 COpenGLDriver::addHighLevelShaderMaterial(
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
	s32 userData, E_GPU_SHADING_LANGUAGE shadingLang)
{
	s32 nr = -1;

	#ifdef _IRR_COMPILE_WITH_CG_
	if (shadingLang == EGSL_CG)
	{
		COpenGLCgMaterialRenderer* r = new COpenGLCgMaterialRenderer(
			this, nr,
			vertexShaderProgram, vertexShaderEntryPointName, vsCompileTarget,
			pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
			geometryShaderProgram, geometryShaderEntryPointName, gsCompileTarget,
			inType, outType, verticesOut,
			callback,baseMaterial, userData);

		r->drop();
	}
	else
	#endif
	{
		COpenGLSLMaterialRenderer* r = new COpenGLSLMaterialRenderer(
			this, nr,
			vertexShaderProgram, vertexShaderEntryPointName, vsCompileTarget,
			pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
			geometryShaderProgram, geometryShaderEntryPointName, gsCompileTarget,
			inType, outType, verticesOut,
			callback,baseMaterial, userData);

		r->drop();
	}

	return nr;
}


//! Returns a pointer to the IVideoDriver interface. (Implementation for
//! IMaterialRendererServices)
IVideoDriver* COpenGLDriver::getVideoDriver()
{
	return this;
}


ITexture* COpenGLDriver::addRenderTargetTexture(const core::dimension2d<u32>& size,
					const io::path& name,
					const ECOLOR_FORMAT format)
{
	//disable mip-mapping
	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	video::ITexture* rtt = 0;
#if defined(GL_EXT_framebuffer_object)
	// if driver supports FrameBufferObjects, use them
	if (queryFeature(EVDF_FRAMEBUFFER_OBJECT))
	{
		rtt = new COpenGLFBOTexture(size, name, this, format);
		if (rtt)
		{
			bool success = false;
			addTexture(rtt);
			ITexture* tex = createDepthTexture(rtt);
			if (tex)
			{
				success = static_cast<video::COpenGLFBODepthTexture*>(tex)->attach(rtt);
				if ( !success )
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
#endif
	{
		// the simple texture is only possible for size <= screensize
		// we try to find an optimal size with the original constraints
		core::dimension2du destSize(core::min_(size.Width,ScreenSize.Width), core::min_(size.Height,ScreenSize.Height));
		destSize = destSize.getOptimalSize((size==size.getOptimalSize()), false, false);
		rtt = addTexture(destSize, name, ECF_A8R8G8B8);
		if (rtt)
		{
			static_cast<video::COpenGLTexture*>(rtt)->setIsRenderTarget(true);
		}
	}

	//restore mip-mapping
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, generateMipLevels);

	return rtt;
}


//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32 COpenGLDriver::getMaximalPrimitiveCount() const
{
	return 0x7fffffff;
}


//! set or reset render target
bool COpenGLDriver::setRenderTarget(video::E_RENDER_TARGET target, bool clearTarget,
					bool clearZBuffer, SColor color)
{
	if (target != CurrentTarget)
		setRenderTarget(0, false, false, 0x0);

	if (ERT_RENDER_TEXTURE == target)
	{
		os::Printer::log("For render textures call setRenderTarget with the actual texture as first parameter.", ELL_ERROR);
		return false;
	}
	if (ERT_MULTI_RENDER_TEXTURES == target)
	{
		os::Printer::log("For multiple render textures call setRenderTarget with the texture array as first parameter.", ELL_ERROR);
		return false;
	}

	if (Params.Stereobuffer && (ERT_STEREO_RIGHT_BUFFER == target))
	{
		if (Params.Doublebuffer)
			glDrawBuffer(GL_BACK_RIGHT);
		else
			glDrawBuffer(GL_FRONT_RIGHT);
	}
	else if (Params.Stereobuffer && ERT_STEREO_BOTH_BUFFERS == target)
	{
		if (Params.Doublebuffer)
			glDrawBuffer(GL_BACK);
		else
			glDrawBuffer(GL_FRONT);
	}
	else if ((target >= ERT_AUX_BUFFER0) && (target-ERT_AUX_BUFFER0 < MaxAuxBuffers))
	{
			glDrawBuffer(GL_AUX0+target-ERT_AUX_BUFFER0);
	}
	else
	{
		if (Params.Doublebuffer)
			glDrawBuffer(GL_BACK_LEFT);
		else
			glDrawBuffer(GL_FRONT_LEFT);
		// exit with false, but also with working color buffer
		if (target != ERT_FRAME_BUFFER)
			return false;
	}
	CurrentTarget=target;
	clearBuffers(clearTarget, clearZBuffer, false, color);
	return true;
}


//! set or reset render target
bool COpenGLDriver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
					bool clearZBuffer, SColor color)
{
	// check for right driver type

	if (texture && texture->getDriverType() != EDT_OPENGL)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

#if defined(GL_EXT_framebuffer_object)
	if (CurrentTarget==ERT_MULTI_RENDER_TEXTURES)
	{
		for (u32 i=0; i<MRTargets.size(); ++i)
		{
			if (MRTargets[i].TargetType==ERT_RENDER_TEXTURE)
			{
				for (++i; i<MRTargets.size(); ++i)
					if (MRTargets[i].TargetType==ERT_RENDER_TEXTURE)
						extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, GL_TEXTURE_2D, 0, 0);
			}
		}
		MRTargets.clear();
	}
#endif

	// check if we should set the previous RT back
	if ((RenderTargetTexture != texture) ||
		(CurrentTarget==ERT_MULTI_RENDER_TEXTURES))
	{
		BridgeCalls->setActiveTexture(GL_TEXTURE0_ARB);
		ResetRenderStates=true;
		if (RenderTargetTexture!=0)
		{
			RenderTargetTexture->unbindRTT();
		}

		if (texture)
		{
			// we want to set a new target. so do this.
			glViewport(0, 0, texture->getSize().Width, texture->getSize().Height);
			RenderTargetTexture = static_cast<COpenGLTexture*>(texture);
			// calls glDrawBuffer as well
			RenderTargetTexture->bindRTT();
			CurrentRendertargetSize = texture->getSize();
			CurrentTarget=ERT_RENDER_TEXTURE;
		}
		else
		{
			glViewport(0,0,ScreenSize.Width,ScreenSize.Height);
			RenderTargetTexture = 0;
			CurrentRendertargetSize = core::dimension2d<u32>(0,0);
			CurrentTarget=ERT_FRAME_BUFFER;
			glDrawBuffer(Params.Doublebuffer?GL_BACK_LEFT:GL_FRONT_LEFT);
		}
		// we need to update the matrices due to the rendersize change.
		Transformation3DChanged=true;
	}

	clearBuffers(clearBackBuffer, clearZBuffer, false, color);

	return true;
}


//! Sets multiple render targets
bool COpenGLDriver::setRenderTarget(const core::array<video::IRenderTarget>& targets,
				bool clearBackBuffer, bool clearZBuffer, SColor color)
{
	// if simply disabling the MRT via array call
	if (targets.size()==0)
		return setRenderTarget(0, clearBackBuffer, clearZBuffer, color);
	// if disabling old MRT, but enabling new one as well
	if ((MRTargets.size()!=0) && (targets != MRTargets))
		setRenderTarget(0, clearBackBuffer, clearZBuffer, color);
	// if no change, simply clear buffers
	else if (targets == MRTargets)
	{
		clearBuffers(clearBackBuffer, clearZBuffer, false, color);
		return true;
	}

	// copy to storage for correct disabling
	MRTargets=targets;

	u32 maxMultipleRTTs = core::min_(static_cast<u32>(MaxMultipleRenderTargets), targets.size());

	// determine common size
	core::dimension2du rttSize = CurrentRendertargetSize;
	if (targets[0].TargetType==ERT_RENDER_TEXTURE)
	{
		if (!targets[0].RenderTexture)
		{
			os::Printer::log("Missing render texture for MRT.", ELL_ERROR);
			return false;
		}
		rttSize=targets[0].RenderTexture->getSize();
	}

	for (u32 i = 0; i < maxMultipleRTTs; ++i)
	{
		// check for right driver type
		if (targets[i].TargetType==ERT_RENDER_TEXTURE)
		{
			if (!targets[i].RenderTexture)
			{
				maxMultipleRTTs=i;
				os::Printer::log("Missing render texture for MRT.", ELL_WARNING);
				break;
			}
			if (targets[i].RenderTexture->getDriverType() != EDT_OPENGL)
			{
				maxMultipleRTTs=i;
				os::Printer::log("Tried to set a texture not owned by this driver.", ELL_WARNING);
				break;
			}

			// check for valid render target
			if (!targets[i].RenderTexture->isRenderTarget() || !static_cast<COpenGLTexture*>(targets[i].RenderTexture)->isFrameBufferObject())
			{
				maxMultipleRTTs=i;
				os::Printer::log("Tried to set a non FBO-RTT as render target.", ELL_WARNING);
				break;
			}

			// check for valid size
			if (rttSize != targets[i].RenderTexture->getSize())
			{
				maxMultipleRTTs=i;
				os::Printer::log("Render target texture has wrong size.", ELL_WARNING);
				break;
			}
		}
	}
	if (maxMultipleRTTs==0)
	{
		os::Printer::log("No valid MRTs.", ELL_ERROR);
		return false;
	}

	// init FBO, if any
	for (u32 i=0; i<maxMultipleRTTs; ++i)
	{
		if (targets[i].TargetType==ERT_RENDER_TEXTURE)
		{
			setRenderTarget(targets[i].RenderTexture, false, false, 0x0);
			break; // bind only first RTT
		}
	}
	// init other main buffer, if necessary
	if (targets[0].TargetType!=ERT_RENDER_TEXTURE)
		setRenderTarget(targets[0].TargetType, false, false, 0x0);

	// attach other textures and store buffers into array
	if (maxMultipleRTTs > 1)
	{
		CurrentTarget=ERT_MULTI_RENDER_TEXTURES;
		core::array<GLenum> MRTs;
		MRTs.set_used(maxMultipleRTTs);
		for(u32 i = 0; i < maxMultipleRTTs; i++)
		{
			if (FeatureAvailable[IRR_EXT_draw_buffers2])
			{
				extGlColorMaskIndexed(i,
					(targets[i].ColorMask & ECP_RED)?GL_TRUE:GL_FALSE,
					(targets[i].ColorMask & ECP_GREEN)?GL_TRUE:GL_FALSE,
					(targets[i].ColorMask & ECP_BLUE)?GL_TRUE:GL_FALSE,
					(targets[i].ColorMask & ECP_ALPHA)?GL_TRUE:GL_FALSE);
				if (targets[i].BlendOp==EBO_NONE)
					extGlDisableIndexed(GL_BLEND, i);
				else
					extGlEnableIndexed(GL_BLEND, i);
			}
#if defined(GL_AMD_draw_buffers_blend) || defined(GL_ARB_draw_buffers_blend)
			if (FeatureAvailable[IRR_AMD_draw_buffers_blend] || FeatureAvailable[IRR_ARB_draw_buffers_blend])
			{
				extGlBlendFuncIndexed(i, getGLBlend(targets[i].BlendFuncSrc), getGLBlend(targets[i].BlendFuncDst));
				switch(targets[i].BlendOp)
				{
				case EBO_SUBTRACT:
					extGlBlendEquationIndexed(i, GL_FUNC_SUBTRACT);
					break;
				case EBO_REVSUBTRACT:
					extGlBlendEquationIndexed(i, GL_FUNC_REVERSE_SUBTRACT);
					break;
				case EBO_MIN:
					extGlBlendEquationIndexed(i, GL_MIN);
					break;
				case EBO_MAX:
					extGlBlendEquationIndexed(i, GL_MAX);
					break;
				case EBO_MIN_FACTOR:
				case EBO_MIN_ALPHA:
#if defined(GL_AMD_blend_minmax_factor)
					if (FeatureAvailable[IRR_AMD_blend_minmax_factor])
						extGlBlendEquationIndexed(i, GL_FACTOR_MIN_AMD);
					// fallback in case of missing extension
					else
#endif
						extGlBlendEquation(GL_MIN);
					break;
				case EBO_MAX_FACTOR:
				case EBO_MAX_ALPHA:
#if defined(GL_AMD_blend_minmax_factor)
					if (FeatureAvailable[IRR_AMD_blend_minmax_factor])
						extGlBlendEquationIndexed(i, GL_FACTOR_MAX_AMD);
					// fallback in case of missing extension
					else
#endif
						extGlBlendEquation(GL_MAX);
				break;
				default:
					extGlBlendEquationIndexed(i, GL_FUNC_ADD);
					break;
				}
			}
#endif
			if (targets[i].TargetType==ERT_RENDER_TEXTURE)
			{
				GLenum attachment = GL_NONE;
#ifdef GL_EXT_framebuffer_object
				// attach texture to FrameBuffer Object on Color [i]
				attachment = GL_COLOR_ATTACHMENT0_EXT+i;
				if ((i != 0) && (targets[i].RenderTexture != RenderTargetTexture))
					extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_2D, static_cast<COpenGLTexture*>(targets[i].RenderTexture)->getOpenGLTextureName(), 0);
#endif
				MRTs[i]=attachment;
			}
			else
			{
				switch(targets[i].TargetType)
				{
					case ERT_FRAME_BUFFER:
						MRTs[i]=GL_BACK_LEFT;
						break;
					case ERT_STEREO_BOTH_BUFFERS:
						MRTs[i]=GL_BACK;
						break;
					case ERT_STEREO_RIGHT_BUFFER:
						MRTs[i]=GL_BACK_RIGHT;
						break;
					case ERT_STEREO_LEFT_BUFFER:
						MRTs[i]=GL_BACK_LEFT;
						break;
					default:
						MRTs[i]=GL_AUX0+(targets[i].TargetType-ERT_AUX_BUFFER0);
						break;
				}
			}
		}

		extGlDrawBuffers(maxMultipleRTTs, MRTs.const_pointer());
	}

	clearBuffers(clearBackBuffer, clearZBuffer, false, color);
	return true;
}


// returns the current size of the screen or rendertarget
const core::dimension2d<u32>& COpenGLDriver::getCurrentRenderTargetSize() const
{
	if (CurrentRendertargetSize.Width == 0)
		return ScreenSize;
	else
		return CurrentRendertargetSize;
}


//! Clears the ZBuffer.
void COpenGLDriver::clearZBuffer()
{
	clearBuffers(false, true, false, 0x0);
}


//! Returns an image created from the last rendered frame.
IImage* COpenGLDriver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
{
	if (target==video::ERT_MULTI_RENDER_TEXTURES || target==video::ERT_RENDER_TEXTURE || target==video::ERT_STEREO_BOTH_BUFFERS)
		return 0;

	// allows to read pixels in top-to-bottom order
#ifdef GL_MESA_pack_invert
	if (FeatureAvailable[IRR_MESA_pack_invert])
		glPixelStorei(GL_PACK_INVERT_MESA, GL_TRUE);
#endif

	if (format==video::ECF_UNKNOWN)
		format=getColorFormat();
	GLenum fmt;
	GLenum type;
	switch (format)
	{
	case ECF_A1R5G5B5:
		fmt = GL_BGRA;
		type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
	case ECF_R5G6B5:
		fmt = GL_RGB;
		type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case ECF_R8G8B8:
		fmt = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		break;
	case ECF_A8R8G8B8:
		fmt = GL_BGRA;
		if (Version > 101)
			type = GL_UNSIGNED_INT_8_8_8_8_REV;
		else
			type = GL_UNSIGNED_BYTE;
		break;
	case ECF_R16F:
		if (FeatureAvailable[IRR_ARB_texture_rg])
			fmt = GL_RED;
		else
			fmt = GL_LUMINANCE;
#ifdef GL_ARB_half_float_pixel
		if (FeatureAvailable[IRR_ARB_half_float_pixel])
			type = GL_HALF_FLOAT_ARB;
		else
#endif
		{
			type = GL_FLOAT;
			format = ECF_R32F;
		}
		break;
	case ECF_G16R16F:
#ifdef GL_ARB_texture_rg
		if (FeatureAvailable[IRR_ARB_texture_rg])
			fmt = GL_RG;
		else
#endif
			fmt = GL_LUMINANCE_ALPHA;
#ifdef GL_ARB_half_float_pixel
		if (FeatureAvailable[IRR_ARB_half_float_pixel])
			type = GL_HALF_FLOAT_ARB;
		else
#endif
		{
			type = GL_FLOAT;
			format = ECF_G32R32F;
		}
		break;
	case ECF_A16B16G16R16F:
		fmt = GL_BGRA;
#ifdef GL_ARB_half_float_pixel
		if (FeatureAvailable[IRR_ARB_half_float_pixel])
			type = GL_HALF_FLOAT_ARB;
		else
#endif
		{
			type = GL_FLOAT;
			format = ECF_A32B32G32R32F;
		}
		break;
	case ECF_R32F:
		if (FeatureAvailable[IRR_ARB_texture_rg])
			fmt = GL_RED;
		else
			fmt = GL_LUMINANCE;
		type = GL_FLOAT;
		break;
	case ECF_G32R32F:
#ifdef GL_ARB_texture_rg
		if (FeatureAvailable[IRR_ARB_texture_rg])
			fmt = GL_RG;
		else
#endif
			fmt = GL_LUMINANCE_ALPHA;
		type = GL_FLOAT;
		break;
	case ECF_A32B32G32R32F:
		fmt = GL_BGRA;
		type = GL_FLOAT;
		break;
	default:
		fmt = GL_BGRA;
		type = GL_UNSIGNED_BYTE;
		break;
	}
	IImage* newImage = createImage(format, ScreenSize);

	u8* pixels = 0;
	if (newImage)
		pixels = static_cast<u8*>(newImage->lock());
	if (pixels)
	{
		GLenum tgt=GL_FRONT;
		switch (target)
		{
		case video::ERT_FRAME_BUFFER:
			break;
		case video::ERT_STEREO_LEFT_BUFFER:
			tgt=GL_FRONT_LEFT;
			break;
		case video::ERT_STEREO_RIGHT_BUFFER:
			tgt=GL_FRONT_RIGHT;
			break;
		default:
			tgt=GL_AUX0+(target-video::ERT_AUX_BUFFER0);
			break;
		}
		glReadBuffer(tgt);
		glReadPixels(0, 0, ScreenSize.Width, ScreenSize.Height, fmt, type, pixels);
		if ( testGLError() )
		{
			os::Printer::log("glReadPixels failed", ELL_ERROR);
		}
		glReadBuffer(GL_BACK);
	}

#ifdef GL_MESA_pack_invert
	if (FeatureAvailable[IRR_MESA_pack_invert])
		glPixelStorei(GL_PACK_INVERT_MESA, GL_FALSE);
	else
#endif
	if (pixels)
	{
		// opengl images are horizontally flipped, so we have to fix that here.
		const s32 pitch=newImage->getPitch();
		u8* p2 = pixels + (ScreenSize.Height - 1) * pitch;
		u8* tmpBuffer = new u8[pitch];
		for (u32 i=0; i < ScreenSize.Height; i += 2)
		{
			memcpy(tmpBuffer, pixels, pitch);
//			for (u32 j=0; j<pitch; ++j)
//			{
//				pixels[j]=(u8)(p2[j]*255.f);
//			}
			memcpy(pixels, p2, pitch);
//			for (u32 j=0; j<pitch; ++j)
//			{
//				p2[j]=(u8)(tmpBuffer[j]*255.f);
//			}
			memcpy(p2, tmpBuffer, pitch);
			pixels += pitch;
			p2 -= pitch;
		}
		delete [] tmpBuffer;
	}

	if (newImage)
	{
		newImage->unlock();
		if (testGLError() || !pixels)
		{
			os::Printer::log("createScreenShot failed", ELL_ERROR);
			newImage->drop();
			return 0;
		}
	}
	return newImage;
}


//! get depth texture for the given render target texture
ITexture* COpenGLDriver::createDepthTexture(ITexture* texture, bool shared)
{
	if ((texture->getDriverType() != EDT_OPENGL) || (!texture->isRenderTarget()))
		return 0;
	COpenGLTexture* tex = static_cast<COpenGLTexture*>(texture);

	if (!tex->isFrameBufferObject())
		return 0;

	if (shared)
	{
		for (u32 i=0; i<DepthTextures.size(); ++i)
		{
			if (DepthTextures[i]->getSize()==texture->getSize())
			{
				DepthTextures[i]->grab();
				return DepthTextures[i];
			}
		}
		DepthTextures.push_back(new COpenGLFBODepthTexture(texture->getSize(), "depth1", this));
		return DepthTextures.getLast();
	}
	return (new COpenGLFBODepthTexture(texture->getSize(), "depth1", this));
}


void COpenGLDriver::removeDepthTexture(ITexture* texture)
{
	for (u32 i=0; i<DepthTextures.size(); ++i)
	{
		if (texture==DepthTextures[i])
		{
			DepthTextures.erase(i);
			return;
		}
	}
}


//! Set/unset a clipping plane.
bool COpenGLDriver::setClipPlane(u32 index, const core::plane3df& plane, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return false;

	UserClipPlanes[index].Plane=plane;
	enableClipPlane(index, enable);
	return true;
}


void COpenGLDriver::uploadClipPlane(u32 index)
{
	// opengl needs an array of doubles for the plane equation
	GLdouble clip_plane[4];
	clip_plane[0] = UserClipPlanes[index].Plane.Normal.X;
	clip_plane[1] = UserClipPlanes[index].Plane.Normal.Y;
	clip_plane[2] = UserClipPlanes[index].Plane.Normal.Z;
	clip_plane[3] = UserClipPlanes[index].Plane.D;
	glClipPlane(GL_CLIP_PLANE0 + index, clip_plane);
}


//! Enable/disable a clipping plane.
void COpenGLDriver::enableClipPlane(u32 index, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return;
	if (enable)
	{
		if (!UserClipPlanes[index].Enabled)
		{
			uploadClipPlane(index);
			glEnable(GL_CLIP_PLANE0 + index);
		}
	}
	else
		glDisable(GL_CLIP_PLANE0 + index);

	UserClipPlanes[index].Enabled=enable;
}


core::dimension2du COpenGLDriver::getMaxTextureSize() const
{
	return core::dimension2du(MaxTextureSize, MaxTextureSize);
}


//! Convert E_PRIMITIVE_TYPE to OpenGL equivalent
GLenum COpenGLDriver::primitiveTypeToGL(scene::E_PRIMITIVE_TYPE type) const
{
	switch (type)
	{
		case scene::EPT_POINTS:
			return GL_POINTS;
		case scene::EPT_LINE_STRIP:
			return GL_LINE_STRIP;
		case scene::EPT_LINE_LOOP:
			return GL_LINE_LOOP;
		case scene::EPT_LINES:
			return GL_LINES;
		case scene::EPT_TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;
		case scene::EPT_TRIANGLE_FAN:
			return GL_TRIANGLE_FAN;
		case scene::EPT_TRIANGLES:
			return GL_TRIANGLES;
		case scene::EPT_QUAD_STRIP:
			return GL_QUAD_STRIP;
		case scene::EPT_QUADS:
			return GL_QUADS;
		case scene::EPT_POLYGON:
			return GL_POLYGON;
		case scene::EPT_POINT_SPRITES:
#ifdef GL_ARB_point_sprite
			return GL_POINT_SPRITE_ARB;
#else
			return GL_POINTS;
#endif
	}
	return GL_TRIANGLES;
}


GLenum COpenGLDriver::getGLBlend(E_BLEND_FACTOR factor) const
{
	GLenum r = 0;
	switch (factor)
	{
		case EBF_ZERO:			r = GL_ZERO; break;
		case EBF_ONE:			r = GL_ONE; break;
		case EBF_DST_COLOR:		r = GL_DST_COLOR; break;
		case EBF_ONE_MINUS_DST_COLOR:	r = GL_ONE_MINUS_DST_COLOR; break;
		case EBF_SRC_COLOR:		r = GL_SRC_COLOR; break;
		case EBF_ONE_MINUS_SRC_COLOR:	r = GL_ONE_MINUS_SRC_COLOR; break;
		case EBF_SRC_ALPHA:		r = GL_SRC_ALPHA; break;
		case EBF_ONE_MINUS_SRC_ALPHA:	r = GL_ONE_MINUS_SRC_ALPHA; break;
		case EBF_DST_ALPHA:		r = GL_DST_ALPHA; break;
		case EBF_ONE_MINUS_DST_ALPHA:	r = GL_ONE_MINUS_DST_ALPHA; break;
		case EBF_SRC_ALPHA_SATURATE:	r = GL_SRC_ALPHA_SATURATE; break;
	}
	return r;
}

GLenum COpenGLDriver::getZBufferBits() const
{
	GLenum bits = 0;
	switch (Params.ZBufferBits)
	{
	case 16:
		bits = GL_DEPTH_COMPONENT16;
		break;
	case 24:
		bits = GL_DEPTH_COMPONENT24;
		break;
	case 32:
		bits = GL_DEPTH_COMPONENT32;
		break;
	default:
		bits = GL_DEPTH_COMPONENT;
		break;
	}
	return bits;
}

COpenGLDriver::E_OPENGL_FIXED_PIPELINE_STATE COpenGLDriver::getFixedPipelineState() const
{
	return FixedPipelineState;
}

void COpenGLDriver::setFixedPipelineState(COpenGLDriver::E_OPENGL_FIXED_PIPELINE_STATE state)
{
	FixedPipelineState = state;
}

const SMaterial& COpenGLDriver::getCurrentMaterial() const
{
	return Material;
}

COpenGLCallBridge* COpenGLDriver::getBridgeCalls() const
{
	return BridgeCalls;
}

#ifdef _IRR_COMPILE_WITH_CG_
const CGcontext& COpenGLDriver::getCgContext()
{
	return CgContext;
}
#endif

COpenGLCallBridge::COpenGLCallBridge(COpenGLDriver* driver) : Driver(driver),
	AlphaMode(GL_ALWAYS), AlphaRef(0.0f), AlphaTest(false),
	BlendSource(GL_ONE), BlendDestination(GL_ZERO), Blend(false),
	ClientStateVertex(false), ClientStateNormal(false), ClientStateColor(false), ClientStateTexCoord0(false),
	CullFaceMode(GL_BACK), CullFace(false),
	DepthFunc(GL_LESS), DepthMask(true), DepthTest(false), MatrixMode(GL_MODELVIEW),
	ActiveTexture(GL_TEXTURE0_ARB), ClientActiveTexture(GL_TEXTURE0_ARB)
{
	// Initial OpenGL values from specification.

	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		Texture[i] = 0;
		TextureType[i] = GL_TEXTURE_2D;
		TextureFixedPipeline[i] = true;
	}

	glAlphaFunc(GL_ALWAYS, 0.0f);
	glDisable(GL_ALPHA_TEST);

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);

	if(Driver->MultiTextureExtension)
	{
		Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
		Driver->extGlClientActiveTexture(GL_TEXTURE0_ARB);
	}

	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void COpenGLCallBridge::setAlphaFunc(GLenum mode, GLclampf ref)
{
	if(AlphaMode != mode || AlphaRef != ref)
	{
		glAlphaFunc(mode, ref);

		AlphaMode = mode;
		AlphaRef = ref;
	}
}

void COpenGLCallBridge::setAlphaTest(bool enable)
{
	if(AlphaTest != enable)
	{
		if (enable)
			glEnable(GL_ALPHA_TEST);
		else
			glDisable(GL_ALPHA_TEST);
		AlphaTest = enable;
	}
}

void COpenGLCallBridge::setBlendFunc(GLenum source, GLenum destination)
{
	if(BlendSource != source || BlendDestination != destination)
	{
		glBlendFunc(source, destination);

		BlendSource = source;
		BlendDestination = destination;
	}
}

void COpenGLCallBridge::setBlend(bool enable)
{
	if(Blend != enable)
	{
		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		Blend = enable;
	}
}

void COpenGLCallBridge::setClientState(bool vertex, bool normal, bool color, bool texCoord0)
{
	if(ClientStateVertex != vertex)
	{
		if(vertex)
			glEnableClientState(GL_VERTEX_ARRAY);
		else
			glDisableClientState(GL_VERTEX_ARRAY);

		ClientStateVertex = vertex;
	}

	if(ClientStateNormal != normal)
	{
		if(normal)
			glEnableClientState(GL_NORMAL_ARRAY);
		else
			glDisableClientState(GL_NORMAL_ARRAY);

		ClientStateNormal = normal;
	}

	if(ClientStateColor != color)
	{
		if(color)
			glEnableClientState(GL_COLOR_ARRAY);
		else
			glDisableClientState(GL_COLOR_ARRAY);

		ClientStateColor = color;
	}

	if(ClientStateTexCoord0 != texCoord0)
	{
		setClientActiveTexture(GL_TEXTURE0_ARB);

		if(texCoord0)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		else
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		ClientStateTexCoord0 = texCoord0;
	}
}

void COpenGLCallBridge::setCullFaceFunc(GLenum mode)
{
	if(CullFaceMode != mode)
	{
		glCullFace(mode);
		CullFaceMode = mode;
	}
}

void COpenGLCallBridge::setCullFace(bool enable)
{
	if(CullFace != enable)
	{
		if (enable)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		CullFace = enable;
	}
}

void COpenGLCallBridge::setDepthFunc(GLenum mode)
{
	if(DepthFunc != mode)
	{
		glDepthFunc(mode);
		DepthFunc = mode;
	}
}

void COpenGLCallBridge::setDepthMask(bool enable)
{
	if(DepthMask != enable)
	{
		if (enable)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
		DepthMask = enable;
	}
}

void COpenGLCallBridge::setDepthTest(bool enable)
{
	if(DepthTest != enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		DepthTest = enable;
	}
}

void COpenGLCallBridge::setMatrixMode(GLenum mode)
{
	if (MatrixMode != mode)
	{
		glMatrixMode(mode);
		MatrixMode = mode;
	}
}

void COpenGLCallBridge::setActiveTexture(GLenum texture)
{
	if (Driver->MultiTextureExtension && ActiveTexture != texture)
	{
		Driver->extGlActiveTexture(texture);
		ActiveTexture = texture;
	}
}

void COpenGLCallBridge::setClientActiveTexture(GLenum texture)
{
	if (Driver->MultiTextureExtension && ClientActiveTexture != texture)
	{
		Driver->extGlClientActiveTexture(texture);
		ClientActiveTexture = texture;
	}
}

void COpenGLCallBridge::getTexture(u32 stage, GLenum& type, bool& fixedPipeline)
{
	if (stage < MATERIAL_MAX_TEXTURES)
	{
		type = TextureType[stage];
		fixedPipeline = TextureFixedPipeline[stage];
	}
}

void COpenGLCallBridge::setTexture(u32 stage, GLenum type, bool fixedPipeline)
{
	if (stage < MATERIAL_MAX_TEXTURES)
	{
		if((fixedPipeline && TextureFixedPipeline[stage] != fixedPipeline) || Texture[stage] != Driver->CurrentTexture[stage] || type != TextureType[stage])
		{
			setActiveTexture(GL_TEXTURE0_ARB + stage);

			if(Driver->CurrentTexture[stage])
			{
				if(fixedPipeline)
				{
					glDisable(TextureType[stage]);
					glEnable(type);
				}

				glBindTexture(type, static_cast<const COpenGLTexture*>(Driver->CurrentTexture[stage])->getOpenGLTextureName());
			}
			else
			{
				glBindTexture(type, 0);

				if(fixedPipeline)
					glDisable(TextureType[stage]);
			}

			TextureFixedPipeline[stage] = fixedPipeline;
			TextureType[stage] = type;
			Texture[stage] = Driver->CurrentTexture[stage];
		}
	}
}


} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_OPENGL_

namespace irr
{
namespace video
{


// -----------------------------------
// WINDOWS VERSION
// -----------------------------------
#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params,
	io::IFileSystem* io, IContextManager* contextManager)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	COpenGLDriver* ogl =  new COpenGLDriver(params, io, contextManager);
	if (!ogl->initDriver((CIrrDeviceWin32*)0))
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
#else
	return 0;
#endif // _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_COMPILE_WITH_WINDOWS_DEVICE_

// -----------------------------------
// MACOSX VERSION
// -----------------------------------
#if defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, IContextManager* contextManager)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	COpenGLDriver* ogl =  new COpenGLDriver(params, io, contextManager);
	if (!ogl->initDriver())
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
#else
	return 0;
#endif // _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_COMPILE_WITH_OSX_DEVICE_

// -----------------------------------
// X11 VERSION
// -----------------------------------
#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, IContextManager* contextManager)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	COpenGLDriver* ogl =  new COpenGLDriver(params, io, contextManager);
	if (!ogl->initDriver((CIrrDeviceLinux*)0))
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_COMPILE_WITH_X11_DEVICE_


// -----------------------------------
// SDL VERSION
// -----------------------------------
#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, CIrrDeviceSDL* device)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	return new COpenGLDriver(params, io, device);
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_COMPILE_WITH_SDL_DEVICE_

} // end namespace
} // end namespace


