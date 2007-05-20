// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IMaterialRenderer.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Video
{
	IMaterialRendererServices::IMaterialRendererServices( irr::video::IMaterialRendererServices* realServices,
														  Irrlicht::Video::IVideoDriver* videoDriver )
		: materialRendererServices(realServices), TheVideoDriver(videoDriver)
	{	
	}

	void IMaterialRendererServices::SetBasicRenderStates( Material material, Material lastMaterial,
		bool resetAllRenderStates )
	{
		if (materialRendererServices)
		materialRendererServices->setBasicRenderStates( irr::NativeConverter::getNativeMaterial( material ),
			irr::NativeConverter::getNativeMaterial( lastMaterial ), resetAllRenderStates );
	}

	bool IMaterialRendererServices::SetVertexShaderConstant( String* name, float data __gc [], int count )
	{
		bool ret = false;

		try 
		{
			float* f = new float[data->get_Count()];

			for( int i = 0; i < data->get_Count(); ++i )
				f[i] = data[i];

			char* str = (char*)(void*)
				System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

			if (materialRendererServices)
				ret = materialRendererServices->setVertexShaderConstant( str, f, count );

			delete [] f;

			System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
		}
		catch(...)
		{
			System::Console::Out->WriteLine("Error while setting vertex shader constant: {0}", name);
		}

		return ret;
	}

	void IMaterialRendererServices::SetVertexShaderConstant( float data __gc [], int startRegister, int constantAmount )
	{
		try
		{
			float* f = new float[data->get_Count()];
			for (int i=0; i<data->get_Count(); ++i)
				f[i] = data[i];

			materialRendererServices->setVertexShaderConstant(f, startRegister, constantAmount);

			delete [] f;
		}
		catch(Exception* e)
		{
			System::Console::Out->WriteLine("Error while setting vertex shader constant: constantAmount:{0}, startRegister:{1}, ({2})", __box(constantAmount), __box(startRegister), e);
		}
	}

	bool IMaterialRendererServices::SetPixelShaderConstant( String* name, float data __gc [], int count )
	{
		bool ret = false;

		try
		{
			float* f = new float[data->get_Count()];

			for( int i = 0; i < data->get_Count(); i++ )
				f[i] = data[i];

			char* str = (char*)(void*)
				System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

			if (materialRendererServices)
				ret = materialRendererServices->setPixelShaderConstant( str, f, count );

			delete [] f;

			System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
		}
		catch(...)
		{
			System::Console::Out->WriteLine("Error while setting pixel shader constant: {0}", name );
		}

		return ret;
	}

	void IMaterialRendererServices::SetPixelShaderConstant( float data __gc [], int startRegister, int constantAmount )
	{
		try
		{
			float* f = new float[data->get_Count()];

			for( int i = 0; i < data->get_Count(); i++ )
				f[i] = data[i];

			materialRendererServices->setPixelShaderConstant( f, startRegister, constantAmount );

			delete [] f;
		}
		catch(Exception* e)
		{
			System::Console::Out->WriteLine("Error while setting pixel shader constant: constantAmount:{0}, startRegister:{1}, ({2})", __box(constantAmount), __box(startRegister), e);
		}
	}

	
	Irrlicht::Video::IVideoDriver* IMaterialRendererServices::get_VideoDriver()
	{
		return TheVideoDriver;
	}

}
}