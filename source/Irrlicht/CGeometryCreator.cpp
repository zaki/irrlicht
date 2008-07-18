// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGeometryCreator.h"
#include "SAnimatedMesh.h"
#include "SMeshBuffer.h"
#include "SMesh.h"
#include "IMesh.h"
#include "IVideoDriver.h"
#include "CImage.h"
#include "os.h"

namespace irr
{
namespace scene
{

// creates a hill plane
IMesh* CGeometryCreator::createHillPlaneMesh(
		const core::dimension2d<f32>& tileSize,
		const core::dimension2d<u32>& tc, video::SMaterial* material,
		f32 hillHeight, const core::dimension2d<f32>& ch,
		const core::dimension2d<f32>& textureRepeatCount)
{
	core::dimension2d<u32> tileCount = tc;
	core::dimension2d<f32> countHills = ch;

	if (countHills.Width < 0.01f)
		countHills.Width = 1.f;
	if (countHills.Height < 0.01f)
		countHills.Height = 1.f;

	// center
	const core::position2d<f32> center((tileSize.Width * tileCount.Width) * 0.5f, (tileSize.Height * tileCount.Height) * 0.5f);

	// texture coord step
	const core::dimension2d<f32> tx(
			textureRepeatCount.Width / tileCount.Width,
			textureRepeatCount.Height / tileCount.Height);

	// add one more point in each direction for proper tile count
	++tileCount.Height;
	++tileCount.Width;

	SMeshBuffer* buffer = new SMeshBuffer();
	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);

	// create vertices from left-front to right-back
	u32 x;

	f32 sx=0.f, tsx=0.f;
	for (x=0; x<tileCount.Width; ++x)
	{
		f32 sy=0.f, tsy=0.f;
		for (u32 y=0; y<tileCount.Height; ++y)
		{
			vtx.Pos.set(sx - center.X, 0, sy - center.Y);
			vtx.TCoords.set(tsx, 1.0f - tsy);

			if (hillHeight != 0.0f)
				vtx.Pos.Y = sinf(vtx.Pos.X * countHills.Width * core::PI / center.X) *
					cosf(vtx.Pos.Z * countHills.Height * core::PI / center.Y) *
					hillHeight;

			buffer->Vertices.push_back(vtx);
			sy += tileSize.Height;
			tsy += tx.Height;
		}
		sx += tileSize.Width;
		tsx += tx.Width;
	}

	// create indices

	for (x=0; x<tileCount.Width-1; ++x)
	{
		for (u32 y=0; y<tileCount.Height-1; ++y)
		{
			const s32 current = x*tileCount.Height + y;

			buffer->Indices.push_back(current);
			buffer->Indices.push_back(current + 1);
			buffer->Indices.push_back(current + tileCount.Height);

			buffer->Indices.push_back(current + 1);
			buffer->Indices.push_back(current + 1 + tileCount.Height);
			buffer->Indices.push_back(current + tileCount.Height);
		}
	}

	// recalculate normals
	for (u32 i=0; i<buffer->Indices.size(); i+=3)
	{
		const core::vector3df normal = core::plane3d<f32>(
			buffer->Vertices[buffer->Indices[i+0]].Pos,
			buffer->Vertices[buffer->Indices[i+1]].Pos,
			buffer->Vertices[buffer->Indices[i+2]].Pos).Normal;

		buffer->Vertices[buffer->Indices[i+0]].Normal = normal;
		buffer->Vertices[buffer->Indices[i+1]].Normal = normal;
		buffer->Vertices[buffer->Indices[i+2]].Normal = normal;
	}

	if (material)
		buffer->Material = *material;

	buffer->recalculateBoundingBox();

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();
	buffer->drop();
	return mesh;
}


IMesh* CGeometryCreator::createTerrainMesh(video::IImage* texture,
		video::IImage* heightmap, const core::dimension2d<f32>& stretchSize,
		f32 maxHeight, video::IVideoDriver* driver,
		const core::dimension2d<s32>& maxVtxBlockSize,
		bool debugBorders)
{
	if (!texture || !heightmap)
		return 0;

	// debug border
	const s32 borderSkip = debugBorders ? 0 : 1;

	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);

	SMesh* mesh = new SMesh();

	const u32 tm = os::Timer::getRealTime()/1000;
	const core::dimension2d<s32> hMapSize= heightmap->getDimension();
	const core::dimension2d<s32> tMapSize= texture->getDimension();
	const core::position2d<f32> thRel(static_cast<f32>(tMapSize.Width) / hMapSize.Width, static_cast<f32>(tMapSize.Height) / hMapSize.Height);
	maxHeight /= 255.0f; // height step per color value

	core::position2d<s32> processed(0,0);
	while (processed.Y<hMapSize.Height)
	{
		while(processed.X<hMapSize.Width)
		{
			core::dimension2d<s32> blockSize = maxVtxBlockSize;
			if (processed.X + blockSize.Width > hMapSize.Width)
				blockSize.Width = hMapSize.Width - processed.X;
			if (processed.Y + blockSize.Height > hMapSize.Height)
				blockSize.Height = hMapSize.Height - processed.Y;

			SMeshBuffer* buffer = new SMeshBuffer();
			buffer->setHardwareMappingHint(scene::EHM_STATIC);
			buffer->Vertices.reallocate(blockSize.getArea());
			// add vertices of vertex block
			s32 y;
			core::vector2df pos(0.f, processed.Y*stretchSize.Height);
			const core::vector2df bs(1.f/blockSize.Width, 1.f/blockSize.Height);
			core::vector2df tc(0.f, 0.5f*bs.Y);
			for (y=0; y<blockSize.Height; ++y)
			{
				pos.X=processed.X*stretchSize.Width;
				tc.X=0.5f*bs.X;
				for (s32 x=0; x<blockSize.Width; ++x)
				{
					const f32 height = heightmap->getPixel(x+processed.X, y+processed.Y).getAverage() * maxHeight;

					vtx.Pos.set(pos.X, height, pos.Y);
					vtx.TCoords.set(tc);
					buffer->Vertices.push_back(vtx);
					pos.X += stretchSize.Width;
					tc.X += bs.X;
				}
				pos.Y += stretchSize.Height;
				tc.Y += bs.Y;
			}

			buffer->Indices.reallocate((blockSize.Height-1)*(blockSize.Width-1)*6);
			// add indices of vertex block
			s32 c1 = 0;
			for (y=0; y<blockSize.Height-1; ++y)
			{
				for (s32 x=0; x<blockSize.Width-1; ++x)
				{
					const s32 c = c1 + x;

					buffer->Indices.push_back(c);
					buffer->Indices.push_back(c + blockSize.Width);
					buffer->Indices.push_back(c + 1);

					buffer->Indices.push_back(c + 1);
					buffer->Indices.push_back(c + blockSize.Width);
					buffer->Indices.push_back(c + 1 + blockSize.Width);
				}
				c1 += blockSize.Width;
			}

			// recalculate normals
			for (u32 i=0; i<buffer->Indices.size(); i+=3)
			{
				const core::vector3df normal = core::plane3d<f32>(
					buffer->Vertices[buffer->Indices[i+0]].Pos,
					buffer->Vertices[buffer->Indices[i+1]].Pos,
					buffer->Vertices[buffer->Indices[i+2]].Pos).Normal;

				buffer->Vertices[buffer->Indices[i+0]].Normal = normal;
				buffer->Vertices[buffer->Indices[i+1]].Normal = normal;
				buffer->Vertices[buffer->Indices[i+2]].Normal = normal;
			}

			if (buffer->Vertices.size())
			{
				c8 textureName[64];
				// create texture for this block
				video::IImage* img = new video::CImage(texture,
					core::position2d<s32>(core::floor32(processed.X*thRel.X), core::floor32(processed.Y*thRel.Y)),
					core::dimension2d<s32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y)));

				sprintf(textureName, "terrain%u_%u", tm, mesh->getMeshBufferCount());

				buffer->Material.setTexture(0, driver->addTexture(textureName, img));

				if (buffer->Material.getTexture(0))
				{
					c8 tmp[255];
					sprintf(tmp, "Generated terrain texture (%dx%d): %s",
						buffer->Material.getTexture(0)->getSize().Width,
						buffer->Material.getTexture(0)->getSize().Height,
						textureName);
					os::Printer::log(tmp);
				}
				else
					os::Printer::log("Could not create terrain texture.", textureName, ELL_ERROR);

				img->drop();
			}

			buffer->recalculateBoundingBox();
			mesh->addMeshBuffer(buffer);
			buffer->drop();

			// keep on processing
			processed.X += maxVtxBlockSize.Width - borderSkip;
		}

		// keep on processing
		processed.X = 0;
		processed.Y += maxVtxBlockSize.Height - borderSkip;
	}

	mesh->recalculateBoundingBox();
	return mesh;
}


/*
	a cylinder, a cone and a cross
	point up on (0,1.f, 0.f )
*/
IMesh* CGeometryCreator::createArrowMesh(const u32 tesselationCylinder,
						const u32 tesselationCone,
						const f32 height,
						const f32 cylinderHeight,
						const f32 width0,
						const f32 width1,
						const video::SColor vtxColor0,
						const video::SColor vtxColor1)
{
	SMesh* mesh = (SMesh*)createCylinderMesh(width0, cylinderHeight, tesselationCylinder, vtxColor0, false);

	IMesh* mesh2 = createConeMesh(width1, height-cylinderHeight, tesselationCone, vtxColor1, vtxColor0);
	for (u32 i=0; i<mesh2->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* buffer = mesh2->getMeshBuffer(i);
		for (u32 j=0; j<buffer->getVertexCount(); ++j)
			buffer->getPosition(j).Y += cylinderHeight;
		mesh->addMeshBuffer(buffer);
	}
	mesh2->drop();

	return mesh;
}


/* A sphere with proper normals and texture coords */
IMesh* CGeometryCreator::createSphereMesh(f32 radius, u32 polyCountX, u32 polyCountY)
{
	SMeshBuffer* buffer = new SMeshBuffer();

	// thanks to Alfaz93 who made his code available for Irrlicht on which
	// this one is based!

	// we are creating the sphere mesh here.

	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	if (polyCountX * polyCountY > 32767) // prevent u16 overflow
	{
		if (polyCountX > polyCountY) // prevent u16 overflow
			polyCountX = 32767/polyCountY-1;
		else
			polyCountY = 32767/(polyCountX+1);
	}

	u32 polyCountXPitch = polyCountX+1; // get to same vertex on next level
	buffer->Vertices.set_used((polyCountXPitch * polyCountY) + 2);
	buffer->Indices.set_used((polyCountX * polyCountY) * 6);

	const video::SColor clr(100, 255,255,255);

	u32 i=0;
	u32 level = 0;

	for (u32 p1 = 0; p1 < polyCountY-1; ++p1)
	{
		//main quads, top to bottom
		for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			const u32 curr = level + p2;
			buffer->Indices[i] = curr + polyCountXPitch;
			buffer->Indices[++i] = curr;
			buffer->Indices[++i] = curr + 1;
			buffer->Indices[++i] = curr + polyCountXPitch;
			buffer->Indices[++i] = curr+1;
			buffer->Indices[++i] = curr + 1 + polyCountXPitch;
			++i;
		}

		// the connectors from front to end
		buffer->Indices[i] = level + polyCountX - 1 + polyCountXPitch;
		buffer->Indices[++i] = level + polyCountX - 1;
		buffer->Indices[++i] = level + polyCountX;
		++i;

		buffer->Indices[i] = level + polyCountX - 1 + polyCountXPitch;
		buffer->Indices[++i] = level + polyCountX;
		buffer->Indices[++i] = level + polyCountX + polyCountXPitch;
		++i;
		level += polyCountXPitch;
	}

	const u32 polyCountSq = polyCountXPitch * polyCountY; // top point
	const u32 polyCountSq1 = polyCountSq + 1; // bottom point
	const u32 polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		buffer->Indices[i] = polyCountSq;
		buffer->Indices[++i] = p2 + 1;
		buffer->Indices[++i] = p2;
		++i;

		// create triangles which are at the bottom of the sphere

		buffer->Indices[i] = polyCountSqM1 + p2;
		buffer->Indices[++i] = polyCountSqM1 + p2 + 1;
		buffer->Indices[++i] = polyCountSq1;
		++i;
	}

	// create final triangle which is at the top of the sphere

	buffer->Indices[i] = polyCountSq;
	buffer->Indices[++i] = polyCountX;
	buffer->Indices[++i] = polyCountX-1;
	++i;

	// create final triangle which is at the bottom of the sphere

	buffer->Indices[i] = polyCountSqM1 + polyCountX - 1;
	buffer->Indices[++i] = polyCountSqM1;
	buffer->Indices[++i] = polyCountSq1;

	// calculate the angle which separates all points in a circle
	const f64 AngleX = 2 * core::PI / polyCountX;
	const f64 AngleY = core::PI / polyCountY;

	i = 0;
	f64 axz;

	// we don't start at 0.

	f64 ay = 0;//AngleY / 2;

	for (u32 y = 0; y < polyCountY; ++y)
	{
		ay += AngleY;
		const f64 sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position

			const core::vector3df pos(static_cast<f32>(radius * cos(axz) * sinay),
						static_cast<f32>(radius * cos(ay)),
						static_cast<f32>(radius * sin(axz) * sinay));
			// for spheres the normal is the position
			core::vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			f32 tu = 0.5f;
			if (y==0)
			{
				if (normal.Y != -1.0f && normal.Y != 1.0f)
					tu = static_cast<f32>(acos(core::clamp(normal.X/sinay, -1.0, 1.0)) * 0.5 *core::RECIPROCAL_PI64);
				if (normal.Z < 0.0f)
					tu=1-tu;
			}
			else
				tu = buffer->Vertices[i-polyCountXPitch].TCoords.X;
			buffer->Vertices[i] = video::S3DVertex(pos.X, pos.Y, pos.Z,
						normal.X, normal.Y, normal.Z,
						clr, tu,
						static_cast<f32>(ay*core::RECIPROCAL_PI64));
			++i;
			axz += AngleX;
		}
		// This is the doubled vertex on the initial position
		buffer->Vertices[i] = video::S3DVertex(buffer->Vertices[i-polyCountX]);
		buffer->Vertices[i].TCoords.X=1.0f;
		++i;
	}

	// the vertex at the top of the sphere
	buffer->Vertices[i] = video::S3DVertex(0.0f,radius,0.0f, 0.0f,1.0f,0.0f, clr, 0.5f, 0.0f);

	// the vertex at the bottom of the sphere
	++i;
	buffer->Vertices[i] = video::S3DVertex(0.0f,-radius,0.0f, 0.0f,-1.0f,0.0f, clr, 0.5f, 1.0f);

	// recalculate bounding box

	buffer->BoundingBox.reset(buffer->Vertices[i].Pos);
	buffer->BoundingBox.addInternalPoint(buffer->Vertices[i-1].Pos);
	buffer->BoundingBox.addInternalPoint(radius,0.0f,0.0f);
	buffer->BoundingBox.addInternalPoint(-radius,0.0f,0.0f);
	buffer->BoundingBox.addInternalPoint(0.0f,0.0f,radius);
	buffer->BoundingBox.addInternalPoint(0.0f,0.0f,-radius);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	buffer->drop();

	mesh->recalculateBoundingBox();
	return mesh;
}


/* A cylinder with proper normals and texture coords */
IMesh* CGeometryCreator::createCylinderMesh(f32 radius, f32 length, u32 tesselation, const video::SColor& color, bool closeTop, f32 oblique)
{
	SMeshBuffer* buffer = new SMeshBuffer();

	const f32 recTesselation = core::reciprocal((f32)tesselation);
	const f32 recTesselationHalf = recTesselation * 0.5f;
	const f32 angleStep = (core::PI * 2.f ) * recTesselation;
	const f32 angleStepHalf = angleStep*0.5f;

	u32 i;
	video::S3DVertex v;
	v.Color = color;
	buffer->Vertices.reallocate(tesselation*4+(closeTop?2:1));
	buffer->Indices.reallocate((tesselation*2)*(closeTop?12:9));
	f32 tcx = 0.f;
	for ( i = 0; i != tesselation; ++i )
	{
		const f32 angle = angleStep * i;
		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize();
		v.TCoords.X=tcx;
		v.TCoords.Y=0.f;
		buffer->Vertices.push_back(v);

		v.Pos.X += oblique;
		v.Pos.Y = length;
		v.Normal = v.Pos;
		v.Normal.normalize();
		v.TCoords.Y=1.f;
		buffer->Vertices.push_back(v);

		v.Pos.X = radius * cosf(angle + angleStepHalf);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle + angleStepHalf);
		v.Normal = v.Pos;
		v.Normal.normalize();
		v.TCoords.X=tcx+recTesselationHalf;
		v.TCoords.Y=0.f;
		buffer->Vertices.push_back(v);

		v.Pos.X += oblique;
		v.Pos.Y = length;
		v.Normal = v.Pos;
		v.Normal.normalize();
		v.TCoords.Y=1.f;
		buffer->Vertices.push_back(v);
		tcx += recTesselation;
	}

	const u32 nonWrappedSize = ( tesselation* 4 ) - 2;
	for ( i = 0; i != nonWrappedSize; i += 2 )
	{
		buffer->Indices.push_back ( i + 2 );
		buffer->Indices.push_back ( i + 0 );
		buffer->Indices.push_back ( i + 1 );

		buffer->Indices.push_back ( i + 2 );
		buffer->Indices.push_back ( i + 1 );
		buffer->Indices.push_back ( i + 3 );
	}

	buffer->Indices.push_back ( 0 );
	buffer->Indices.push_back ( i + 0 );
	buffer->Indices.push_back ( i + 1 );

	buffer->Indices.push_back ( 0 );
	buffer->Indices.push_back ( i + 1 );
	buffer->Indices.push_back ( 1 );

	// close down
	v.Pos.X = 0.f;
	v.Pos.Y = 0.f;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = -1.f;
	v.Normal.Z = 0.f;
	v.TCoords.X = 1.f;
	v.TCoords.Y = 1.f;
	buffer->Vertices.push_back ( v );

	u32 index = buffer->Vertices.size () - 1;

	for ( i = 0; i != nonWrappedSize; i += 2 )
	{
		buffer->Indices.push_back ( index );
		buffer->Indices.push_back ( i + 0 );
		buffer->Indices.push_back ( i + 2 );
	}

	buffer->Indices.push_back ( index );
	buffer->Indices.push_back ( i + 0 );
	buffer->Indices.push_back ( 0 );

	if (closeTop)
	{
		// close top
		v.Pos.X = oblique;
		v.Pos.Y = length;
		v.Pos.Z = 0.f;
		v.Normal.X = 0.f;
		v.Normal.Y = 1.f;
		v.Normal.Z = 0.f;
		v.TCoords.X = 0.f;
		v.TCoords.Y = 0.f;
		buffer->Vertices.push_back ( v );

		index = buffer->Vertices.size () - 1;

		for ( i = 0; i != nonWrappedSize; i += 2 )
		{
			buffer->Indices.push_back ( i + 1 );
			buffer->Indices.push_back ( index );
			buffer->Indices.push_back ( i + 3 );
		}

		buffer->Indices.push_back ( i + 1 );
		buffer->Indices.push_back ( index );
		buffer->Indices.push_back ( 1 );
	}

	buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();
	buffer->drop();
	return mesh;
}


/* A cone with proper normals and texture coords */
IMesh* CGeometryCreator::createConeMesh(f32 radius, f32 length, u32 tesselation, const video::SColor& colorTop, const video::SColor& colorBottom, f32 oblique)
{
	SMeshBuffer* buffer = new SMeshBuffer();

	const f32 angleStep = (core::PI * 2.f ) / tesselation;
	const f32 angleStepHalf = angleStep*0.5f;

	video::S3DVertex v;
	u32 i;

	v.Color = colorTop;
	for ( i = 0; i != tesselation; ++i )
	{
		f32 angle = angleStep * f32(i);

		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize();
		buffer->Vertices.push_back(v);

		angle += angleStepHalf;
		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize();
		buffer->Vertices.push_back(v);
	}
	const u32 nonWrappedSize = buffer->Vertices.size() - 1;

	// close top
	v.Pos.X = oblique;
	v.Pos.Y = length;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = 1.f;
	v.Normal.Z = 0.f;
	buffer->Vertices.push_back(v);

	u32 index = buffer->Vertices.size() - 1;

	for ( i = 0; i != nonWrappedSize; i += 1 )
	{
		buffer->Indices.push_back ( i + 0 );
		buffer->Indices.push_back ( index );
		buffer->Indices.push_back ( i + 1 );
	}

	buffer->Indices.push_back(i + 0);
	buffer->Indices.push_back(index);
	buffer->Indices.push_back(0);

	// close down
	v.Color = colorBottom;
	v.Pos.X = 0.f;
	v.Pos.Y = 0.f;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = -1.f;
	v.Normal.Z = 0.f;
	buffer->Vertices.push_back(v);

	index = buffer->Vertices.size() - 1;

	for ( i = 0; i != nonWrappedSize; i += 1 )
	{
		buffer->Indices.push_back(index);
		buffer->Indices.push_back(i + 0);
		buffer->Indices.push_back(i + 1);
	}

	buffer->Indices.push_back(index);
	buffer->Indices.push_back(i + 0);
	buffer->Indices.push_back(0);

	buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	buffer->drop();

	mesh->recalculateBoundingBox();
	return mesh;
}


} // end namespace scene
} // end namespace irr

