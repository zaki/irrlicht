#pragma once

using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "IMaterialRenderer.h"

namespace Irrlicht
{
namespace Video
{
	public __gc class IMaterialRendererServices;

	public __gc __interface IShaderConstantSetCallBack
	{
	public:

		/// <summary>
		///	Called by the engine when the vertex and/or pixel shader constants for an
		/// material renderer should be set.
		/// </summary>
		void OnSetConstants( Irrlicht::Video::IMaterialRendererServices* services );
	};
}
}