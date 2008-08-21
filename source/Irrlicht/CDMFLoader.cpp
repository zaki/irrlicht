// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
//
// This file was originally written by Salvatore Russo.
// I (Nikolaus Gebhardt) did some minor modifications and changes to it and
// integrated it into Irrlicht.
// Thanks a lot to Salvatore for his work on this and that he gave me
// his permission to add it into Irrlicht using the zlib license.
/*
  CDMFLoader by Salvatore Russo (September 2005)

  See the header file for additional information including use and distribution rights.
*/

#include "IrrCompileConfig.h" 
#ifdef _IRR_COMPILE_WITH_DMF_LOADER_

#include "CDMFLoader.h"
#include "ISceneManager.h"
#include "IAttributes.h"
#include "SAnimatedMesh.h"
#include "SMeshBufferLightMap.h"
#include "irrString.h"
#include "irrMath.h"
#include "dmfsupport.h"
#include "CImage.h"

namespace irr
{
namespace scene
{

/** Constructor*/
CDMFLoader::CDMFLoader(ISceneManager* smgr, io::IFileSystem* filesys)
: SceneMgr(smgr), FileSystem(filesys)
{
	#ifdef _DEBUG
	IReferenceCounted::setDebugName("CDMFLoader");
	#endif
}


/** Given first three points of a face, returns a face normal*/
void CDMFLoader::GetFaceNormal(	f32 a[3], //First point
				f32 b[3], //Second point
				f32 c[3], //Third point
				f32 out[3]) //Normal computed
{
	f32 v1[3], v2[3];

	v1[0] = a[0] - b[0];
	v1[1] = a[1] - b[1];
	v1[2] = a[2] - b[2];

	v2[0] = b[0] - c[0];
	v2[1] = b[1] - c[1];
	v2[2] = b[2] - c[2];

	out[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	out[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	out[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);

	f32 dist = (f32)sqrtf((out[0] * out[0]) + (out[1] * out[1]) + (out[2] * out[2]));

	if (dist == 0.0f)
		dist = 0.001f;

	out[0] /= dist;
	out[1] /= dist;
	out[2] /= dist;
}


/**Creates/loads an animated mesh from the file.
 \return Pointer to the created mesh. Returns 0 if loading failed.
 If you no longer need the mesh, you should call IAnimatedMesh::drop().
 See IReferenceCounted::drop() for more information.*/
IAnimatedMesh* CDMFLoader::createMesh(io::IReadFile* file)
{
	if (!file)
		return 0;
	video::IVideoDriver* driver = SceneMgr->getVideoDriver();

	//Load stringlist
	StringList dmfRawFile(file);

	if (dmfRawFile.size()==0)
		return 0;

	SMesh * mesh = new SMesh();

	u32 i;

	dmfHeader header;

	//load header
	if (GetDMFHeader(dmfRawFile, header))
	{
		//let's set ambient light
		SceneMgr->setAmbientLight(header.dmfAmbient);

		//let's create the correct number of materials, vertices and faces
		dmfMaterial *materiali=new dmfMaterial[header.numMaterials];
		dmfVert *verts=new dmfVert[header.numVertices];
		dmfFace *faces=new dmfFace[header.numFaces];

		//let's get the materials
		bool use_mat_dirs=false;
		use_mat_dirs=SceneMgr->getParameters()->getAttributeAsBool(DMF_USE_MATERIALS_DIRS);

		GetDMFMaterials(dmfRawFile , materiali,header.numMaterials,use_mat_dirs);

		//let's get vertices and faces
		GetDMFVerticesFaces(dmfRawFile, verts,faces);

		//create a meshbuffer for each material, then we'll remove empty ones
		for (i=0; i<header.numMaterials; i++)
		{
			//create a new SMeshBufferLightMap for each material
			SMeshBufferLightMap* buffer = new SMeshBufferLightMap();
			buffer->Material.MaterialType = video::EMT_LIGHTMAP_LIGHTING;
			buffer->Material.Wireframe = false;
			buffer->Material.Lighting = true;
			mesh->addMeshBuffer(buffer);
			buffer->drop();
		}

		// Build the mesh buffers
		for (i = 0; i < header.numFaces; i++)
		{
			if (faces[i].numVerts < 3)
				continue;

			f32 normal[3];

			GetFaceNormal(verts[faces[i].firstVert].pos,
					verts[faces[i].firstVert+1].pos, verts[faces[i].firstVert+2].pos, normal);

			SMeshBufferLightMap * meshBuffer = (SMeshBufferLightMap*)mesh->getMeshBuffer(
					faces[i].materialID);

			u32 base = meshBuffer->Vertices.size();

			// Add this face's verts
			u32 v;
			for (v = 0; v < faces[i].numVerts; v++)
			{
				dmfVert * vv = &verts[faces[i].firstVert + v];
				video::S3DVertex2TCoords vert(vv->pos[0], vv->pos[1], vv->pos[2],
					normal[0], normal[1], normal[2], video::SColor(0,255,255,255), 0.0f, 0.0f);
				if ( materiali[faces[i].materialID].textureBlend==4 &&
					SceneMgr->getParameters()->getAttributeAsBool(DMF_FLIP_ALPHA_TEXTURES))
				{
					vert.TCoords.set(vv->tc[0],-vv->tc[1]);
					vert.TCoords2.set(vv->lc[0],vv->lc[1]);
				}
				else
				{
					vert.TCoords.set(vv->tc[0], vv->tc[1]);
					vert.TCoords2.set(vv->lc[0], vv->lc[1]);
				}
				meshBuffer->Vertices.push_back(vert);
			}

			// Now add the indices
			// This weird loop turns convex polygons into triangle strips.
			// I do it this way instead of a simple fan because it usually
			// looks  a lot better in wireframe, for example.
			u32 h = faces[i].numVerts - 1, l = 0, c; // High, Low, Center
			for (v = 0; v < faces[i].numVerts - 2; v++)
			{
				if (v & 1)
					c = h - 1;
				else
					c = l + 1;

				meshBuffer->Indices.push_back(base + h);
				meshBuffer->Indices.push_back(base + l);
				meshBuffer->Indices.push_back(base + c);

				if (v & 1)
					h--;
				else
					l++;
			}
		}

		//load textures and lightmaps in materials.
		//don't worry if you receive a could not load texture, cause if you don't need
		//a particular material in your scene it will be loaded and then destroyed.
		for (i=0; i<header.numMaterials; i++)
		{
			core::stringc path;
			if ( SceneMgr->getParameters()->existsAttribute(DMF_TEXTURE_PATH) )
				path = SceneMgr->getParameters()->getAttributeAsString(DMF_TEXTURE_PATH);
			else
				path = FileSystem->getFileDir(file->getFileName());
			path += ('/');

			//texture and lightmap
			ITexture *tex = 0;
			ITexture *lig = 0;

			//current buffer to apply material
			SMeshBufferLightMap* buffer = (SMeshBufferLightMap*)mesh->getMeshBuffer(i);

			//Primary texture is normal
			if ((materiali[i].textureFlag==0) || (materiali[i].textureBlend==4))
				driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT,true);
			tex = driver->getTexture((path+materiali[i].textureName).c_str());

			//Primary texture is just a colour
			if(materiali[i].textureFlag==1)
			{
				String colour(materiali[i].textureName);
				String alpha,red,green,blue;

				alpha.append((char*)&colour[0]);
				alpha.append((char*)&colour[1]);
				blue.append((char*)&colour[2]);
				blue.append((char*)&colour[3]);
				green.append((char*)&colour[4]);
				green.append((char*)&colour[5]);
				red.append((char*)&colour[6]);
				red.append((char*)&colour[7]);

				SColor color(axtoi(alpha.c_str()),
					axtoi(red.c_str()),axtoi(green.c_str()),
					axtoi(blue.c_str()));

				//just for compatibility with older Irrlicht versions
				//to support transparent materials
				if (color.getAlpha()!=255 && materiali[i].textureBlend==4)
					driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT,true);

				CImage *immagine= new CImage(ECF_A8R8G8B8,
					core::dimension2d<s32>(8,8));
				immagine->fill(color);
				tex = driver->addTexture("", immagine);
				immagine->drop();

				//to support transparent materials
				if(color.getAlpha()!=255 && materiali[i].textureBlend==4)
				{
					buffer->Material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
					buffer->Material.MaterialTypeParam =(((f32) (color.getAlpha()-1))/255.0f);
				}
				immagine->drop();
			}

			//Lightmap is present
			if (materiali[i].lightmapFlag == 0)
				lig = driver->getTexture((path+materiali[i].lightmapName).c_str());
			else //no lightmap
			{
				lig = 0;
				buffer->Material.MaterialType = video::EMT_SOLID;
				const f32 mult = 100.0f - header.dmfShadow;
				buffer->Material.AmbientColor=header.dmfAmbient.getInterpolated(SColor(255,0,0,0),mult/100.f);
			}

			if(materiali[i].textureBlend==4)
			{
				buffer->Material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
				buffer->Material.MaterialTypeParam =SceneMgr->getParameters()->getAttributeAsFloat(DMF_ALPHA_CHANNEL_REF);
			}

			core::dimension2d<s32> texsize;
			core::dimension2d<s32> ligsize;

			if (tex && header.dmfVersion<1.1)
				texsize=tex->getSize();

			if (lig && header.dmfVersion<1.1)
				ligsize=lig->getSize();

			//if texture is present mirror vertically owing to DeleD rapresentation
			if (tex && header.dmfVersion<1.1)
			{
				void* pp = tex->lock();
				if (pp)
				{
					video::ECOLOR_FORMAT format = tex->getColorFormat();
					if (format == video::ECF_A1R5G5B5)
					{
						s16* p = (s16*)pp;
						s16 tmp=0;
						for (s32 x=0; x<texsize.Width; x++)
							for (s32 y=0; y<texsize.Height/2; y++)
							{
								tmp=p[y*texsize.Width + x];
								p[y*texsize.Width + x] = p[(texsize.Height-y-1)*texsize.Width + x];
								p[(texsize.Height-y-1)*texsize.Width + x]=tmp;
							}
					}
					else
					if (format == video::ECF_A8R8G8B8)
					{
						s32* p = (s32*)pp;
						s32 tmp=0;
						for (s32 x=0; x<texsize.Width; x++)
							for (s32 y=0; y<texsize.Height/2; y++)
							{
								tmp=p[y*texsize.Width + x];
								p[y*texsize.Width + x] = p[(texsize.Height-y-1)*texsize.Width + x];
								p[(texsize.Height-y-1)*texsize.Width + x]=tmp;
							}
					}
				}
			}

			if(tex && header.dmfVersion<1.1)
			{
				tex->unlock();
				tex->regenerateMipMapLevels();
			}

			//if lightmap is present mirror vertically owing to DeleD rapresentation
			if (lig && header.dmfVersion<1.1)
			{
				void* pp = lig->lock();
				if (pp)
				{
					video::ECOLOR_FORMAT format = lig->getColorFormat();
					if (format == video::ECF_A1R5G5B5)
					{
						s16* p = (s16*)pp;
						s16 tmp=0;
						for (s32 x=0; x<ligsize.Width; x++)
						{
							for (s32 y=0; y<ligsize.Height/2; y++)
							{
								tmp=p[y*ligsize.Width + x];
								p[y*ligsize.Width + x] = p[(ligsize.Height-y-1)*ligsize.Width + x];
								p[(ligsize.Height-y-1)*ligsize.Width + x]=tmp;
							}
						}
					}
					else if (format == video::ECF_A8R8G8B8)
					{
						s32* p = (s32*)pp;
						s32 tmp=0;
						for (s32 x=0; x<ligsize.Width; x++)
						{
							for (s32 y=0; y<ligsize.Height/2; y++)
							{
								tmp=p[y*ligsize.Width + x];
								p[y*ligsize.Width + x] = p[(ligsize.Height-y-1)*ligsize.Width + x];
								p[(ligsize.Height-y-1)*ligsize.Width + x]=tmp;
							}
						}
					}
				}
			}

			if (lig && header.dmfVersion<1.1)
			{
				lig->unlock();
				lig->regenerateMipMapLevels();
			}

			buffer->Material.setTexture(0, tex);
			buffer->Material.setTexture(1, lig);
		}

		delete verts;
		delete faces;
		delete materiali;
	}

	// delete all buffers without geometry in it.
	i = 0;
	while(i < mesh->MeshBuffers.size())
	{
		if (mesh->MeshBuffers[i]->getVertexCount() == 0 ||
			mesh->MeshBuffers[i]->getIndexCount() == 0 ||
			mesh->MeshBuffers[i]->getMaterial().getTexture(0) == 0)
		{
			// Meshbuffer is empty -- drop it
			mesh->MeshBuffers[i]->drop();
			mesh->MeshBuffers.erase(i);
		}
		else
		{
			i++;
		}
	}

	// create bounding box
	for (i = 0; i < mesh->MeshBuffers.size(); ++i)
	{
		mesh->MeshBuffers[i]->recalculateBoundingBox();
	}
	mesh->recalculateBoundingBox();

	// Set up an animated mesh to hold the mesh
	SAnimatedMesh* AMesh = new SAnimatedMesh();
	AMesh->Type = EAMT_UNKNOWN;
	AMesh->addMesh(mesh);
	AMesh->recalculateBoundingBox();
	mesh->drop();

	return AMesh;
}


/** \brief Tell us if this file is able to be loaded by this class
 based on the file extension (e.g. ".bsp")
 \return true if file is loadable.*/
bool CDMFLoader::isALoadableFileExtension(const c8* filename) const
{
	return strstr(filename, ".dmf") != 0;
}


} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_DMF_LOADER_

