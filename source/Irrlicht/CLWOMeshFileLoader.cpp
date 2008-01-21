#include "CLWOMeshFileLoader.h"
#include <cstring>
#include "os.h"
#include "SAnimatedMesh.h"
#include "SMesh.h"
#include "IReadFile.h"

using namespace std;

namespace irr
{
namespace scene
{

#define LWO_READER_DEBUG

#define charsToUIntD(a, b, c, d) ((a << 24) | (b << 16) | (c << 8) | d)
inline unsigned int charsToUInt(const char *str)
{
	return (str[0] << 24) | (str[1] << 16) | (str[2] << 8) | str[3];
}


struct tLWOTextureInfo
{
	tLWOTextureInfo() : Flags(0), WidthWrap(2), HeightWrap(2),
			Value(0.0f), AntiAliasing(1.0f), Opacity(1.0f) {};
	core::stringc Type;
	core::stringc Map;
	core::stringc AlphaMap;
	u16 Flags;
	bool Active;
	u16 WidthWrap;
	u16 HeightWrap;
	core::vector3df Size;
	core::vector3df Center;
	core::vector3df Falloff;
	core::vector3df Velocity;
	video::SColor Color;
	f32 Value;
	f32 AntiAliasing;
	f32 Opacity;
	u16 OpacType;
	f32 FParam[3];
	u16 IParam[3];
};

struct CLWOMeshFileLoader::tLWOMaterial
{
	tLWOMaterial() : Luminance(0.0f),Diffuse(1.0f),Specular(0.0f),Reflection(0.0f),Transparency(0.0f),Translucency(0.0f),Sharpness(0.0f),ReflMode(3),ReflSeamAngle(0.0f),ReflBlur(0.0f),RefrIndex(1.0f),TranspMode(3),TranspBlur(0.0f) {};

	core::stringc Name;
	scene::SMeshBuffer *Meshbuffer;
	u16 Flags;
	f32 Luminance;
	f32 Diffuse;
	f32 Specular;
	f32 Reflection;
	f32 Transparency;
	f32 Translucency;
	f32 Sharpness;
	u16 ReflMode;
	core::stringc ReflMap;
	f32 ReflSeamAngle;
	f32 ReflBlur;
	f32 RefrIndex;
	u16 TranspMode;
	f32 TranspBlur;
	f32 EdgeTransparency;
	f32 SmoothingAngle;
	tLWOTextureInfo Texture[7];
	u32 Envelope[18];
};

struct tLWOLayerInfo
{
	u16 Number;
	u16 Parent;
	u16 Flags;
	bool Active;
	core::stringc Name;
	core::vector3df Pivot;
};


//! Constructor
CLWOMeshFileLoader::CLWOMeshFileLoader(video::IVideoDriver* driver)
: Driver(driver), File(0), Mesh(0)
{
	if (Driver)
		Driver->grab();
}



//! destructor
CLWOMeshFileLoader::~CLWOMeshFileLoader()
{
	if (Driver)
		Driver->drop();

	if (Mesh)
		Mesh->drop();
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CLWOMeshFileLoader::isALoadableFileExtension(const c8* filename) const
{
	return strstr(filename, ".lwo")!=0;
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* CLWOMeshFileLoader::createMesh(io::IReadFile* file)
{
	File = file;
	Points.clear();
	Materials.clear();

	if (Mesh)
		Mesh->drop();

	Mesh = new SMesh();

	if (!readFileHeader())
		return false;

	if (!readChunks())
		return false;

	SAnimatedMesh* am = new SAnimatedMesh();
	am->Type = EAMT_3DS;

	for (u32 i=0; i<Materials.size(); ++i)
	{
		for (u32 j=0; j<Materials[i]->Meshbuffer->Vertices.size(); ++j)
			Materials[i]->Meshbuffer->Vertices[j].Color=Materials[i]->Meshbuffer->Material.DiffuseColor;
		Materials[i]->Meshbuffer->recalculateBoundingBox();
		Mesh->addMeshBuffer(Materials[i]->Meshbuffer);
		Materials[i]->Meshbuffer->drop();
	}

	Mesh->recalculateBoundingBox();

	am->addMesh(Mesh);
	am->recalculateBoundingBox();
	Mesh->drop();
	Mesh = 0;
	Points.clear();
	Materials.clear();

	return am;
}


bool CLWOMeshFileLoader::readChunks()
{
	s32 lastPos;
	u32 size;
	unsigned int uiType;
	char type[5];
	type[4]=0;
	tLWOLayerInfo layer;

	while(File->getPos()<File->getSize())
	{
		File->read(&type, 4);
		//Convert 4-char string to 4-byte integer
		//Makes it possible to do a switch statement
		uiType = charsToUInt(type);
		File->read(&size, 4);
#ifndef __BIG_ENDIAN__
		size=os::Byteswap::byteswap(size);
#endif
		lastPos=File->getPos();

		switch(uiType)
		{
			case charsToUIntD('L','A','Y','R'):
				{
#ifdef LWO_READER_DEBUG
					os::Printer::log("LWO loader: loading layer.");
#endif
					u16 tmp16;
					File->read(&tmp16, 2); // number
					File->read(&tmp16, 2); // flags
					size -= 4;
#ifndef __BIG_ENDIAN__
					tmp16=os::Byteswap::byteswap(tmp16);
#endif
					if (((FormatVersion==1)&&(tmp16!=1)) ||
						((FormatVersion==2)&&(tmp16&1)))
						layer.Active=false;
					else
						layer.Active=true;
					if (FormatVersion==2)
						size -= readVec(layer.Pivot);
					size -= readString(layer.Name);
					if (size)
					{
						File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
						tmp16=os::Byteswap::byteswap(tmp16);
#endif
						layer.Parent = tmp16;
					}
				}
				break;
			case charsToUIntD('P','N','T','S'):
				{
#ifdef LWO_READER_DEBUG
					os::Printer::log("LWO loader: loading points.");
#endif
					core::vector3df vec;
					Points.clear();
					const u32 tmpsize = size/12;
					Points.reallocate(tmpsize);
					for (u32 i=0; i<tmpsize; ++i)
					{
						readVec(vec);
						Points.push_back(vec);
					}
				}
				break;
			case charsToUIntD('V','M','A','P'):
				readVertexMapping(size);
				break;
			case charsToUIntD('P','O','L','S'):
			case charsToUIntD('P','T','C','H'): // TODO: should be a subdivison mesh
				if (FormatVersion!=2)
					readObj1(size);
				else
					readObj2(size);
				break;
			case charsToUIntD('T','A','G','S'):
			case charsToUIntD('S','R','F','S'):
				{
#ifdef LWO_READER_DEBUG
					os::Printer::log("LWO loader: loading surface names.");
#endif
					while (size!=0)
					{
						tLWOMaterial *mat=new tLWOMaterial();
						mat->Name="";
						mat->Meshbuffer=new scene::SMeshBuffer();
						size -= readString(mat->Name);
						Materials.push_back(mat);
					}
				}
				break;
			case charsToUIntD('P','T','A','G'):
				readTagMapping(size);
				break;
//			case charsToUIntD('V','M','A','D'): // dicontinuous vertex mapping, i.e. additional texcoords
//			case charsToUIntD('V','M','P','A'):
//			case charsToUIntD('E','N','V','L'):
//				break;
			case charsToUIntD('C','L','I','P'):
				{
#ifdef LWO_READER_DEBUG
					os::Printer::log("LWO loader: loading clips.");
#endif
					u32 index;
					u16 subsize;
					File->read(&index, 4);
#ifndef __BIG_ENDIAN__
					index=os::Byteswap::byteswap(index);
#endif
					File->read(&type, 4);
					File->read(&subsize, 2);
#ifndef __BIG_ENDIAN__
					subsize=os::Byteswap::byteswap(subsize);
#endif
					size -= 10;
					if (strncmp(type, "STIL", 4))
					{
						File->seek(size, true);
						break;
					}
					core::stringc path;
					size -= readString(path);
					Images.push_back(path);
				}
				break;
			case charsToUIntD('S','U','R','F'):
				readMat(size);
				break;
			case charsToUIntD('B','B','O','X'):
				{
#ifdef LWO_READER_DEBUG
					os::Printer::log("LWO loader: loading bbox.");
#endif
					// not stored
					core::vector3df vec;
					for (u32 i=0; i<2; ++i)
						readVec(vec);
					size -= 24;
				}
				break;
			case charsToUIntD('D','E','S','C'):
			case charsToUIntD('T','E','X','T'):
				{
#ifdef LWO_READER_DEBUG
					os::Printer::log("LWO loader: loading text.");
#endif
					core::stringc text;
					size -= readString(text);
				}
				break;
			// not needed
			case charsToUIntD('I','C','O','N'):
			// not yet supported
			case charsToUIntD('P','C','H','S'):
			case charsToUIntD('C','R','V','S'):
			default:
#ifdef LWO_READER_DEBUG
				os::Printer::log("LWO loader: skipping ", type);
#endif
				//Go to next chunk
				File->seek(lastPos + size, false);
				break;
		}
	}
	return true;
}

void CLWOMeshFileLoader::readObj1(u32 size)
{
#ifdef LWO_READER_DEBUG
	os::Printer::log("LWO loader: loading polygons (old format).");
#endif
	u32 pos;
	u16 numVerts, vertIndex;
	s16 material;
	video::S3DVertex vertex;

	while (size!=0)
	{
		File->read(&numVerts, 2);
#ifndef __BIG_ENDIAN__
		numVerts=os::Byteswap::byteswap(numVerts);
#endif
		pos=File->getPos();
		File->seek(2*numVerts, true);
		File->read(&material, 2);
#ifndef __BIG_ENDIAN__
		material=os::Byteswap::byteswap(material);
#endif
		size -=2*numVerts+4;
		// detail meshes ?
		scene::SMeshBuffer *mb;
		if (material<0)
			mb=Materials[-material-1]->Meshbuffer;
		else
			mb=Materials[material-1]->Meshbuffer;
		File->seek(pos, false);

		u16 vertCount=mb->Vertices.size();
		for (u16 i=0; i<numVerts; ++i)
		{
			File->read(&vertIndex, 2);
#ifndef __BIG_ENDIAN__
			vertIndex=os::Byteswap::byteswap(vertIndex);
#endif
			vertex.Pos=Points[vertIndex];
			mb->Vertices.push_back(vertex);
		}
		for (u16 i=1; i<numVerts-1; ++i)
		{
			core::vector3df normal = core::plane3df(mb->Vertices[vertCount].Pos,mb->Vertices[vertCount+i].Pos,mb->Vertices[vertCount+i+1].Pos).Normal.normalize();
			mb->Vertices[vertCount].Normal=normal;
			mb->Vertices[vertCount+i].Normal=normal;
			mb->Vertices[vertCount+i+1].Normal=normal;
			mb->Indices.push_back(vertCount);
			mb->Indices.push_back(vertCount+i);
			mb->Indices.push_back(vertCount+i+1);
		}
		// skip material number and detail surface count
		if (material<0)
			File->read(&material, 2);
		File->read(&material, 2);
	}
}

void CLWOMeshFileLoader::readVertexMapping(u32 size)
{
#ifdef LWO_READER_DEBUG
	os::Printer::log("LWO loader: loading Vertex mapping.");
#endif
	char type[5];
	type[4]=0;
	u16 dimension;
	core::stringc name;
	File->read(&type, 4);
	File->read(&dimension, 2);
#ifndef __BIG_ENDIAN__
	dimension=os::Byteswap::byteswap(dimension);
#endif
	size -= 6;
	size -= readString(name);
	if (strncmp(type, "TXUV", 4)) // also support RGB, RGBA, WGHT, ...
	{
		File->seek(size, true);
		return;
	}

	u32 index;
	core::vector2df tcoord;
	TCoords.set_used(Points.size());
	while (size!=0)
	{
		size -= readVX(index);
		File->read(&tcoord.X, 4);
#ifndef __BIG_ENDIAN__
		tcoord.X=os::Byteswap::byteswap(tcoord.X);
#endif
		File->read(&tcoord.Y, 4);
#ifndef __BIG_ENDIAN__
		tcoord.Y=os::Byteswap::byteswap(tcoord.Y);
#endif
		tcoord.Y=-tcoord.Y;
		TCoords[index]=tcoord;
		size -= 8;
	}
}

void CLWOMeshFileLoader::readTagMapping(u32 size)
{
#ifdef LWO_READER_DEBUG
	os::Printer::log("LWO loader: loading tag mappings.");
#endif
	char type[5];
	type[4]=0;
	File->read(&type, 4);
	size -= 4;
	if ((strncmp(type, "SURF", 4))||(Polygons.size()==0))
	{
		File->seek(size, true);
		return;
	}

	while (size!=0)
	{
		u16 tag;
		u32 polyIndex;
		size-=readVX(polyIndex);
		File->read(&tag, 2);
#ifndef __BIG_ENDIAN__
		tag=os::Byteswap::byteswap(tag);
#endif
		size -= 2;

		scene::SMeshBuffer *mb=Materials[tag]->Meshbuffer;
		const s32 vertCount=mb->Vertices.size();
		const s32 polySize=Polygons[polyIndex].size();
		video::S3DVertex vertex;
		for (s32 i=0; i<polySize; ++i)
		{
			vertex.Pos=Points[Polygons[polyIndex][i]];
			if (TCoords.size()>0)
				vertex.TCoords=TCoords[Polygons[polyIndex][i]];
			mb->Vertices.push_back(vertex);
		}
		for (s32 i=1; i<polySize-1; ++i)
		{
			core::vector3df normal = core::plane3df(mb->Vertices[vertCount].Pos,mb->Vertices[vertCount+i].Pos,mb->Vertices[vertCount+i+1].Pos).Normal.normalize();
			mb->Vertices[vertCount].Normal=normal;
			mb->Vertices[vertCount+i].Normal=normal;
			mb->Vertices[vertCount+i+1].Normal=normal;
			mb->Indices.push_back(vertCount);
			mb->Indices.push_back(vertCount+i);
			mb->Indices.push_back(vertCount+i+1);
		}
	}
}

void CLWOMeshFileLoader::readObj2(u32 size)
{
#ifdef LWO_READER_DEBUG
	os::Printer::log("LWO loader: loading polygons (new format).");
#endif
	char type[5];
	type[4]=0;
	File->read(&type, 4);
	size -= 4;
	Polygons.clear();
	if (strncmp(type, "FACE", 4)) // also possible are splines, subdivision patches, metaballs, and bones
	{
		File->seek(size, true);
		return;
	}
	u16 numVerts=0;
	while (size!=0)
	{
		File->read(&numVerts, 2);
#ifndef __BIG_ENDIAN__
		numVerts=os::Byteswap::byteswap(numVerts);
#endif
		// mask out flags
		numVerts &= 0x03FF;

		size -= 2;
		Polygons.push_back(core::array<u32>());
		u32 vertIndex;
		core::array<u32>& polyArray = Polygons.getLast();
		polyArray.reallocate(numVerts);
		for (u16 i=0; i<numVerts; ++i)
		{
			size -= readVX(vertIndex);
			polyArray.push_back(vertIndex);
		}
	}
}


void CLWOMeshFileLoader::readMat(u32 size)
{
#ifdef LWO_READER_DEBUG
	os::Printer::log("LWO loader: loading Surfaces.");
#endif
	core::stringc name;

	tLWOMaterial* mat=0;
	size -= readString(name);
	for (u32 i=0; i<Materials.size(); ++i)
	{
		if (Materials[i]->Name==name)
		{
			mat=Materials[i];
			break;
		}
	}
	if (!mat)
	{
		File->seek(size, true);
		return;
	}
	if (FormatVersion==2)
		size -= readString(name);

	video::SMaterial *irrMat=&mat->Meshbuffer->Material;

	u8 currTexture=0;
	while (size!=0)
	{
		char type[5];
		type[4]=0;
		u32 uiType;
		u32 tmp32;
		u16 subsize, tmp16;
		f32 tmpf32;
		File->read(&type, 4);
		//Convert 4-char string to 4-byte integer
		//Makes it possible to do a switch statement
		uiType = charsToUInt(type);
		File->read(&subsize, 2);
#ifndef __BIG_ENDIAN__
		subsize=os::Byteswap::byteswap(subsize);
#endif
		size -= 6;
		switch (uiType)
		{
			case charsToUIntD('C','O','L','R'):
				{
					s32 colSize = readColor(irrMat->DiffuseColor);
					irrMat->AmbientColor=irrMat->DiffuseColor;
					size -= colSize;
					subsize -= colSize;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[0]);
				}
				break;
			case charsToUIntD('D','I','F','F'):
				{
					if (FormatVersion==2)
					{
						File->read(&mat->Diffuse, 4);
#ifndef __BIG_ENDIAN__
						mat->Diffuse=os::Byteswap::byteswap(mat->Diffuse);
#endif
						size -= 4;
						subsize -= 4;
						size -= readVX(mat->Envelope[1]);
					}
					else
					{
						File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
						tmp16=os::Byteswap::byteswap(tmp16);
#endif
						mat->Diffuse=tmp16/256.0f;
						size -= 2;
						subsize -= 2;
					}
				}
				break;
			case charsToUIntD('V','D','I','F'):
				{
					File->read(&mat->Diffuse, 4);
#ifndef __BIG_ENDIAN__
					mat->Diffuse=os::Byteswap::byteswap(mat->Diffuse);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('L','U','M','I'):
				{
					if (FormatVersion==2)
					{
						File->read(&mat->Luminance, 4);
#ifndef __BIG_ENDIAN__
						mat->Luminance=os::Byteswap::byteswap(mat->Luminance);
#endif
						size -= 4;
						subsize -= 4;
						size -= readVX(mat->Envelope[2]);
					}
					else
					{
						File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
						tmp16=os::Byteswap::byteswap(tmp16);
#endif
						mat->Luminance=tmp16/256.0f;
						size -= 2;
						subsize -= 2;
					}				}
				break;
			case charsToUIntD('V','L','U','M'):
				{
					File->read(&mat->Luminance, 4);
#ifndef __BIG_ENDIAN__
					mat->Luminance=os::Byteswap::byteswap(mat->Luminance);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('S','P','E','C'):
				{
					if (FormatVersion==2)
					{
						File->read(&mat->Specular, 4);
#ifndef __BIG_ENDIAN__
						mat->Specular=os::Byteswap::byteswap(mat->Specular);
#endif
						size -= 4;
						subsize -= 4;
						size -= readVX(mat->Envelope[3]);
					}
					else
					{
						File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
						tmp16=os::Byteswap::byteswap(tmp16);
#endif
						mat->Specular=tmp16/256.0f;;
						size -= 2;
						subsize -= 2;
					}
				}
				break;
			case charsToUIntD('V','S','P','C'):
				{
					File->read(&mat->Specular, 4);
#ifndef __BIG_ENDIAN__
					mat->Specular=os::Byteswap::byteswap(mat->Specular);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('R','E','F','L'):
				{
					if (FormatVersion==2)
					{
						File->read(&mat->Reflection, 4);
#ifndef __BIG_ENDIAN__
						mat->Reflection=os::Byteswap::byteswap(mat->Reflection);
#endif
						size -= 4;
						subsize -= 4;
						size -= readVX(mat->Envelope[4]);
					}
					else
					{
						File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
						tmp16=os::Byteswap::byteswap(tmp16);
#endif
						mat->Reflection=tmp16/256.0f;
						size -= 2;
						subsize -= 2;
					}
				}
				break;
			case charsToUIntD('V','R','F','L'):
				{
					File->read(&mat->Reflection, 4);
#ifndef __BIG_ENDIAN__
					mat->Reflection=os::Byteswap::byteswap(mat->Reflection);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('T','R','A','N'):
				{
					if (FormatVersion==2)
					{
						File->read(&mat->Transparency, 4);
#ifndef __BIG_ENDIAN__
						mat->Transparency=os::Byteswap::byteswap(mat->Transparency);
#endif
						size -= 4;
						subsize -= 4;
						size -= readVX(mat->Envelope[5]);
					}
					else
					{
						File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
						tmp16=os::Byteswap::byteswap(tmp16);
#endif
						mat->Transparency=tmp16/256.0f;
						size -= 2;
						subsize -= 2;
					}
				}
				break;
			case charsToUIntD('V','T','R','N'):
				{
					File->read(&mat->Transparency, 4);
#ifndef __BIG_ENDIAN__
					mat->Transparency=os::Byteswap::byteswap(mat->Transparency);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('T','R','N','L'):
				{
					File->read(&mat->Translucency, 4);
#ifndef __BIG_ENDIAN__
					mat->Translucency=os::Byteswap::byteswap(mat->Translucency);
#endif
					size -= 4;
					subsize -= 4;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[6]);
				}
				break;
			case charsToUIntD('G','L','O','S'):
				{
					if (FormatVersion == 2)
					{
						File->read(&irrMat->Shininess, 4);
#ifndef __BIG_ENDIAN__
						irrMat->Shininess=os::Byteswap::byteswap(irrMat->Shininess);
#endif
						size -= 4;
						subsize -= 4;
						size -= readVX(mat->Envelope[7]);
					}
					else
					{
						File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
						tmp16=os::Byteswap::byteswap(tmp16);
#endif
						irrMat->Shininess=tmp16/16.f;
						size -= 2;
						subsize -= 2;
					}
				}
				break;
			case charsToUIntD('S','H','R','P'):
				{
					File->read(&mat->Sharpness, 4);
#ifndef __BIG_ENDIAN__
					mat->Sharpness=os::Byteswap::byteswap(mat->Sharpness);
#endif
					size -= 4;
					subsize -= 4;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[8]);
				}
				break;
			case charsToUIntD('B','U','M','P'):
			case charsToUIntD('T','A','M','P'):
				{
					File->read(&tmpf32, 4);
#ifndef __BIG_ENDIAN__
						tmpf32=os::Byteswap::byteswap(tmpf32);
#endif
					if (currTexture==6)
						irrMat->MaterialTypeParam=tmpf32;
					size -= 4;
					subsize -= 4;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[9]);
				}
				break;
			case charsToUIntD('S','I','D','E'):
				{
					File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
					tmp16=os::Byteswap::byteswap(tmp16);
#endif
					if (tmp16==1)
						irrMat->BackfaceCulling=true;
					if (tmp16==3)
						irrMat->BackfaceCulling=false;
					size -= 2;
				}
				break;
			case charsToUIntD('S','M','A','N'):
				{
					File->read(&mat->SmoothingAngle, 4);
#ifndef __BIG_ENDIAN__
					mat->SmoothingAngle=os::Byteswap::byteswap(mat->SmoothingAngle);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('R','F','O','P'):
			case charsToUIntD('R','F','L','T'):
				{
					File->read(&mat->ReflMode, 2);
#ifndef __BIG_ENDIAN__
					mat->ReflMode=os::Byteswap::byteswap(mat->ReflMode);
#endif
					size -= 2;
				}
				break;
			case charsToUIntD('R','I','M','G'):
				{
					if (FormatVersion==2)
					{
						size -= readVX(tmp32);
						if (tmp32)
							mat->ReflMap=Images[tmp32-1];
					}
					else
						size -= readString(mat->ReflMap);
				}
				break;
			case charsToUIntD('R','S','A','N'):
				{
					File->read(&mat->ReflSeamAngle, 4);
#ifndef __BIG_ENDIAN__
					mat->ReflSeamAngle=os::Byteswap::byteswap(mat->ReflSeamAngle);
#endif
					size -= 4;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[10]);
				}
				break;
			case charsToUIntD('R','B','L','R'):
				{
					File->read(&mat->ReflBlur, 4);
#ifndef __BIG_ENDIAN__
					mat->ReflBlur=os::Byteswap::byteswap(mat->ReflBlur);
#endif
					size -= 4;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[11]);
				}
				break;
			case charsToUIntD('R','I','N','D'):
				{
					File->read(&mat->RefrIndex, 4);
#ifndef __BIG_ENDIAN__
					mat->RefrIndex=os::Byteswap::byteswap(mat->RefrIndex);
#endif
					size -= 4;
					subsize -= 4;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[12]);
				}
				break;
			case charsToUIntD('T','R','O','P'):
				{
					File->read(&mat->TranspMode, 2);
#ifndef __BIG_ENDIAN__
					mat->TranspMode=os::Byteswap::byteswap(mat->TranspMode);
#endif
					size -= 2;
				}
				break;
			case charsToUIntD('T','I','M','G'):
				{
					if (FormatVersion==2)
					{
						size -= readVX(tmp32);
#ifndef __BIG_ENDIAN__
						tmp32=os::Byteswap::byteswap(tmp32);
#endif
						if (tmp32)
							mat->Texture[currTexture].Map=Images[tmp32-1];
					}
					else
						size -= readString(mat->Texture[currTexture].Map);
				}
				break;
			case charsToUIntD('T','B','L','R'):
				{
					File->read(&mat->TranspBlur, 4);
#ifndef __BIG_ENDIAN__
					mat->TranspBlur=os::Byteswap::byteswap(mat->TranspBlur);
#endif
					size -= 4;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[12]);
				}
				break;
			case charsToUIntD('F','L','A','G'):
				{
					File->read(&mat->Flags, 2);
#ifndef __BIG_ENDIAN__
					mat->Flags=os::Byteswap::byteswap(mat->Flags);
#endif
					if (mat->Flags&1)
						mat->Luminance=1.0f;
					if (mat->Flags&256)
						irrMat->BackfaceCulling=false;
					size -= 2;
				}
				break;
			case charsToUIntD('E','D','G','E'):
				{
					File->read(&mat->EdgeTransparency, 4);
#ifndef __BIG_ENDIAN__
					mat->EdgeTransparency=os::Byteswap::byteswap(mat->EdgeTransparency);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('C','T','E','X'):
				currTexture=0;
				size -= readString(mat->Texture[currTexture].Type);
				break;
			case charsToUIntD('D','T','E','X'):
				currTexture=1;
				size -= readString(mat->Texture[currTexture].Type);
				break;
			case charsToUIntD('S','T','E','X'):
				currTexture=2;
				size -= readString(mat->Texture[currTexture].Type);
				break;
			case charsToUIntD('R','T','E','X'):
				currTexture=3;
				size -= readString(mat->Texture[currTexture].Type);
				break;
			case charsToUIntD('T','T','E','X'):
				currTexture=4;
				size -= readString(mat->Texture[currTexture].Type);
				break;
			case charsToUIntD('L','T','E','X'):
				currTexture=5;
				size -= readString(mat->Texture[currTexture].Type);
				break;
			case charsToUIntD('B','T','E','X'):
				currTexture=6;
				size -= readString(mat->Texture[currTexture].Type);
				break;
			case charsToUIntD('T','A','L','P'):
				size -= readString(mat->Texture[currTexture].AlphaMap);
				break;
			case charsToUIntD('T','F','L','G'):
				{
					File->read(&mat->Texture[currTexture].Flags, 2);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].Flags=os::Byteswap::byteswap(mat->Texture[currTexture].Flags);
#endif
					size -= 2;
				}
				break;
			case charsToUIntD('E','N','A','B'):
				{
					File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
					tmp16=os::Byteswap::byteswap(tmp16);
#endif
					mat->Texture[currTexture].Active=(tmp16!=0);
					size -= 2;
				}
				break;
			case charsToUIntD('T','W','R','P'):
				{
					File->read(&mat->Texture[currTexture].WidthWrap, 2);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].WidthWrap=os::Byteswap::byteswap(mat->Texture[currTexture].WidthWrap);
#endif
					File->read(&mat->Texture[currTexture].HeightWrap, 2);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].HeightWrap=os::Byteswap::byteswap(mat->Texture[currTexture].HeightWrap);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('T','S','I','Z'):
				size -= readVec(mat->Texture[currTexture].Size);
				break;
			case charsToUIntD('T','C','T','R'):
				size -= readVec(mat->Texture[currTexture].Center);
				break;
			case charsToUIntD('T','F','A','L'):
				size -= readVec(mat->Texture[currTexture].Falloff);
				break;
			case charsToUIntD('T','V','E','L'):
				size -= readVec(mat->Texture[currTexture].Velocity);
				break;
			case charsToUIntD('T','C','L','R'):
				size -= readColor(mat->Texture[currTexture].Color);
				break;
			case charsToUIntD('T','A','A','S'):
				{
					File->read(&mat->Texture[currTexture].AntiAliasing, 4);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].AntiAliasing=os::Byteswap::byteswap(mat->Texture[currTexture].AntiAliasing);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('T','O','P','C'):
				{
					File->read(&mat->Texture[currTexture].Opacity, 4);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].Opacity=os::Byteswap::byteswap(mat->Texture[currTexture].Opacity);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('O','P','A','C'):
				{
					File->read(&mat->Texture[currTexture].OpacType, 2);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].OpacType=os::Byteswap::byteswap(mat->Texture[currTexture].OpacType);
#endif
					File->read(&mat->Texture[currTexture].Opacity, 4);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].Opacity=os::Byteswap::byteswap(mat->Texture[currTexture].Opacity);
#endif
					size -= 6;
					subsize -= 6;
					if (FormatVersion==2)
						size -= readVX(mat->Envelope[13]);
				}
				break;
			case charsToUIntD('T','V','A','L'):
				{
					File->read(&tmp16, 2);
#ifndef __BIG_ENDIAN__
					tmp16=os::Byteswap::byteswap(tmp16);
#endif
					mat->Texture[currTexture].Value=tmp16/256.0f;
					size -= 2;
				}
				break;
			case charsToUIntD('T','F','P','0'):
			case charsToUIntD('T','S','P','0'):
				{
					File->read(&mat->Texture[currTexture].FParam[0], 4);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].FParam[0]=os::Byteswap::byteswap(mat->Texture[currTexture].FParam[0]);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('T','F','P','1'):
			case charsToUIntD('T','S','P','1'):
				{
					File->read(&mat->Texture[currTexture].FParam[1], 4);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].FParam[1]=os::Byteswap::byteswap(mat->Texture[currTexture].FParam[1]);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('T','F','P','2'):
			case charsToUIntD('T','S','P','2'):
				{
					File->read(&mat->Texture[currTexture].FParam[2], 4);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].FParam[2]=os::Byteswap::byteswap(mat->Texture[currTexture].FParam[2]);
#endif
					size -= 4;
				}
				break;
			case charsToUIntD('T','F','R','Q'):
			case charsToUIntD('T','I','P','0'):
				{
					File->read(&mat->Texture[currTexture].IParam[0], 2);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].IParam[0]=os::Byteswap::byteswap(mat->Texture[currTexture].IParam[0]);
#endif
					size -= 2;
				}
				break;
			case charsToUIntD('T','I','P','1'):
				{
					File->read(&mat->Texture[currTexture].IParam[1], 2);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].IParam[1]=os::Byteswap::byteswap(mat->Texture[currTexture].IParam[1]);
#endif
					size -= 2;
				}
				break;
			case charsToUIntD('T','I','P','2'):
				{
					File->read(&mat->Texture[currTexture].IParam[2], 2);
#ifndef __BIG_ENDIAN__
					mat->Texture[currTexture].IParam[2]=os::Byteswap::byteswap(mat->Texture[currTexture].IParam[2]);
#endif
					size -= 2;
				}
				break;
			case charsToUIntD('B','L','O','K'):
				{
					core::stringc ordinal;
					File->read(&type, 4);
					File->read(&subsize, 2);
#ifndef __BIG_ENDIAN__
					subsize=os::Byteswap::byteswap(subsize);
#endif
					size -= 6;
					size -= readString(ordinal);
				}
				break;
			case charsToUIntD('C','H','A','N'):
				{
					File->read(&type, 4);
					size -= 4;
					if (!strncmp(type, "COLR", 4))
						currTexture=0;
					else if (!strncmp(type, "DIFF", 4))
						currTexture=1;
					else if (!strncmp(type, "LUMI", 4))
						currTexture=5;
					else if (!strncmp(type, "SPEC", 4))
						currTexture=2;
					else if (!strncmp(type, "REFL", 4))
						currTexture=3;
					else if (!strncmp(type, "TRAN", 4))
						currTexture=4;
					else if (!strncmp(type, "BUMP", 4))
						currTexture=6;
				}
				break;
			case charsToUIntD('I','M','A','G'):
				{
					u16 index;
					File->read(&index, 2);
#ifndef __BIG_ENDIAN__
					index=os::Byteswap::byteswap(index);
#endif
					size -= 2;
					if (index)
						mat->Texture[currTexture].Map=Images[index-1];
				}
				break;
			default:
				{
					File->seek(subsize, true);
					size -= subsize;
				}
		}
	}

	s32 stringPos;
	if (mat->Texture[0].Map != "")
	{
		irrMat->setTexture(0,Driver->getTexture(mat->Texture[0].Map.c_str()));
		if (!irrMat->getTexture(0))
		{
			stringPos = mat->Texture[0].Map.findLast('/');
			if (stringPos==-1)
				stringPos = mat->Texture[0].Map.findLast('\\');
			if (stringPos != -1)
				irrMat->setTexture(0, Driver->getTexture(mat->Texture[0].Map.subString(stringPos+1, mat->Texture[0].Map.size()-stringPos).c_str()));
		}
	}
	if (mat->Texture[3].Map != "")
	{
		video::ITexture* reflTexture = Driver->getTexture(mat->Texture[3].Map.c_str());
		if (!reflTexture)
		{
			stringPos = mat->Texture[3].Map.findLast('/');
			if (stringPos==-1)
				stringPos = mat->Texture[3].Map.findLast('\\');
			if (stringPos != -1)
				reflTexture = Driver->getTexture(mat->Texture[3].Map.subString(stringPos+1, mat->Texture[3].Map.size()-stringPos).c_str());
		}
		if (reflTexture)
		{
			irrMat->setTexture(1, irrMat->getTexture(0));
			irrMat->setTexture(0, reflTexture);
			irrMat->MaterialType=video::EMT_REFLECTION_2_LAYER;
		}
	}
	if (mat->Texture[4].Map != "")
	{
		video::ITexture* transTexture = Driver->getTexture(mat->Texture[4].Map.c_str());
		if (!transTexture)
		{
			stringPos = mat->Texture[4].Map.findLast('/');
			if (stringPos==-1)
				stringPos = mat->Texture[4].Map.findLast('\\');
			if (stringPos != -1)
				transTexture = Driver->getTexture(mat->Texture[4].Map.subString(stringPos+1, mat->Texture[4].Map.size()-stringPos).c_str());
		}
		if (transTexture)
		{
			irrMat->setTexture(1, irrMat->getTexture(0));
			irrMat->setTexture(0, transTexture);
			irrMat->MaterialType=video::EMT_TRANSPARENT_ADD_COLOR;
		}
	}
	if (mat->Texture[6].Map != "")
	{
		irrMat->setTexture(1, Driver->getTexture(mat->Texture[6].Map.c_str()));
		if (!irrMat->getTexture(1))
		{
			stringPos = mat->Texture[6].Map.findLast('/');
			if (stringPos==-1)
				stringPos = mat->Texture[6].Map.findLast('\\');
			if (stringPos != -1)
				irrMat->setTexture(1, Driver->getTexture(mat->Texture[6].Map.subString(stringPos+1, mat->Texture[6].Map.size()-stringPos).c_str()));
		}
		if (irrMat->getTexture(1))
		{
			Driver->makeNormalMapTexture(irrMat->getTexture(1));
			irrMat->MaterialType=video::EMT_PARALLAX_MAP_SOLID;
		}
	}
}


u32 CLWOMeshFileLoader::readColor(video::SColor& color)
{
	if (FormatVersion!=2)
	{
		u8 colorComponent;
		File->read(&colorComponent, 1);
		color.setRed(colorComponent);
		File->read(&colorComponent, 1);
		color.setGreen(colorComponent);
		File->read(&colorComponent, 1);
		color.setBlue(colorComponent);
		// unknown value
		File->read(&colorComponent, 1);
		return 4;
	}
	else
	{
		video::SColorf col;
		File->read(&col.r, 4);
#ifndef __BIG_ENDIAN__
		col.r=os::Byteswap::byteswap(col.r);
#endif
		File->read(&col.g, 4);
#ifndef __BIG_ENDIAN__
		col.g=os::Byteswap::byteswap(col.g);
#endif
		File->read(&col.b, 4);
#ifndef __BIG_ENDIAN__
		col.b=os::Byteswap::byteswap(col.b);
#endif
		color=col.toSColor();
		return 12;
	}
}

u32 CLWOMeshFileLoader::readString(core::stringc& name)
{
	c8 c;

	name="";
	File->read(&c, 1);
	while (c)
	{
		name.append(c);
		File->read(&c, 1);
	}
	// read extra 0 upon odd file position
	if (File->getPos() & 0x1)
	{
		File->read(&c, 1);
		return (name.size()+2);
	}
	return (name.size()+1);
}


u32 CLWOMeshFileLoader::readVec(core::vector3df& vec)
{
	File->read(&vec.X, 4);
#ifndef __BIG_ENDIAN__
	vec.X=os::Byteswap::byteswap(vec.X);
#endif
	File->read(&vec.Y, 4);
#ifndef __BIG_ENDIAN__
	vec.Y=os::Byteswap::byteswap(vec.Y);
#endif
	File->read(&vec.Z, 4);
#ifndef __BIG_ENDIAN__
	vec.Z=os::Byteswap::byteswap(vec.Z);
#endif
	return 12;
}


u32 CLWOMeshFileLoader::readVX(u32& num)
{
	u16 tmpIndex;

	File->read(&tmpIndex, 2);
#ifndef __BIG_ENDIAN__
	tmpIndex=os::Byteswap::byteswap(tmpIndex);
#endif
	num=tmpIndex;
	if (num >= 0xFF00)
	{
		File->read(&tmpIndex, 2);
#ifndef __BIG_ENDIAN__
		tmpIndex=os::Byteswap::byteswap(tmpIndex);
#endif
		num=((num << 16)|tmpIndex) & ~0xFF000000;
		return 4;
	}
	return 2;
}


bool CLWOMeshFileLoader::readFileHeader()
{
	u32 Id;

	File->read(&Id, 4);
#ifndef __BIG_ENDIAN__
	Id=os::Byteswap::byteswap(Id);
#endif
	if (Id != 0x464f524d) // FORM
		return false;

	//skip the file length
	File->read(&Id, 4);

	File->read(&Id, 4);
#ifndef __BIG_ENDIAN__
	Id=os::Byteswap::byteswap(Id);
#endif
	// Currently supported: LWOB, LWLO, LWO2
	switch (Id)
	{
		case 0x4c574f42:
			FormatVersion = 0; // LWOB
		break;
		case 0x4c574c4f:
			FormatVersion = 1; // LWLO
		break;
		case 0x4c574f32:
			FormatVersion = 2; // LWO2
		break;
		default:
			return false; // unsupported
	}

	return true;
}



} // end namespace scene
} // end namespace irr
