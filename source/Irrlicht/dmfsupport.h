// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
//
// This file was originally written by Salvatore Russo.
// I (Nikolaus Gebhardt) did some minor modifications changes to it and integrated
// it into Irrlicht:
// - removed STL dependency
// - removed log file and replaced it with irrlicht logging
// - adapted code formatting a bit to Irrlicht style
// - removed memory leaks
// Thanks a lot to Salvatore for his work on this and that he gave me
// his permission to add it into Irrlicht.

// This support library has been made by Salvatore Russo and is released under GNU public license for general uses.
// For uses in Irrlicht core and only for Irrlicht related uses I release this library under zlib license.
// It uses standard template libraries to create String class and StringList class used in DeleD
// plugins made by me.

#ifndef __DMF_SUPPORT_H_INCLUDED__
#define __DMF_SUPPORT_H_INCLUDED__

using namespace irr;
using namespace video;

#include "irrString.h"

/** A structure representing some DeleD infos.
This structure contains data about DeleD level file like: version, ambient colour, number of objects etc...*/
struct dmfHeader
{
	//main file header
	core::stringc dmfName; //!<Scene name
	f32 dmfVersion;     //!<File version
	SColor dmfAmbient; //!<Ambient color
	f32 dmfShadow;     //!<Shadow intensity
	u32 numObjects;    //!<Number of objects in this scene
	u32 numMaterials;  //!<Number of materials in this scene
	u32 numVertices;   //!<Total number of vertices faces*(vertices for each face)
	u32 numFaces;      //!<Total number of faces
	u32 numLights;     //!<Number of dynamic lights in this scene
	u32 numWatVertices; //!<Total number of vertices of water plains watfaces*(vertices for each face)
	u32 numWatFaces;   //!<Total number of faces for water plains.Note that each water plane is a rectangle with one face only.
};


/** A structure representing a DeleD material.
This structure contains texture names, an ID and some flags.*/
struct dmfMaterial
{
	u32 materialID;//!<This material unique ID.
	u32 textureLayers;//!<First texture Flag (0=Normal, 1=Color).
	u32 textureFlag;//!<First texture Flag (0=Normal, 1=Color).
	u32 lightmapFlag;//!<Lightmap Flag (0=Normal, others not considered).
	u32 textureBlend;//!<Texture Blend mode used to support alpha maps (4=Alpha map, others not implemented yet).
	core::stringc textureName;//!<Name of first texture (only file name, no path).
	core::stringc lightmapName;//!<Name of lightmap (only file name, no path).
};


/** A structure rapresenting a single face.
This structure contains first vertice index, number of vertices and the material used.*/
struct dmfFace
{
	u32 firstVert;//!<First vertex index.
	u32 numVerts;//!<Number of vertices for this face.
	u32 materialID;//!<Material used for this face.
};


/** A structure rapresenting a single vertice.
This structure contains vertice position coordinates and texture an lightmap UV.*/
struct dmfVert
{
	core::vector3df pos;//!<Position of vertex
	core::vector2df tc;//!<Texture UV coords
	core::vector2df lc;//!<Lightmap UV coords
};


/** A structure representing a single dynamic light.
This structure contains light position coordinates, diffuse colour, specular colour and maximum radius of light.*/
struct dmfLight
{
	core::vector3df pos;//!<Position of this light.
	SColorf diffuseColor;//!<Diffuse color.
	SColorf specularColor;//!<Specular color.
	f32 radius;//!<Maximum radius of light.
};

/** A structure rapresenting a single water plane.
This structure contains light position coordinates, diffuse colour, specular colour and maximum radius of light.*/
struct dmfWaterPlane
{
	u32 waterID;//!<ID of specified water plane.
	u32 numFaces;//!<number of faces that make this plain.Owing to the fact that this is a rectangle you'll have 1 every time.
	u32 firstFace;//!<first face of this plain.
	core::dimension2d<s32> tileNum;//!<number of tiles of this water plain.
	f32 waveHeight;//!<height of waves.
	f32 waveSpeed;//!<waves speed.
	f32 waveLength;//!<waves length.
};


/** A function to convert a hexstring to a int.
This function converts an hex string (i.e. FF) to its int value (i.e. 255).
\return An int representing the hex input value.*/
int axtoi(const char *hexStg)
{
	unsigned int intValue = 0;  // integer value of hex string
	sscanf(hexStg, "%x", &intValue);
	return (intValue);
}


/** A standard string.
A standard string created with standard template libraries.*/
typedef core::stringc String;

//Define a class StringList based on core::array and the defined class String
/** A simple stringlist class based on core::array.
This StringList class is based on core::array and the defined String class.
*/
class StringList : public core::array<String>
{
public:

	/**Basic Constructor*/
	StringList()
	{
	}

	/**Constructor based on file loading.
	Look at LoadFromFile specifications.*/
	StringList(io::IReadFile* file)
	{
		LoadFromFile(file);
	}

	/**Basic destructor.*/
	~StringList()
	{
		clear();
	}

	//Adds a String to StringList
	/** Add a string to this StringList.*/
	void Add(String str/*<String to add to the current StringList*/)
	{
		push_back(str);
	}

	//Loads a stringlist from a file
	//note that each String added to StringList
	//is separated by a \\n character and it's present
	//at the end of line.
	/** Loads a StringList from a file.
	This function loads a StringList from a file where each string is divided by a \\n char.*/
	void LoadFromFile(io::IReadFile* file)
	{
		const long sz = file->getSize();
		char* buf = new char[sz+1];
		file->read(buf, sz);
		buf[sz] = 0;
		char* p = buf;
		char* start = p;

		while(*p)
		{
			if (*p == '\n')
			{
				core::stringc str(start, (u32)(p - start - 1));
				str.trim();
				Add(str);
				start = p+1;
			}

			++p;
		}

		if (p - start > 1)
		{
			core::stringc str(start, (u32)(p - start - 1));
			str.trim();
			Add(str);
		}

		delete [] buf;
	}
};

//This function subdivides a string in a list of strings
/** This function subdivides strings divided by divider in a list of strings.
\return A StringList made of all strings divided by divider.*/
StringList SubdivideString(const String& str, const String& divider)
{
	StringList strings; //returned StringList
	strings.clear();    //clear returned stringlist

	int c=0;
	int l=str.size();

	//process entire string
	while(c<l)
	{
		String resultstr;
		resultstr = "";
		//read characters until divider is encountered
		while((str[c]!=divider[0]) && c<l)
		{
			resultstr += str[c];
			++c;
		}

		//Remove spaces \t and \n from string in my implementation...
		//pay attention or change it in dll.h if you don't want to remove
		//a particular char.
		resultstr.trim();//trims string resultstr
		strings.Add(resultstr);//add trimmed string
		++c;
	}

	return strings;
}


//Get DeleD informations and convert in dmfHeader
/**This function extract a dmfHeader from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile.
\return true if function succeed or false on fail.*/
bool GetDMFHeader(const StringList& RawFile, dmfHeader& header)
{
	StringList temp=SubdivideString(RawFile[0],String(";")); //file info

	if ( temp[0] != String("DeleD Map File") )
		return false; //not a deled file

	temp.clear();
	temp = SubdivideString(RawFile[1],String(" "));//get version
	StringList temp1=SubdivideString(temp[1],String(";"));

	header.dmfVersion = (float)atof(temp1[0].c_str());//save version
	if (header.dmfVersion < 0.91)
		return false;//not correct version

	temp.clear();
	temp = SubdivideString(RawFile[2],String(";"));//get name,ambient color and shadow opacity
	header.dmfName=temp[0];//save name

	//set ambient color
	header.dmfAmbient.set(axtoi(temp[1].c_str()));

	//set Shadow intensity
	header.dmfShadow = (float)atof(temp[2].c_str());

	//set current position
	int offs=3;

	//set Materials Number
	header.numMaterials=atoi(RawFile[offs].c_str());
	offs+=header.numMaterials;
	++offs;

	//set Object Number
	header.numObjects=atoi(RawFile[offs].c_str());

	//retrieve face and vertices number
	header.numVertices=0;
	header.numFaces=0;
	header.numWatFaces=0;
	header.numWatVertices=0;
	offs++;

	s32 fac;
	int i;

	for(i=0; i < (int)header.numObjects; i++)
	{
		StringList wat=SubdivideString(RawFile[offs],String(";"));
		StringList wat1=SubdivideString(wat[0],String("_"));

		++offs;
		offs += atoi(RawFile[offs].c_str());
		++offs;

		fac=atoi(RawFile[offs].c_str());

		if(!(wat1[0]==String("water") && wat[2]==String("0")))
			header.numFaces = header.numFaces + fac;
		else
			header.numWatFaces = header.numWatFaces + fac;

		offs++;

		for(int j=0; j<fac; j++)
		{
			if(!(wat1[0] == String("water") && wat[2] == String("0")))
				header.numVertices=header.numVertices + atoi(RawFile[offs+j].c_str());
			else
				header.numWatVertices=header.numWatVertices + atoi(RawFile[offs + j].c_str());
		}

		offs = offs + fac;
	}

	//retrieve number of dynamic lights
	header.numLights=0;
	temp.clear();
	temp1.clear();
	s32 lit = atoi(RawFile[offs].c_str());

	for (i=0; i<lit; i++)
	{
		offs++;
		temp=SubdivideString(RawFile[offs],String(";"));

		if(atoi(temp[0].c_str())==1)
		{
			temp1=SubdivideString(temp[18],String("_"));

			if(temp1[0]==String("dynamic"))
			header.numLights++;
		}
		temp.clear();
		temp1.clear();
	}

	return true; //everything is OK so loading is correct
}


/**This function extract an array of dmfMaterial from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile.
\param RawFile StringList representing a DMF file.
\param materials Materials returned.
\param num_material Number of materials contained in DMF file.
\param use_material_dirs Here you can choose to use default DeleD structure for material dirs.
\return true if function succeed or false on fail.*/
bool GetDMFMaterials(const StringList& RawFile,
			core::array<dmfMaterial>& materials,
			int num_material,
			bool use_material_dirs=false)
{
	int offs=4;
	StringList temp;
	StringList temp1;
	StringList temp2;

	materials.reallocate(num_material);
	for(int i=0; i<num_material; ++i)
	{
		materials.push_back(dmfMaterial());
		temp=SubdivideString(RawFile[offs+i],";");
		materials[i].materialID = i;
		materials[i].textureLayers = atoi(temp[3].c_str());
		temp1=SubdivideString(temp[5],",");

		materials[i].textureFlag = atoi(temp1[0].c_str());
		materials[i].textureName=temp1[1];
		materials[i].textureName.replace('\\','/');
		materials[i].textureBlend = atoi(temp1[2].c_str());
		temp1.clear();
		temp2.clear();
		int a=temp.size();
		if(a>=9)
		{
			temp1=SubdivideString(temp[temp.size() - 1],",");
			materials[i].lightmapFlag=atoi(temp1[0].c_str());
			if(!use_material_dirs)
			{
				temp2=SubdivideString(temp1[1],"\\");
				materials[i].lightmapName=temp2.getLast();
			}
			else
				materials[i].lightmapName=temp1[1];
		}
		else
		{
			materials[i].lightmapFlag=1;
			materials[i].lightmapName="";
		}
		temp1.clear();
		temp2.clear();
	}
	return true;
}


/**This function extract an array of dmfMaterial from a DMF file considering 1st an 2nd layer for water plains.
You must give in input a StringList representing a DMF file loaded with LoadFromFile.
\return true if function succeed or false on fail.*/
bool GetDMFWaterMaterials(const StringList& RawFile /**<StringList representing a DMF file.*/,
		core::array<dmfMaterial>& materials/**<Materials returned.*/,
		int num_material/**<Number of materials contained in DMF file.*/
		)
{
	int offs=4;
	StringList temp;
	StringList temp1;
	StringList temp2;
	//Checking if this is a DeleD map File of version >= 0.91
	temp=SubdivideString(RawFile[0],String(";"));//file info

	if ( temp[0] != String("DeleD Map File") )
		return false;//not a deled file

	temp.clear();
	temp=SubdivideString(RawFile[1],String(" "));//get version
	temp1=SubdivideString(temp[1],String(";"));

	if (atof(temp1[0].c_str()) < 0.91)
		return false;//not correct version

	//end checking
	temp.clear();
	temp1.clear();

	for(int i=0;i<num_material;i++)
	{
		temp = SubdivideString(RawFile[offs+i],";");
		materials[i].materialID=i;

		temp1 = SubdivideString(temp[5],",");
		materials[i].textureFlag=atoi(temp1[0].c_str());
		temp2 = SubdivideString(temp1[1],"\\");

		materials[i].textureName=temp2.getLast();
		temp1.clear();
		temp2.clear();
		int a=temp.size();
		if(a==7)
		{
			temp1=SubdivideString(temp[6],",");
			materials[i].lightmapFlag=atoi(temp1[0].c_str());
			temp2=SubdivideString(temp1[1],"\\");
			materials[i].lightmapName=temp2.getLast();
		}
		else
		{
			materials[i].lightmapFlag=1;
			materials[i].lightmapName="FFFFFFFF";
		}
		temp1.clear();
		temp2.clear();
	}
	return true;
}


/**This function extract an array of dmfVert and dmfFace from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile and two arrays long enough.
Please use GetDMFHeader() before this function to know number of vertices and faces.
\return true if function succeed or false on fail.*/
bool GetDMFVerticesFaces(const StringList& RawFile/**<StringList representing a DMF file.*/,
		dmfVert vertices[]/**<Vertices returned*/,
		dmfFace faces[]/**Faces returned*/
		)
{
	StringList temp,temp1;

	// skip materials
	s32 offs = 4 + atoi(RawFile[3].c_str());

	const s32 objs = atoi(RawFile[offs].c_str());
#ifdef _IRR_DMF_DEBUG_
	os::Printer::log("Reading objects", core::stringc(objs).c_str());
#endif

	s32 vert=0, tmp_sz=0, vert_cnt=0, face_cnt=0;
	offs++;

	for (int i=0; i<objs; ++i)
	{
		StringList wat=SubdivideString(RawFile[offs],";");
		StringList wat1=SubdivideString(wat[0],"_");
#ifdef _IRR_DMF_DEBUG_
	os::Printer::log("Reading object", wat[0].c_str());
#endif

		offs++;
		const s32 vrtxPos=offs+1;
		// skip vertices
		offs += atoi(RawFile[offs].c_str());
		offs++;
		const s32 numFaces=atoi(RawFile[offs].c_str());
		offs++;
		if(!(wat1[0]==String("water") && wat[2]==String("0")))
		{
			for(s32 j=0; j<numFaces; ++j)
			{
				temp=SubdivideString(RawFile[offs+j],";");

				//first value is vertices number for this face
				vert=atoi(temp[0].c_str());
				faces[face_cnt].numVerts=vert;
				//second is material ID
				faces[face_cnt].materialID=atoi(temp[1].c_str());
				//vertices are ordined
				faces[face_cnt].firstVert=vert_cnt;

				//now we'll create vertices structure
				for(s32 k=0; k<vert; ++k)
				{
					//get vertex position
					temp1=SubdivideString(RawFile[vrtxPos+atoi(temp[2+k].c_str())],";");
					//copy x,y,z values
					vertices[vert_cnt].pos.set((float)atof(temp1[0].c_str()),
							(float)atof(temp1[1].c_str()),
							(float)-atof(temp1[2].c_str()));
					//get uv coords for tex and light if any
					vertices[vert_cnt].tc.set((float)atof(temp[2+vert+(2*k)].c_str()),
							(float)atof(temp[2+vert+(2*k)+1].c_str()));
					tmp_sz=temp.size();
					vertices[vert_cnt].lc.set((float)atof(temp[tmp_sz-(2*vert)+(2*k)].c_str()),
							(float)atof(temp[tmp_sz-(2*vert)+(2*k)+1].c_str()));
					vert_cnt++;
					temp1.clear();
				}

				face_cnt++;
				temp.clear();
			}
		}

		offs=offs+numFaces;
	}

	return true;
}


/**This function extract an array of dmfLights from a DMF file.
You must give in input a StringList representing a DMF file loaded with
LoadFromFile and one array long enough.  Please use GetDMFHeader() before this
function to know number of dynamic lights.
\return true if function succeed or false on fail.*/
bool GetDMFLights(const StringList& RawFile/**<StringList representing a DMF file.*/,
		dmfLight lights[]/**<Lights returned.*/
		)
{
	int offs=3;
	StringList temp,temp1;

	//Checking if this is a DeleD map File of version >= 0.91
	temp=SubdivideString(RawFile[0],String(";"));//file info

	if ( temp[0] != String("DeleD Map File") )
		return false;//not a deled file

	temp.clear();
	temp=SubdivideString(RawFile[1],String(" "));//get version
	temp1=SubdivideString(temp[1],String(";"));

	if (atof(temp1[0].c_str()) < 0.91)
		return false;//not correct version

	//end checking

	temp.clear();
	temp1.clear();
	offs=offs + atoi(RawFile[offs].c_str());
	offs++;
	s32 objs = atoi(RawFile[offs].c_str());
	s32 lit=0;
	s32 d_lit=0;
	offs++;

	//let's get position of lights in file
	int i;
	for(i=0;i<objs;i++)
	{
		offs++;

		offs = offs + atoi(RawFile[offs].c_str());
		offs++;

		offs = offs + atoi(RawFile[offs].c_str());
		offs++;
	}

	//let's find dynamic lights
	lit = atoi(RawFile[offs].c_str());

	for(i=0;i<lit;i++)
	{
		offs++;
		temp=SubdivideString(RawFile[offs],String(";"));
		if(atoi(temp[0].c_str())==1)
		{
			temp1=SubdivideString(temp[18],String("_"));
			if(temp1[0]==String("dynamic"))
			{
				lights[d_lit].radius = (float)atof(temp[4].c_str());
				lights[d_lit].pos.set((float)atof(temp[5].c_str()),
						(float)atof(temp[6].c_str()),
						(float)-atof(temp[7].c_str()));

				lights[d_lit].diffuseColor = SColorf(
						SColor(255, atoi(temp[10].c_str()), atoi(temp[11].c_str()),
						atoi(temp[12].c_str())));

				lights[d_lit].specularColor = SColorf(
						SColor(255, atoi(temp[13].c_str()), atoi(temp[14].c_str()),
						atoi(temp[15].c_str())));

				d_lit++;
			}
		}
		temp.clear();
		temp1.clear();
	}

	return true;
}


/**This function extracts an array of dmfWaterPlane,dmfVert and dmfFace from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile and three arrays long enough.
Please use GetDMFHeader() before this function to know number of water plains and water faces as well as water vertices.
\return true if function succeed or false on fail.*/
bool GetDMFWaterPlanes(const StringList& RawFile/**<StringList representing a DMF file.*/,
		dmfWaterPlane wat_planes[]/**<Water planes returned.*/,
		dmfVert vertices[]/**<Vertices returned*/,
		dmfFace faces[]/**Faces returned*/
		)
{
	int offs=3;
	int offs1=0;
	StringList temp,temp1;

	//Checking if this is a DeleD map File of version >= 0.91
	temp=SubdivideString(RawFile[0],String(";"));//file info

	if ( temp[0] != String("DeleD Map File") )
		return false;//not a deled file

	temp.clear();
	temp=SubdivideString(RawFile[1],String(" "));//get version
	temp1=SubdivideString(temp[1],String(";"));

	if (atof(temp1[0].c_str()) < 0.91)
		return false;//not correct version

	//end checking

	temp.clear();
	temp1.clear();
	offs=offs+atoi(RawFile[offs].c_str());
	offs++;
	s32 objs=atoi(RawFile[offs].c_str());
	s32 fac=0,vert=0,tmp_sz=0,vert_cnt=0,face_cnt=0,wat_id=0;
	core::dimension2d<s32> tilenum(40,40);
	f32 waveheight=3.0f;
	f32 wavespeed=300.0f;
	f32 wavelength=80.0f;
	offs++;

	for(int i=0;i<objs;i++)
	{
		StringList wat=SubdivideString(RawFile[offs],";");
		StringList wat1=SubdivideString(wat[0],"_");
		offs++;
		offs1=offs;
		offs=offs+atoi(RawFile[offs].c_str());
		offs++;
		offs1++;
		fac=atoi(RawFile[offs].c_str());
		offs++;

		if(wat1[0]==String("water") && wat[2]==String("0"))
		{
			StringList userinfo=SubdivideString(wat[7],",");

			int j;

			for (j=0; j<(int)userinfo.size(); j++)
			{
				switch(j)
				{
				case 0:
					if(atoi(userinfo[0].c_str()))
						tilenum.Width = atoi(userinfo[0].c_str());
				break;
				case 1:
					if(atoi(userinfo[1].c_str()))
						tilenum.Height = atoi(userinfo[1].c_str());
				break;
				case 2:
					if(atof(userinfo[2].c_str()))
						waveheight = (float)atof(userinfo[2].c_str());
				break;
				case 3:
					if(atof(userinfo[3].c_str()))
						wavespeed = (float)atof(userinfo[3].c_str());
				break;
				case 4:
					if(atof(userinfo[4].c_str()))
						wavelength = (float)atof(userinfo[4].c_str());
				break;
				}
			}

			wat_planes[wat_id].waterID=wat_id;
			wat_planes[wat_id].numFaces=fac;
			wat_planes[wat_id].firstFace=face_cnt;
			wat_planes[wat_id].tileNum=tilenum;
			wat_planes[wat_id].waveHeight=waveheight;
			wat_planes[wat_id].waveSpeed=wavespeed;
			wat_planes[wat_id].waveLength=wavelength;

			for(j=0;j<fac;j++)
			{
				temp=SubdivideString(RawFile[offs+j],";");

				//first value is vertices number for this face
				faces[face_cnt].numVerts=atoi(temp[0].c_str());
				vert=faces[face_cnt].numVerts;
				//second is material ID
				faces[face_cnt].materialID=atoi(temp[1].c_str());
				//vertices are ordined
				faces[face_cnt].firstVert=vert_cnt;

				//now we'll create vertices structure
				for(int k=0;k<vert;k++)
				{
					//get vertex position
					temp1=SubdivideString(RawFile[offs1+atoi(temp[2+k].c_str())], ";");

					//copy x,y,z values
					vertices[vert_cnt].pos.set((float)atof(temp1[0].c_str()),
							(float)atof(temp1[1].c_str()),
							(float)-atof(temp1[2].c_str()));

					//get uv coords for tex and light if any
					vertices[vert_cnt].tc.set((float)atof(temp[2+vert+(2*k)].c_str()),
							(float)atof(temp[2+vert+(2*k)+1].c_str()));
					tmp_sz=temp.size();

					vertices[vert_cnt].lc.set((float)atof(temp[tmp_sz-(2*vert)+(2*k)].c_str()),
							(float)atof(temp[tmp_sz-(2*vert)+(2*k)+1].c_str()));
					++vert_cnt;
					temp1.clear();
				}
				++face_cnt;
				temp.clear();
			}
		}
		offs=offs+fac;
	}

	return true;
}

#endif /* __DMF_SUPPORT_H__ */

