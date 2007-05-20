// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IVideoDriver.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Video
{
	IVideoDriver::IVideoDriver(irr::video::IVideoDriver* driver)
		: Driver(driver), Services(0)
	{
		if (Driver && Driver->getGPUProgrammingServices())
		{
			Services = new Irrlicht::Video::IGPUProgrammingServices(
									Driver->getGPUProgrammingServices(),
									this );
		}
	}

	bool IVideoDriver::BeginScene(bool backBuffer, bool zBuffer, Color color)
	{
		return Driver->beginScene(backBuffer, zBuffer, (irr::s32)color.color);
	}

	bool IVideoDriver::EndScene()
	{
		return Driver->endScene();
	}

	bool IVideoDriver::EndScene( System::IntPtr ptr )
	{
		return Driver->endScene( reinterpret_cast<irr::s32>((void*)ptr) );
	}

	bool IVideoDriver::EndScene( System::IntPtr ptr, Core::Rect sourceRect )
	{
		irr::core::rect<irr::s32> rct = irr::NativeConverter::getNativeRect( sourceRect );

		return Driver->endScene( reinterpret_cast<irr::s32>((void*)ptr), &rct );
	}

	bool IVideoDriver::QueryFeature(VideoDriverFeature feature)
	{
		return Driver->queryFeature((irr::video::E_VIDEO_DRIVER_FEATURE)feature);
	}

	void IVideoDriver::SetTransform(TransformationState state, Core::Matrix4 mat)
	{
		irr::core::matrix4 imat;
		for (int i=0; i<16; ++i)
			imat[i] = mat.Members[i];

		Driver->setTransform((irr::video::E_TRANSFORMATION_STATE)state, imat);
	}

	Core::Matrix4 IVideoDriver::GetTransform( TransformationState state )
	{
		Core::Matrix4 mat;

		irr::core::matrix4 imat = Driver->getTransform((irr::video::E_TRANSFORMATION_STATE)state );

		for (int i=0; i<16; ++i)
			mat.Members[i] = imat[i];

		return mat;
	}


	ITexture* IVideoDriver::GetTexture(System::String* filename)
	{
		ITexture* ret = 0;
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filename);
	
		irr::video::ITexture* tex = Driver->getTexture(str);
		if (tex)
			ret = new ITexture(tex);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
		return ret;
	}

	ITexture* IVideoDriver::AddTexture(Core::Dimension2D size, System::String* name,
		ColorFormat format)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

		irr::video::ITexture* tex = 
			Driver->addTexture(irr::core::dimension2d<irr::s32>(size.Width, size.Height),
			str, (irr::video::ECOLOR_FORMAT)format);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
		return tex ? new ITexture(tex) : 0;
	}

	ITexture* IVideoDriver::AddTexture(Core::Dimension2D size, System::String* name)
	{
		return AddTexture(size, name, ColorFormat::A1R5G5B5);
	}

	void IVideoDriver::SetMaterial(Material material)
	{
		Driver->setMaterial(irr::NativeConverter::getNativeMaterial(material));
	}

	void IVideoDriver::RemoveTexture(ITexture* texture)
	{
		if (!texture)
			return;

		Driver->removeTexture(texture->get_NativeTexture());
	}

	void IVideoDriver::RemoveAllTextures()
	{
		Driver->removeAllTextures();
	}

	void IVideoDriver::MakeColorKeyTexture(ITexture* texture, Color color)
	{
		if (texture)
			Driver->makeColorKeyTexture(texture->get_NativeTexture(), color.color);
	}

	void IVideoDriver::MakeColorKeyTexture(ITexture* texture, Core::Position2D colorKeyPixelPos)
	{
		if (texture)
			Driver->makeColorKeyTexture(texture->get_NativeTexture(), 
			irr::core::position2d<irr::s32>(colorKeyPixelPos.X, colorKeyPixelPos.Y));
	}

	void IVideoDriver::SetRenderTarget(ITexture* texture)
	{
		if (texture)
			Driver->setRenderTarget(texture->get_NativeTexture());
	}

	void IVideoDriver::set_ViewPort(Core::Rect area)
	{
		Driver->setViewPort(irr::core::rect<irr::s32>(area.UpperLeftCorner.X, area.UpperLeftCorner.Y,
			area.LowerRightCorner.X, area.LowerRightCorner.Y));
	}

	Core::Rect IVideoDriver::get_ViewPort()
	{
		irr::core::rect<irr::s32> r = Driver->getViewPort();
		return Core::Rect(r.UpperLeftCorner.X, r.UpperLeftCorner.Y,
			r.LowerRightCorner.X, r.LowerRightCorner.Y);
	}

	void IVideoDriver::DrawIndexedTriangleList(Vertex3D vertices[],
			int vertexCount, unsigned short indexList __gc[], int triangleCount)
	{
		Vertex3D __pin * pVertices = &vertices[0];
		unsigned short __pin * pIndices = &indexList[0];

		Driver->drawIndexedTriangleList((irr::video::S3DVertex*)((void*)pVertices),
			vertexCount, 
			(unsigned short*)((void*)pIndices),
			triangleCount);
	}

	void IVideoDriver::DrawIndexedTriangleList(Vertex3D2Tex vertices[],
			int vertexCount, unsigned short indexList __gc[], int triangleCount)
	{
		Vertex3D2Tex __pin * pVertices = &vertices[0];
		unsigned short __pin * pIndices = &indexList[0];

		Driver->drawIndexedTriangleList((irr::video::S3DVertex2TCoords*)((void*)pVertices),
			vertexCount, 
			(unsigned short*)((void*)pIndices),
			triangleCount);
	}

    void IVideoDriver::DrawIndexedTriangleFan(Vertex3D vertices[],
			int vertexCount, unsigned short indexList __gc[], int triangleCount)
	{
		Vertex3D __pin * pVertices = &vertices[0];
		unsigned short __pin * pIndices = &indexList[0];

		Driver->drawIndexedTriangleFan((irr::video::S3DVertex*)((void*)pVertices),
			vertexCount, 
			(unsigned short*)((void*)pIndices),
			triangleCount);
	}

	void IVideoDriver::DrawIndexedTriangleFan(Vertex3D2Tex vertices[],
			int vertexCount, unsigned short indexList __gc[], int triangleCount)
	{
		Vertex3D2Tex __pin * pVertices = &vertices[0];
		unsigned short __pin * pIndices = &indexList[0];

		Driver->drawIndexedTriangleFan((irr::video::S3DVertex2TCoords*)((void*)pVertices),
			vertexCount, 
			(unsigned short*)((void*)pIndices),
			triangleCount);
	}

	void IVideoDriver::Draw3DLine(Core::Vector3D start, Core::Vector3D end,
			Color color)
	{
		Driver->draw3DLine(irr::core::vector3df(start.X, start.Y, start.Z),
			irr::core::vector3df(end.X, end.Y, end.Z), color.color);
	}

	int IVideoDriver::get_PrimitiveCountDrawn()
	{
		return Driver->getPrimitiveCountDrawn();
	}

	int IVideoDriver::get_FPS()
	{
		return Driver->getFPS();
	}

	void IVideoDriver::Draw2DImage(ITexture* texture, Core::Position2D pos)
	{
		if (!texture)
			return;

		Driver->draw2DImage(texture->get_NativeTexture(),
			irr::core::position2d<irr::s32>(pos.X, pos.Y));
	}

	void IVideoDriver::Draw3DTriangle(Core::Triangle3D triangle, Color color)
	{
		irr::core::triangle3d<irr::f32> tri;
		tri.set(
			irr::core::vector3df(triangle.pointA.X, triangle.pointA.Y, triangle.pointA.Z),
			irr::core::vector3df(triangle.pointB.X, triangle.pointB.Y, triangle.pointB.Z),
			irr::core::vector3df(triangle.pointC.X, triangle.pointC.Y, triangle.pointC.Z));

		Driver->draw3DTriangle(tri, color.color);
	}

	void IVideoDriver::Draw3DBox(Core::Box3D box, Color color)
	{
		Driver->draw3DBox( irr::NativeConverter::getNativeBox(box), 
						   irr::NativeConverter::getNativeColor(color));
	}


	void IVideoDriver::Draw2DImage(ITexture* texture, Core::Position2D destPos,
			Core::Rect sourceRect, Core::Rect clipRect,
			Color color, bool useAlphaChannelOfTexture)
	{
		if (!texture)
			return;

		irr::core::rect<irr::s32> clip = irr::NativeConverter::getNativeRect(clipRect);

		Driver->draw2DImage(texture->get_NativeTexture(), 
			irr::NativeConverter::getNativePos(destPos),
			irr::NativeConverter::getNativeRect(sourceRect),
			&clip,
			color.color, useAlphaChannelOfTexture);
	}

	void IVideoDriver::Draw2DImage(ITexture* texture, Core::Position2D destPos,
		Core::Rect sourceRect, 	Color color, bool useAlphaChannelOfTexture)
	{
		if (!texture)
			return;

		Driver->draw2DImage(texture->get_NativeTexture(), 
			irr::NativeConverter::getNativePos(destPos),
			irr::NativeConverter::getNativeRect(sourceRect),
			0,	color.color, useAlphaChannelOfTexture);
	}

	void IVideoDriver::Draw2DRectangle(Color color, Core::Rect pos, Core::Rect clipp)
	{
		irr::core::rect<irr::s32> clip = irr::NativeConverter::getNativeRect(clipp);
		Driver->draw2DRectangle(color.color, 
			irr::NativeConverter::getNativeRect(pos), &clip);
	}

	void IVideoDriver::Draw2DRectangle(Color color, Core::Rect pos)
	{
		Driver->draw2DRectangle(color.color, 
			irr::NativeConverter::getNativeRect(pos), 0);
	}

	void IVideoDriver::Draw2DLine(Core::Position2D start, Core::Position2D end, Color color)
	{
		Driver->draw2DLine(
			irr::NativeConverter::getNativePos(start),
			irr::NativeConverter::getNativePos(end),
			color.color);
	}

	void IVideoDriver::DrawStencilShadowVolume(Core::Vector3D triangles[], int count, bool zfail)
	{
		Core::Vector3D __pin * v = &triangles[0];
		Driver->drawStencilShadowVolume((irr::core::vector3df*)((void*)v), count, zfail);
	}

	void IVideoDriver::DrawStencilShadow(bool clearStencilBuffer, 
			Color leftUpEdge, Color rightUpEdge, Color leftDownEdge, Color rightDownEdge)
	{
		Driver->drawStencilShadow(clearStencilBuffer,
			leftUpEdge.color, rightUpEdge.color, leftDownEdge.color, rightDownEdge.color);
	}

	void IVideoDriver::SetFog(Color color, bool linearFog, float start, float end, 
		float density, bool pixelFog, bool rangeFog)
	{
		Driver->setFog(color.color, linearFog, start, end, density, pixelFog, rangeFog);
	}

	Core::Dimension2D IVideoDriver::get_ScreenSize()
	{
		irr::core::dimension2d<irr::s32> dim = Driver->getScreenSize();
		return Core::Dimension2D(dim.Width, dim.Height);
	}

	void IVideoDriver::DeleteAllDynamicLights()
	{
		Driver->deleteAllDynamicLights();
	}

	void IVideoDriver::AddDynamicLight(Light light)
	{
		Driver->addDynamicLight(irr::NativeConverter::getNativeLight(light));
	}

	void IVideoDriver::SetAmbientLight(Colorf color)
	{
		Driver->setAmbientLight(irr::NativeConverter::getNativeColorf(color));
	}

	int IVideoDriver::get_MaximalDynamicLightAmount()
	{
		return Driver->getMaximalDynamicLightAmount();
	}

	int IVideoDriver::get_DynamicLightCount()
	{
		return Driver->getDynamicLightCount();
	}

	Light IVideoDriver::GetDynamicLight(int idx)
	{
		return irr::NativeConverter::getNETLight(
			Driver->getDynamicLight(idx));
	}

	System::String* IVideoDriver::get_Name()
	{
		return new System::String(Driver->getName());
	}

	int IVideoDriver::get_MaximalPrimitiveCount()
	{
		return Driver->getMaximalPrimitiveCount();
	}

	void IVideoDriver::SetTextureCreationFlag(TextureCreationFlag flag, bool enabled)
	{
		Driver->setTextureCreationFlag((irr::video::E_TEXTURE_CREATION_FLAG)flag, enabled);
	}

	bool IVideoDriver::GetTextureCreationFlag(TextureCreationFlag flag)
	{
		return Driver->getTextureCreationFlag((irr::video::E_TEXTURE_CREATION_FLAG)flag);
	}

	IGPUProgrammingServices* IVideoDriver::get_GPUProgrammingServices()
	{
		return Services;
	}

	DriverType IVideoDriver::get_DriverType()
	{
		return (Irrlicht::Video::DriverType)Driver->getDriverType();
	}

}
}
