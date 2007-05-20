// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#pragma unmanaged
#include "..\\..\\include\\rect.h"
#include "..\\..\\include\\position2d.h"
#pragma managed

#include "Rect.h"
#include "Position2D.h"
#include "Light.h"
#include "Color.h"
#include "Box3D.h"
#include "Matrix4.h"
#include "Material.h"
#include "Event.h"
#include "ViewFrustum.h"
#include "Triangle3D.h"

//   \\param {.*}\:
//   <param name="\1"/>

namespace irr
{
	// Class for converting native classes into .net ones and the other way round.
	// Could be optimized still a lot.
	class NativeConverter
	{
	public:
		
		inline static irr::core::rect<s32> getNativeRect(Irrlicht::Core::Rect r)
		{
			return core::rect<s32>(r.UpperLeftCorner.X, r.UpperLeftCorner.Y,
				r.LowerRightCorner.X, r.LowerRightCorner.Y);
		}

		inline static Irrlicht::Core::Rect getNETRect(irr::core::rect<s32>& r)
		{
			return Irrlicht::Core::Rect(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, 
				r.LowerRightCorner.X, r.LowerRightCorner.Y);
		}

		inline static irr::core::line3d<f32> getNativeLine(Irrlicht::Core::Line3D l)
		{
			return irr::core::line3d<f32>(l.start.X, l.start.Y, l.start.Z, 
				l.end.X, l.end.Y, l.end.Z);
		}

		inline static Irrlicht::Core::Line3D getNETLine(irr::core::line3d<f32>& l)
		{
			return Irrlicht::Core::Line3D(l.start.X, l.start.Y, l.start.Z,
				l.end.X, l.end.Y, l.end.Z);
		}

		inline static Irrlicht::Core::Triangle3D getNETTriangle(core::triangle3df& tri)
		{
			Irrlicht::Core::Triangle3D t;
			t.pointA.Set(tri.pointA.X, tri.pointA.Y, tri.pointA.Z);
			t.pointB.Set(tri.pointB.X, tri.pointB.Y, tri.pointB.Z);
			t.pointC.Set(tri.pointC.X, tri.pointC.Y, tri.pointC.Z);
			return t;
		}

		inline static irr::core::position2d<s32> getNativePos(Irrlicht::Core::Position2D p)
		{
			return irr::core::position2d<s32>(p.X, p.Y);
		}

		inline static irr::video::SLight getNativeLight(Irrlicht::Video::Light light)
		{
			irr::video::SLight l = *((irr::video::SLight*)((void*)&light));
			return l;
		}

		inline static Irrlicht::Video::Light getNETLight(const irr::video::SLight& l)
		{
			Irrlicht::Video::Light light;
			light.AmbientColor = getNETColor(l.AmbientColor);
			light.CastShadows = l.CastShadows;
			light.DiffuseColor = getNETColor(l.DiffuseColor);
			light.Position.Set(l.Position.X, l.Position.Y, l.Position.Z);
			light.Radius = l.Radius;
			light.SpecularColor = getNETColor(l.SpecularColor);
			return light;
		}

		inline static irr::video::SColorf getNativeColorf(Irrlicht::Video::Colorf c)
		{
			return irr::video::SColorf(c.r, c.g, c.b, c.a);
		}

		inline static Irrlicht::Video::Colorf getNETColor(irr::video::SColorf c)
		{
			return Irrlicht::Video::Colorf(c.r, c.g, c.b, c.a);
		}

		inline static Irrlicht::Core::Box3D getNETBox(irr::core::aabbox3df b)
		{
			return Irrlicht::Core::Box3D(b.MinEdge.X, b.MinEdge.Y, b.MinEdge.Z,
				b.MaxEdge.X, b.MaxEdge.Y, b.MaxEdge.Z);
		}

		inline static irr::core::aabbox3df getNativeBox(Irrlicht::Core::Box3D b)
		{
			return irr::core::aabbox3df(b.MinEdge.X, b.MinEdge.Y, b.MinEdge.Z,
				b.MaxEdge.X, b.MaxEdge.Y, b.MaxEdge.Z);
		}

		inline static Irrlicht::Core::Matrix4 getNETMatrix(const irr::core::matrix4& mat)
		{
			Irrlicht::Core::Matrix4 nmat;
			for (int i=0; i<16; ++i)
				nmat.Members[i] = mat[i];

			return nmat;
		}

		inline static irr::core::matrix4 getNativeMatrix(Irrlicht::Core::Matrix4 mat)
		{
			irr::core::matrix4 nmat;
			for (int i=0; i<16; ++i)
				nmat[i] = mat.Members[i];

			return nmat;
		}

		inline static Irrlicht::Video::Material getNETMaterial(irr::video::SMaterial& material)
		{
			Irrlicht::Video::Material mat;
			mat.AmbientColor.color = material.AmbientColor.color;
			mat.DiffuseColor.color = material.DiffuseColor.color;
			mat.EmissiveColor.color = material.EmissiveColor.color;
			mat.Type = (Irrlicht::Video::MaterialType)material.MaterialType;
			mat.Texture1 = material.Textures[0] ? new Irrlicht::Video::ITexture(material.Textures[0]) : 0;
			mat.Texture2 = material.Textures[1] ? new Irrlicht::Video::ITexture(material.Textures[1]) : 0;
			mat.Texture3 = material.Textures[2] ? new Irrlicht::Video::ITexture(material.Textures[2]) : 0;
			mat.Texture4 = material.Textures[3] ? new Irrlicht::Video::ITexture(material.Textures[3]) : 0;

			for (int i=0; i<irr::video::EMF_MATERIAL_FLAG_COUNT; ++i)
				mat.Flags[i] = material.getFlag((irr::video::E_MATERIAL_FLAG)i);

			return mat;
		}

		inline static irr::video::SMaterial getNativeMaterial(Irrlicht::Video::Material& material)
		{
			irr::video::SMaterial mat;
			mat.AmbientColor.color = material.AmbientColor.color;
			mat.DiffuseColor.color = material.DiffuseColor.color;
			mat.EmissiveColor.color = material.EmissiveColor.color;
			mat.MaterialType = (irr::video::E_MATERIAL_TYPE)material.Type;
			mat.Textures[0] = material.Texture1 ? material.Texture1->get_NativeTexture() : 0;
			mat.Textures[1] = material.Texture2 ? material.Texture2->get_NativeTexture() : 0;
			mat.Textures[2] = material.Texture3 ? material.Texture3->get_NativeTexture() : 0;
			mat.Textures[3] = material.Texture4 ? material.Texture4->get_NativeTexture() : 0;

			for (int i=0; i<irr::video::EMF_MATERIAL_FLAG_COUNT; ++i)
				mat.setFlag( (irr::video::E_MATERIAL_FLAG)i, material.Flags[i] );

			return mat;
		}

		inline static Irrlicht::Core::Vector3D getNETVector(irr::core::vector3df v)
		{
			return Irrlicht::Core::Vector3D(v.X, v.Y, v.Z);
		}

		inline static irr::core::vector3df getNativeVector(Irrlicht::Core::Vector3D v)
		{
			return irr::core::vector3df(v.X, v.Y, v.Z);
		}

		inline static irr::SEvent getNativeEvent(Irrlicht::Event event)
		{
			// TODO: implement
			return irr::SEvent();
		}

		inline static Irrlicht::Scene::ViewFrustum getNETFrustum(const irr::scene::SViewFrustum& fr)
		{
			//TODO: implement
			return Irrlicht::Scene::ViewFrustum( fr );
		}

		inline static Irrlicht::Core::Dimension2Df getNETDim(irr::core::dimension2d<f32> dim)
		{
			return Irrlicht::Core::Dimension2Df(dim.Width, dim.Height);
		}

		inline static irr::core::dimension2d<f32> getNativeDim(Irrlicht::Core::Dimension2Df dim)
		{
			return irr::core::dimension2d<f32>(dim.Width, dim.Height);
		}

		inline static Irrlicht::Core::Dimension2D getNETDim(irr::core::dimension2d<s32> dim)
		{
			return Irrlicht::Core::Dimension2D(dim.Width, dim.Height);
		}

		inline static irr::core::dimension2d<s32> getNativeDim(Irrlicht::Core::Dimension2D dim)
		{
			return irr::core::dimension2d<s32>(dim.Width, dim.Height);
		}

		inline static irr::video::SColor getNativeColor( Irrlicht::Video::Color c )
		{
			return irr::video::SColor(c.color);
		}

		inline static Irrlicht::Video::Color getNETColor( irr::video::SColor c )
		{
			return Irrlicht::Video::Color(c.color);
		}


	};

}