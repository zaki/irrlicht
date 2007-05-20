#pragma once

using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Material.h"
#include "IShaderConstantSetCallBack.h"

namespace Irrlicht
{
namespace Video
{
	/// <summary>
	/// Compile target enumeration for the addHighLevelShaderMaterial() method. 
	/// </summary>
	public __value enum VertexShaderType
	{
		VST_VS_1_1 = 0,
		VST_VS_2_0,
		VST_VS_2_a,
		VST_VS_3_0,
		VST_COUNT
	};

	/// <summary>
	/// Compile target enumeration for the addHighLevelShaderMaterial() method. 
	/// </summary>
	public __value enum PixelShaderType
	{
		PST_PS_1_1 = 0,
		PST_PS_1_2,
		PST_PS_1_3,
		PST_PS_1_4,
		PST_PS_2_0,
		PST_PS_2_a,
		PST_PS_2_b,
		PST_PS_3_0,
		PST_COUNT
	};

	/// <summary>
	///	Interface making it possible to create and use programs running on the GPU.
	/// </summary>
	public __gc class IGPUProgrammingServices
	{
	public:

		IGPUProgrammingServices( irr::video::IGPUProgrammingServices* realGPU, 
								 Irrlicht::Video::IVideoDriver* videoDriver );

		/// <summary>
		/// Adds a new material renderer to the VideoDriver, based on a high level shading 
		/// language. Currently only HLSL/D3D9 and GLSL/OpenGL is supported. 
		/// </summary>
		/// <param name="vertexShaderProgram> String containing the source of the vertex shader program.
		/// This can be 0 if no vertex program should be used. </param>
		/// <param name="Entry">name of the function of the vertexShaderProgram</param>
		/// <param name="vsCompileTarget"> Vertex shader version where the high level shader should be compiled to.</param>
		/// <param name="pixelShaderProgram"> String containing the source of the pixel shader program.
		/// This can be 0 if no pixel shader should be used. </param>
		/// <param name="pixelShaderEntryPointName"> Entry name of the function of the pixelShaderEntryPointName</param>
		/// <param name="psCompileTarget">Pixel shader version where the high level shader should be compiled to.</param>
		/// <param name="callback"> Pointer to an implementation of IShaderConstantSetCallBack in which you 
		/// can set the needed vertex and pixel shader program constants. Set this to 0 if you don't need this.</param>
		/// <param name="baseMaterial">Base material which renderstates will be used to shade the
		/// material.</param>
		/// <returns>Returns the number of the
		/// material type which can be set in SMaterial::MaterialType to use the renderer.
		/// -1 is returned if an error occured. -1 is returned for example if a vertex or pixel shader
		/// program could not be compiled or a compile target is not reachable.
		/// The error strings are then printed out into the error log, and
		/// can be catched with a custom event receiver.</returns>
		int AddHighLevelShaderMaterial( String* vertexShaderProgram, String* vertexShaderEntryPointName,
			VertexShaderType vsCompileTarget, String* pixelShaderProgram, String* pixelShaderEntryPointName,
			PixelShaderType psCompileTarget, IShaderConstantSetCallBack* callback,
			MaterialType baseMaterial );   

		/// <summary>
		/// Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
		/// but tries to load the programs from files. </summary>
		/// <param name="vertexShaderProgram"> Text file containing the source of the vertex shader program.
		/// Set to null if no shader should be created.</param>
		/// <param name="pixelShaderProgram"> Text file containing the source of the pixel shader program. Set to 
		/// 0 if no shader should be created.</param>
		int AddHighLevelShaderMaterialFromFiles(
				String* vertexShaderProgram,
				String* vertexShaderEntryPointName,
				VertexShaderType vsCompileTarget,
				String* pixelShaderProgram, 
				String* pixelShaderEntryPointName,
				PixelShaderType psCompileTarget,
				IShaderConstantSetCallBack* callback,
				MaterialType baseMaterial) ; 

		/// <summary>
		/// Adds a new material renderer to the VideoDriver, using pixel and/or 
		/// vertex shaders to render geometry.
		/// Note that it is a good idea to call IVideoDriver::queryFeature() before to check 
		/// if the IVideoDriver supports the vertex and/or pixel shader version your are using.  
		/// The material is added to the VideoDriver like with IVideoDriver::addMaterialRenderer()
		/// and can be used like it had been added with that method.
		/// </summary>
		/// <param name="vertexShaderProgram"> String containing the source of the vertex shader program. This can be
		/// 0 if no vertex program should be used. 
		/// For DX8 programs, the will always input registers look like this:
		/// v0: position, v1: normal, 
		/// v2: color, v3: texture cooridnates, v4: texture coordinates 2 if available.
		/// For DX9 programs, you can manually set the registers using the dcl_ statements.</param>
		/// <param name="pixelShaderProgram"> String containing the source of the pixel shader program.
		/// This can be 0 if you don't want to use a pixel shader. </param>
		/// <param name="callback"> Pointer to an implementation of IShaderConstantSetCallBack in which you 
		/// can set the needed vertex and pixel shader program constants. Set this to 0 if you don't need this.</param>
		/// <param name="baseMaterial"> Base material which renderstates will be used to shade the
		/// material.</param>
		/// <returns> Returns the number of the
		/// material type which can be set in SMaterial::MaterialType to use the renderer.
		/// -1 is returned if an error occured. -1 is returned for example if a vertex or pixel shader
		/// program could not be compiled, the error strings are then printed out into the error log, and
		/// can be catched with a custom event receiver.</returns>
		int AddShaderMaterial( String* vertexShaderProgram, String* pixelShaderProgram, 
				IShaderConstantSetCallBack* callback,
				MaterialType baseMaterial );

		/// <summary>
		/// Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the 
		/// programs from files.
		/// </summary>
		/// <param name="vertexShaderProgram"> Text file containing the source of the vertex shader program.
		/// Set to null if no shader should be created.</param>
		/// <param name="pixelShaderProgram"> Text file containing the source of the pixel shader program. Set to 
		/// null if no shader should be created.</param>
		int AddShaderMaterialFromFiles( String* vertexShaderProgram, String* pixelShaderProgram,
				IShaderConstantSetCallBack* callback,
				MaterialType baseMaterial );

		/// <summary>
		/// Internal list for mapping C++ callbacks to .NET callbacks. This was made public because 
		/// of an obvious bug in managed C++. Simply don't use this.
		/// </summary>
		__property static System::Collections::ArrayList* get_CallbackMap()
		{
			return CallBackList;
		}

		__gc class SServiceCallbackMapping
		{
		public:
			Irrlicht::Video::IGPUProgrammingServices* ManagedServices;
			Irrlicht::Video::IShaderConstantSetCallBack* ManagedCallback;
			Irrlicht::Video::IMaterialRendererServices* ManagedMaterialServices;
		};

		/// <summary>
		/// Returns the video driver.
		/// </summary>
		__property Irrlicht::Video::IVideoDriver* get_VideoDriver();

	protected:

		irr::video::IGPUProgrammingServices* gpuProgrammingServices;

		inline irr::video::IGPUProgrammingServices* getGPUProgrammingServices()
		{
			return (irr::video::IGPUProgrammingServices*)gpuProgrammingServices;
		}

		/// <summary>
		/// Private method for receiving shader callbacks from the native C++ Irrlicht engine and 
		/// to map them to the .NET callback method
		/// </summary>
		__nogc class NativeCallbackReceiver : public irr::video::IShaderConstantSetCallBack
		{
		public:
			void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);
		};

		int registerCallback(Irrlicht::Video::IShaderConstantSetCallBack* callback);

		NativeCallbackReceiver* Receiver;
		static System::Collections::ArrayList* CallBackList;
		Irrlicht::Video::IVideoDriver* TheVideoDriver;
	};

}
}