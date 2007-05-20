#include "IGPUProgrammingServices.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Video
{
	IGPUProgrammingServices::IGPUProgrammingServices( irr::video::IGPUProgrammingServices* realGPU,
													  Irrlicht::Video::IVideoDriver* videoDriver )
		: gpuProgrammingServices( realGPU )
	{
		TheVideoDriver = videoDriver;
		Receiver = new NativeCallbackReceiver();

		if (!CallBackList)
			CallBackList = new System::Collections::ArrayList();
	}

	int IGPUProgrammingServices::AddHighLevelShaderMaterial( String* vertexShaderProgram, String* vertexShaderEntryPointName,
			VertexShaderType vsCompileTarget, String* pixelShaderProgram, String* pixelShaderEntryPointName,
			PixelShaderType psCompileTarget, Irrlicht::Video::IShaderConstantSetCallBack* callback,
			MaterialType baseMaterial )
	{
		char* vsProgram = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(vertexShaderProgram);

		char* vsEntryPoint = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(vertexShaderEntryPointName);

		char* psProgram = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(pixelShaderProgram);

		char* psEntryPoint = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(pixelShaderEntryPointName);

		int userData = registerCallback(callback);

		int ret = gpuProgrammingServices->addHighLevelShaderMaterial( vsProgram, vsEntryPoint,
					(irr::video::E_VERTEX_SHADER_TYPE)vsCompileTarget, psProgram, psEntryPoint,
					(irr::video::E_PIXEL_SHADER_TYPE)psCompileTarget, 
					Receiver,
					(irr::video::E_MATERIAL_TYPE)baseMaterial,
					userData );

		System::Runtime::InteropServices::Marshal::FreeHGlobal(vsProgram);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(vsEntryPoint);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(psProgram);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(psEntryPoint);

		return ret;
	}

	int IGPUProgrammingServices::AddHighLevelShaderMaterialFromFiles( String* vertexShaderProgram,
				String* vertexShaderEntryPointName, VertexShaderType vsCompileTarget,
				String* pixelShaderProgram, String* pixelShaderEntryPointName,
				PixelShaderType psCompileTarget, Irrlicht::Video::IShaderConstantSetCallBack* callback,
				MaterialType baseMaterial) 
	{
		char* vsProgram = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(vertexShaderProgram);

		char* vsEntryPoint = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(vertexShaderEntryPointName);

		char* psProgram = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(pixelShaderProgram);

		char* psEntryPoint = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(pixelShaderEntryPointName);

		int userData = registerCallback(callback);

		int ret =  gpuProgrammingServices->addHighLevelShaderMaterialFromFiles( 
					vsProgram, vsEntryPoint,
					(irr::video::E_VERTEX_SHADER_TYPE)vsCompileTarget, 
					psProgram, psEntryPoint,
					(irr::video::E_PIXEL_SHADER_TYPE)psCompileTarget, 
					Receiver,
					(irr::video::E_MATERIAL_TYPE)baseMaterial,
					userData );

		System::Runtime::InteropServices::Marshal::FreeHGlobal(vsProgram);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(vsEntryPoint);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(psProgram);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(psEntryPoint);

		return ret;
	}

	int IGPUProgrammingServices::AddShaderMaterial( String* vertexShaderProgram, String* pixelShaderProgram, 
			Irrlicht::Video::IShaderConstantSetCallBack* callback,
			MaterialType baseMaterial )
	{
		char* vsProgram = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(vertexShaderProgram);

		char* psProgram = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(pixelShaderProgram);

		int userData = registerCallback(callback);

		int ret = gpuProgrammingServices->addShaderMaterial( vsProgram, psProgram, 
					Receiver, 
					(irr::video::E_MATERIAL_TYPE)baseMaterial,
					userData );

		System::Runtime::InteropServices::Marshal::FreeHGlobal(vsProgram);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(psProgram);
		return ret;
	}

	int IGPUProgrammingServices::AddShaderMaterialFromFiles( String* vertexShaderProgram, String* pixelShaderProgram,
			Irrlicht::Video::IShaderConstantSetCallBack* callback,
			MaterialType baseMaterial )
	{
		char* vsProgram = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(vertexShaderProgram);

		char* psProgram = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(pixelShaderProgram);

		int userData = registerCallback(callback);

		int ret = gpuProgrammingServices->addShaderMaterialFromFiles( 
					vsProgram, 
					psProgram,
					Receiver,
					(irr::video::E_MATERIAL_TYPE)baseMaterial, 
					userData);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(psProgram);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(vsProgram);

		return ret;
	}


	void IGPUProgrammingServices::NativeCallbackReceiver::OnSetConstants( irr::video::IMaterialRendererServices* services,
																		  irr::s32 userData)
	{
		System::Collections::ArrayList* l = IGPUProgrammingServices::get_CallbackMap();

		SServiceCallbackMapping* map = 
			__try_cast<SServiceCallbackMapping*>( l->get_Item(userData) );

		if (map && map->ManagedCallback)
		{
			if ( !map->ManagedMaterialServices && services )
			{
				map->ManagedMaterialServices = new IMaterialRendererServices(services, 
														map->ManagedServices->get_VideoDriver() );
			}

			if (map->ManagedMaterialServices)
				map->ManagedCallback->OnSetConstants( map->ManagedMaterialServices );
		}
		else
			System::Console::Out->WriteLine("Internal shader callback error");
	}

	int IGPUProgrammingServices::registerCallback(Irrlicht::Video::IShaderConstantSetCallBack* callback)
	{
		System::Collections::ArrayList* l = IGPUProgrammingServices::CallBackList;
		SServiceCallbackMapping* mapping = new SServiceCallbackMapping();

		mapping->ManagedServices = this;
		mapping->ManagedCallback = callback;

		l->Add(mapping);

		return l->get_Count() - 1;
	}

	Irrlicht::Video::IVideoDriver* IGPUProgrammingServices::get_VideoDriver()
	{
		return TheVideoDriver;
	}
}
}