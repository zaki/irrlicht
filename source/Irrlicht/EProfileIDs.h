// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef E_PROFILE_IDS_H_INCLUDED__
#define E_PROFILE_IDS_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "limits.h"

namespace irr
{
#ifdef _IRR_COMPILE_WITH_PROFILING_
    enum EPROFILE_ID
    {
		// We use negative ID's to avoid clashing with user application id's.
		EPID_FIRST = -INT_MAX,	// not used

		//! scenemanager.
		EPID_SM_DRAW_ALL,
		EPID_SM_ANIMATE,
		EPID_SM_RENDER_CAMERAS,
		EPID_SM_RENDER_LIGHTS,
		EPID_SM_RENDER_SKYBOXES,
		EPID_SM_RENDER_DEFAULT,
		EPID_SM_RENDER_SHADOWS,
		EPID_SM_RENDER_TRANSPARENT,
		EPID_SM_RENDER_EFFECT,
		EPID_SM_REGISTER,

		//! octrees
		EPID_OC_RENDER,
		EPID_OC_CALCPOLYS,

		//! es2 driver
		EPID_ES2_END_SCENE,
		EPID_ES2_BEGIN_SCENE,
		EPID_ES2_UPDATE_VERTEX_HW_BUF,
		EPID_ES2_UPDATE_INDEX_HW_BUF,
		EPID_ES2_DRAW_PRIMITIVES,
		EPID_ES2_DRAW_2DIMAGE,
		EPID_ES2_DRAW_2DIMAGE_BATCH,
		EPID_ES2_DRAW_2DRECTANGLE,
		EPID_ES2_DRAW_2DLINE,
		EPID_ES2_DRAW_3DLINE,
		EPID_ES2_SET_RENDERSTATE_2D,
		EPID_ES2_SET_RENDERSTATE_3D,
		EPID_ES2_SET_RENDERSTATE_BASIC,
		EPID_ES2_SET_RENDERSTATE_TEXTURE,
		EPID_ES2_DRAW_SHADOW
    };
#endif
} // end namespace irr

#endif // E_PROFILE_IDS_H_INCLUDED__
