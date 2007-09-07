// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h" 
#ifdef _IRR_COMPILE_WITH_OBJ_LOADER_

#include "COBJMeshFileLoader.h"
#include "SMeshBuffer.h"
#include "SAnimatedMesh.h"
#include "IReadFile.h"
#include "fast_atof.h"
#include "irrString.h"
#include "coreutil.h"

namespace irr
{
namespace scene
{

//! Constructor
COBJMeshFileLoader::COBJMeshFileLoader(io::IFileSystem* fs, video::IVideoDriver* driver)
: FileSystem(fs), Driver(driver), Mesh(0)
{
	if (FileSystem)
		FileSystem->grab();

	if (Driver)
		Driver->grab();
}



//! destructor
COBJMeshFileLoader::~COBJMeshFileLoader()
{
	if (FileSystem)
		FileSystem->drop();

	if (Driver)
		Driver->drop();

	if (Mesh)
		Mesh->drop();
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool COBJMeshFileLoader::isALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".obj")!=0;
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* COBJMeshFileLoader::createMesh(io::IReadFile* file)
{
	const u32 filesize = file->getSize();
	if (!filesize)
		return 0;

	const u32 WORD_BUFFER_LENGTH = 512;

	if (Mesh)
		Mesh->drop();
	Mesh = new SMesh();

	core::array<core::vector3df> vertexBuffer;
	core::array<core::vector2df> textureCoordBuffer;
	core::array<core::vector3df> normalsBuffer;
	SObjMtl * pCurrMtl = new SObjMtl();
	pCurrMtl->name="";
	materials.push_back(pCurrMtl);
	u32 smoothingGroup=0;

	// ********************************************************************
	// Patch to locate the file in the same folder as the .obj.
	// If you load the file as "data/some.obj" and mtllib contains
	// "mtlname test.mtl" (as usual), the loading will fail. Instead it
	// must look for data/test.tml. This patch does exactly that.
	//
	// patch by mandrav@codeblocks.org
	// ********************************************************************
	core::stringc obj_fullname = file->getFileName();
	core::stringc obj_relpath = "";
	s32 pathend = obj_fullname.findLast('/');
	if (pathend == -1)
		pathend = obj_fullname.findLast('\\');
	if (pathend != -1)
		obj_relpath = obj_fullname.subString(0, pathend + 1);
	// ********************************************************************
	// end of mtl folder patch
	// ********************************************************************

	c8* pBuf = new c8[filesize];
	memset(pBuf, 0, filesize);
	file->read((void*)pBuf, filesize);
	const c8* const pBufEnd = pBuf+filesize;

	// Process obj information
	const c8* pBufPtr = pBuf;
	while(pBufPtr != pBufEnd)
	{
		switch(pBufPtr[0])
		{
		case 'm':	// mtllib (material)
		{
			c8 name[WORD_BUFFER_LENGTH];
			pBufPtr = goAndCopyNextWord(name, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
			readMTL(name, obj_relpath);
		}
			break;

		case 'v':               // v, vn, vt
			switch(pBufPtr[1])
			{
			case ' ':          // vertex
				{
					core::vector3df vec;
					pBufPtr = readVec3(pBufPtr, vec, pBufEnd);
					vertexBuffer.push_back(vec);
				}
				break;

			case 'n':       // normal
				{
					core::vector3df vec;
					pBufPtr = readVec3(pBufPtr, vec, pBufEnd);
					normalsBuffer.push_back(vec);
				}
				break;

			case 't':       // texcoord
				{
					core::vector2df vec;
					pBufPtr = readVec2(pBufPtr, vec, pBufEnd);
					textureCoordBuffer.push_back(vec);
				}
				break;
			}
			break;

		case 'g': // group names skipped
			{
			}
			break;

		case 's': // smoothing can be a group or off (equiv. to 0)
			{
				c8 smooth[WORD_BUFFER_LENGTH];
				pBufPtr = goAndCopyNextWord(smooth, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
				if (core::stringc("off")==smooth)
					smoothingGroup=0;
				else
					smoothingGroup=core::strtol10(smooth, 0);
			}
			break;

		case 'u': // usemtl
			// get name of material
			{
				c8 matName[WORD_BUFFER_LENGTH];
				pBufPtr = goAndCopyNextWord(matName, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
				// retrieve the material
				SObjMtl *pUseMtl = findMtl(matName);
				// only change material if we found it
				if (pUseMtl)
					pCurrMtl = pUseMtl;
			}
			break;

		case 'f':               // face
		{
			c8 vertexWord[WORD_BUFFER_LENGTH]; // for retrieving vertex data
			video::S3DVertex v;
			u32 currentVertexCount = pCurrMtl->pMeshbuffer->Vertices.size();
			u32 facePointCount = 0;	// number of vertices in this face

			// Assign vertex color from currently active material's diffuse colour
			if (pCurrMtl)
				v.Color = pCurrMtl->pMeshbuffer->Material.DiffuseColor;

			// get all vertices data in this face (current line of obj file)
			const core::stringc wordBuffer = copyLine(pBufPtr, pBufEnd);
			const c8* pLinePtr = wordBuffer.c_str();
			const c8* const pEndPtr = pLinePtr+wordBuffer.size();

			// read in all vertices
			pLinePtr = goNextWord(pLinePtr, pEndPtr);
			while (0 != pLinePtr[0])
			{
				// Array to communicate with retrieveVertexIndices()
				// sends the buffer sizes and gets the actual indices
				// if index not set returns -1
				s32 Idx[3];
				Idx[1] = Idx[2] = -1;

				// read in next vertex's data
				u32 wlength = copyWord(vertexWord, pLinePtr, WORD_BUFFER_LENGTH, pBufEnd);
				// this function will also convert obj's 1-based index to c++'s 0-based index
				retrieveVertexIndices(vertexWord, Idx, vertexWord+wlength+1, vertexBuffer.size(), textureCoordBuffer.size(), normalsBuffer.size());
				v.Pos = vertexBuffer[Idx[0]];
				if ( -1 != Idx[1] )
					v.TCoords = textureCoordBuffer[Idx[1]];
				else
					v.TCoords.set(0.0f,0.0f);
				if ( -1 != Idx[2] )
					v.Normal = normalsBuffer[Idx[2]];
				else
					v.Normal.set(0.0f,0.0f,0.0f);
				pCurrMtl->pMeshbuffer->Vertices.push_back(v);
				++facePointCount;

				// go to next vertex
				pLinePtr = goNextWord(pLinePtr, pEndPtr);
			}

			// Add indices for first 3 vertices
			pCurrMtl->pMeshbuffer->Indices.push_back( currentVertexCount );
			pCurrMtl->pMeshbuffer->Indices.push_back( ( facePointCount - 1 ) + currentVertexCount );
			pCurrMtl->pMeshbuffer->Indices.push_back( ( facePointCount - 2 ) + currentVertexCount );
			// Add indices for subsequent vertices
			for ( u32 i = 0; i < facePointCount - 3; ++i )
			{
				pCurrMtl->pMeshbuffer->Indices.push_back( currentVertexCount );
				pCurrMtl->pMeshbuffer->Indices.push_back( ( facePointCount - 2 - i ) + currentVertexCount );
				pCurrMtl->pMeshbuffer->Indices.push_back( ( facePointCount - 3 - i ) + currentVertexCount );
			}
		}
		break;

		case '#': // comment 
		default:
			break;
		}	// end switch(pBufPtr[0])
		// eat up rest of line
		pBufPtr = goNextLine(pBufPtr, pBufEnd);
	}	// end while(pBufPtr && (pBufPtr-pBuf<filesize))

	// Combine all the groups (meshbuffers) into the mesh
	for ( u32 m = 0; m < materials.size(); ++m )
	{
		if ( materials[m]->pMeshbuffer->getIndexCount() > 0 )
		{
			materials[m]->pMeshbuffer->recalculateBoundingBox();
			Mesh->addMeshBuffer( materials[m]->pMeshbuffer );
		}
	}

	// Create the Animated mesh if there's anything in the mesh
	SAnimatedMesh* pAM = 0;
	if ( 0 != Mesh->getMeshBufferCount() )
	{
		Mesh->recalculateBoundingBox();
		pAM = new SAnimatedMesh();
		pAM->Type = EAMT_OBJ;
		pAM->addMesh(Mesh);
		pAM->recalculateBoundingBox();
	}

	// Clean up the allocate obj file contents
	delete [] pBuf;
	// more cleaning up
	cleanUp();
	Mesh->drop();
	Mesh = 0;

	return pAM;
}


void COBJMeshFileLoader::readMTL(const c8* pFileName, core::stringc relPath)
{
	const u32 WORD_BUFFER_LENGTH = 512;

	io::IReadFile * pMtlReader;
	if (FileSystem->existFile(pFileName))
		pMtlReader = FileSystem->createAndOpenFile(pFileName);
	else
		// try to read in the relative path, the .obj is loaded from
		pMtlReader = FileSystem->createAndOpenFile((relPath + pFileName).c_str());
	if (!pMtlReader)	// fail to open and read file
		return;

	const u32 filesize = pMtlReader->getSize();
	if (!filesize)
		return;

	c8* pBuf = new c8[filesize];
	pMtlReader->read((void*)pBuf, filesize);
	const c8* pBufEnd = pBuf+filesize;

	SObjMtl* pCurrMaterial = 0;

	const c8* pBufPtr = pBuf;
	while(pBufPtr != pBufEnd)
	{
		switch(*pBufPtr)
		{
			case 'n': // newmtl
			{
				// if there's an existing material, store it first
				if ( pCurrMaterial )
					materials.push_back( pCurrMaterial );

				// extract new material's name
				c8 mtlNameBuf[WORD_BUFFER_LENGTH];
				pBufPtr = goAndCopyNextWord(mtlNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);

				pCurrMaterial = new SObjMtl;
				pCurrMaterial->name = mtlNameBuf;
			}
			break;
			case 'i': // illum - illumination
			if ( pCurrMaterial )
			{
				const u32 COLOR_BUFFER_LENGTH = 16;
				c8 illumStr[COLOR_BUFFER_LENGTH];

				pBufPtr = goAndCopyNextWord(illumStr, pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);
				pCurrMaterial->illumination = (c8)atol(illumStr);
			}
			break;
			case 'N': // Ns - shininess
			if ( pCurrMaterial )
			{
				const u32 COLOR_BUFFER_LENGTH = 16;
				c8 nsStr[COLOR_BUFFER_LENGTH];

				pBufPtr = goAndCopyNextWord(nsStr, pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);
				f32 shininessValue = core::fast_atof(nsStr);

				// wavefront shininess is from [0, 1000], so scale for OpenGL
				shininessValue *= 0.128f;
				pCurrMaterial->pMeshbuffer->Material.Shininess = shininessValue;
			}
			break;
			case 'K':
			if ( pCurrMaterial )
			{
				switch(pBufPtr[1])
				{
				case 'd':		// Kd = diffuse
					{
						pBufPtr = readColor(pBufPtr, pCurrMaterial->pMeshbuffer->Material.DiffuseColor, pBufEnd);

					}
					break;

				case 's':		// Ks = specular
					{
						pBufPtr = readColor(pBufPtr, pCurrMaterial->pMeshbuffer->Material.SpecularColor, pBufEnd);
					}
					break;

				case 'a':		// Ka = ambience
					{
						pBufPtr=readColor(pBufPtr, pCurrMaterial->pMeshbuffer->Material.AmbientColor, pBufEnd);
					}
					break;
				case 'e':		// Ke = emissive
					{
						pBufPtr=readColor(pBufPtr, pCurrMaterial->pMeshbuffer->Material.EmissiveColor, pBufEnd);
					}
					break;
				}	// end switch(pBufPtr[1])
			}	// end case 'K': if ( 0 != pCurrMaterial )...
			break;
			case 'm': // texture maps
			if (pCurrMaterial)
			{
				u8 type=0; // map_Kd - diffuse texture map
				if (!strncmp(pBufPtr,"map_bump",8))
					type=1;
				else if (!strncmp(pBufPtr,"map_d",5))
					type=2;
				else if (!strncmp(pBufPtr,"map_refl",8))
					type=3;
				// extract new material's name
				c8 textureNameBuf[WORD_BUFFER_LENGTH];
				pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
				// handle options
				while (textureNameBuf[0]=='-')
				{
					if (!strncmp(pBufPtr,"-blendu",7))
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
					if (!strncmp(pBufPtr,"-blendv",7))
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
					if (!strncmp(pBufPtr,"-cc",3))
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
					if (!strncmp(pBufPtr,"-clamp",6))
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
					if (!strncmp(pBufPtr,"-texres",7))
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
					if (!strncmp(pBufPtr,"-mm",3))
					{
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
					}
					if (!strncmp(pBufPtr,"-o",2))
					{
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						// next parameters are optional, so skip rest of loop if no number is found
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						if (!core::isdigit(textureNameBuf[0]))
							continue;
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						if (!core::isdigit(textureNameBuf[0]))
							continue;
					}
					if (!strncmp(pBufPtr,"-s",2))
					{
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						// next parameters are optional, so skip rest of loop if no number is found
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						if (!core::isdigit(textureNameBuf[0]))
							continue;
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						if (!core::isdigit(textureNameBuf[0]))
							continue;
					}
					if (!strncmp(pBufPtr,"-t",2))
					{
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						// next parameters are optional, so skip rest of loop if no number is found
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						if (!core::isdigit(textureNameBuf[0]))
							continue;
						pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
						if (!core::isdigit(textureNameBuf[0]))
							continue;
					}
					// get next word
					pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
				}
				if (type==1)
				{
					pCurrMaterial->pMeshbuffer->Material.MaterialTypeParam=core::fast_atof(textureNameBuf);
					pBufPtr = goAndCopyNextWord(textureNameBuf, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
				}

				video::ITexture * pTexture;
				if (FileSystem->existFile(textureNameBuf))
					pTexture = Driver->getTexture( textureNameBuf );
				else
					// try to read in the relative path, the .obj is loaded from
					pTexture = Driver->getTexture( (relPath + textureNameBuf).c_str() );
				if ( pTexture )
				{
					if (type==0)
						pCurrMaterial->pMeshbuffer->Material.Textures[0] = pTexture;
					else if (type==1)
					{
						Driver->makeNormalMapTexture(pTexture);
						pCurrMaterial->pMeshbuffer->Material.Textures[1] = pTexture;
						pCurrMaterial->pMeshbuffer->Material.MaterialType=video::EMT_PARALLAX_MAP_SOLID;
					}
					else if (type==2)
					{
						pCurrMaterial->pMeshbuffer->Material.Textures[0] = pTexture;
						pCurrMaterial->pMeshbuffer->Material.MaterialType=video::EMT_TRANSPARENT_ADD_COLOR;
					}
					else if (type==3)
					{
//						pCurrMaterial->pMeshbuffer->Material.Textures[1] = pTexture;
//						pCurrMaterial->pMeshbuffer->Material.MaterialType=video::EMT_REFLECTION_2_LAYER;
					}
					// Set diffuse material colour to white so as not to affect texture colour
					// Because Maya set diffuse colour Kd to black when you use a diffuse colour map
					// But is this the right thing to do?
					pCurrMaterial->pMeshbuffer->Material.DiffuseColor.set(
						pCurrMaterial->pMeshbuffer->Material.DiffuseColor.getAlpha(), 255, 255, 255 );
				}
			}
			break;
			case 'd': // d - transparency
			if ( pCurrMaterial )
			{
				const u32 COLOR_BUFFER_LENGTH = 16;
				c8 dStr[COLOR_BUFFER_LENGTH];

				pBufPtr = goAndCopyNextWord(dStr, pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);
				f32 dValue = core::fast_atof(dStr);

				pCurrMaterial->pMeshbuffer->Material.DiffuseColor.setAlpha( (s32)(dValue * 255) );
				if (dValue<1.0f)
					pCurrMaterial->pMeshbuffer->Material.MaterialType = irr::video::EMT_TRANSPARENT_VERTEX_ALPHA;
			}
			break;
			case 'T':
			if ( pCurrMaterial )
			{
				switch ( pBufPtr[1] )
				{
				case 'f':		// Tf - Transmitivity
					const u32 COLOR_BUFFER_LENGTH = 16;
					c8 redStr[COLOR_BUFFER_LENGTH];
					c8 greenStr[COLOR_BUFFER_LENGTH];
					c8 blueStr[COLOR_BUFFER_LENGTH];

					pBufPtr = goAndCopyNextWord(redStr,   pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);
					pBufPtr = goAndCopyNextWord(greenStr, pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);
					pBufPtr = goAndCopyNextWord(blueStr,  pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);

					f32 transparency = ( core::fast_atof(redStr) + core::fast_atof(greenStr) + core::fast_atof(blueStr) ) / 3;

					pCurrMaterial->pMeshbuffer->Material.DiffuseColor.setAlpha( (s32)(transparency * 255) );
					if (transparency < 1.0f)
						pCurrMaterial->pMeshbuffer->Material.MaterialType = irr::video::EMT_TRANSPARENT_VERTEX_ALPHA;
				}
			}
			break;
			default: // comments or not recognised
			break;
		} // end switch(pBufPtr[0])
		// go to next line
		pBufPtr = goNextLine(pBufPtr, pBufEnd);
	}	// end while (pBufPtr)

	// end of file. if there's an existing material, store it
	if ( pCurrMaterial )
	{
		materials.push_back( pCurrMaterial );
		pCurrMaterial = 0;
	}

	delete [] pBuf;
	pMtlReader->drop();
}

//! Read RGB color
const c8* COBJMeshFileLoader::readColor(const c8* pBufPtr, video::SColor& color, const c8* const pBufEnd)
{
	const u32 COLOR_BUFFER_LENGTH = 16;
	c8 colStr[COLOR_BUFFER_LENGTH];

	color.setAlpha(255);
	pBufPtr = goAndCopyNextWord(colStr, pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);
	color.setRed((s32)(core::fast_atof(colStr) * 255.0f));
	pBufPtr = goAndCopyNextWord(colStr,   pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);
	color.setGreen((s32)(core::fast_atof(colStr) * 255.0f));
	pBufPtr = goAndCopyNextWord(colStr,   pBufPtr, COLOR_BUFFER_LENGTH, pBufEnd);
	color.setBlue((s32)(core::fast_atof(colStr) * 255.0f));
	return pBufPtr;
}


//! Read 3d vector of floats
const c8* COBJMeshFileLoader::readVec3(const c8* pBufPtr, core::vector3df& vec, const c8* const pBufEnd)
{
	const u32 WORD_BUFFER_LENGTH = 256;
	c8 wordBuffer[WORD_BUFFER_LENGTH];

	pBufPtr = goAndCopyNextWord(wordBuffer, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
	vec.X=-core::fast_atof(wordBuffer); // change handedness
	pBufPtr = goAndCopyNextWord(wordBuffer, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
	vec.Y=core::fast_atof(wordBuffer);
	pBufPtr = goAndCopyNextWord(wordBuffer, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
	vec.Z=core::fast_atof(wordBuffer);
	return pBufPtr;
}


//! Read 2d vector of floats
const c8* COBJMeshFileLoader::readVec2(const c8* pBufPtr, core::vector2df& vec, const c8* const pBufEnd)
{
	const u32 WORD_BUFFER_LENGTH = 256;
	c8 wordBuffer[WORD_BUFFER_LENGTH];

	pBufPtr = goAndCopyNextWord(wordBuffer, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
	vec.X=core::fast_atof(wordBuffer);
	pBufPtr = goAndCopyNextWord(wordBuffer, pBufPtr, WORD_BUFFER_LENGTH, pBufEnd);
	vec.Y=-core::fast_atof(wordBuffer); // change handedness
	return pBufPtr;
}


//! Read boolean value represented as 'on' or 'off'
const c8* COBJMeshFileLoader::readBool(const c8* pBufPtr, bool& tf, const c8* const pBufEnd)
{
	const u32 BUFFER_LENGTH = 8;
	c8 tfStr[BUFFER_LENGTH];

	pBufPtr = goAndCopyNextWord(tfStr, pBufPtr, BUFFER_LENGTH, pBufEnd);
	tf = strcmp(tfStr, "off") != 0;
	return pBufPtr;
}


COBJMeshFileLoader::SObjMtl* COBJMeshFileLoader::findMtl(const c8* pMtlName)
{
	for (u32 i = 0; i < materials.size(); ++i)
	{
		if ( materials[i]->name == pMtlName )
			return materials[i];
	}
	return 0;
}



//! skip space characters and stop on first non-space
const c8* COBJMeshFileLoader::goFirstWord(const c8* buf, const c8* const pBufEnd)
{
	// skip space characters
	while((buf != pBufEnd) && core::isspace(*buf))
		++buf;

	return buf;
}


//! skip current word and stop at beginning of next one
const c8* COBJMeshFileLoader::goNextWord(const c8* buf, const c8* const pBufEnd)
{
	// skip current word
	while(( buf != pBufEnd ) && !core::isspace(*buf))
		++buf;

	return goFirstWord(buf, pBufEnd);
}


//! Read until line break is reached and stop at the next non-space character
const c8* COBJMeshFileLoader::goNextLine(const c8* buf, const c8* const pBufEnd)
{
	// look for newline characters
	while(buf != pBufEnd)
	{
		// found it, so leave
		if (*buf=='\n' || *buf=='\r')
			break;
		++buf;
	}
	return goFirstWord(buf, pBufEnd);
}


u32 COBJMeshFileLoader::copyWord(c8* outBuf, const c8* const inBuf, u32 outBufLength, const c8* const pBufEnd)
{
	if (!outBufLength)
		return 0;
	if (!inBuf)
	{
		*outBuf = 0;
		return 0;
	}

	u32 i = 0;
	while(inBuf[i])
	{
		if (core::isspace(inBuf[i]) || &(inBuf[i]) == pBufEnd)
			break;
		++i;
	}

	u32 length = core::min_(i, outBufLength-1);
	for (u32 j=0; j<length; ++j)
		outBuf[j] = inBuf[j];

	outBuf[i] = 0;
	return length;
}


core::stringc COBJMeshFileLoader::copyLine(const c8* inBuf, const c8* pBufEnd)
{
	if (!inBuf)
		return core::stringc();

	const c8* ptr = inBuf;
	while (ptr<pBufEnd)
	{
		if (*ptr=='\n' || *ptr=='\r')
			break;
		++ptr;
	}
	return core::stringc(inBuf, ptr-inBuf+1);
}


const c8* COBJMeshFileLoader::goAndCopyNextWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* pBufEnd)
{
	inBuf = goNextWord(inBuf, pBufEnd);
	copyWord(outBuf, inBuf, outBufLength, pBufEnd);
	return inBuf;
}


bool COBJMeshFileLoader::retrieveVertexIndices(c8* pVertexData, s32* pIdx, const c8* pBufEnd, u32 vbsize, u32 vtsize, u32 vnsize)
{
	c8 word[16] = "";
	const c8* pChar = goFirstWord(pVertexData, pBufEnd);
	u32 idxType = 0;	// 0 = posIdx, 1 = texcoordIdx, 2 = normalIdx

	u32 i = 0;
	while ( pChar != pBufEnd )
	{
		if ( ( core::isdigit(*pChar)) || (*pChar == '-') )
		{
			// build up the number
			word[i++] = *pChar;
		}
		else if ( *pChar == '/' || *pChar == ' ' || *pChar == '\0' )
		{
			// number is completed. Convert and store it
			word[i] = '\0';
			// if no number was found index will become 0 and later on -1 by decrement
			if (word[0]=='-')
			{
				pIdx[idxType] = -core::strtol10(word+1,0);
				switch (idxType)
				{
					case 0:
						pIdx[idxType] += vbsize;
						break;
					case 1:
						pIdx[idxType] += vtsize;
						break;
					case 2:
						pIdx[idxType] += vnsize;
						break;
				}
			}
			else
				pIdx[idxType] = core::strtol10(word,0)-1;

			// reset the word
			word[0] = '\0';
			i = 0;

			// go to the next kind of index type
			if (*pChar == '/')
			{
				if ( ++idxType > 2 )
				{
					// error checking, shouldn't reach here unless file is wrong
					idxType = 0;
				}
			}
			else
			{
				// set all missing values to disable (=-1)
				while (++idxType < 3)
					pIdx[idxType]=-1;
				++pChar;
				break; // while
			}
		}

		// go to the next char
		++pChar;
	}

	return true;
}


void COBJMeshFileLoader::cleanUp()
{
	u32 i;

	for (i = 0; i < materials.size(); ++i )
	{
		materials[i]->pMeshbuffer->drop();
		delete materials[i];
	}

	materials.clear();
}


} // end namespace scene
} // end namespace irr


#endif // _IRR_COMPILE_WITH_OBJ_LOADER_
