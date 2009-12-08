#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace io;

#define COMPARE(a, b) if ( (a) != (b) ) { logTestString("Not identical %s in %s:%d", #a, __FILE__, __LINE__ ); return false; }

const u32 BINARY_BLOCK_SIZE = 10;

enum EMockEnum
{
	EME_NONE,
	EME_ONE,
	EME_COUNT
};

const c8* const MockEnumNames[EME_COUNT+1] =
{
	"none",
	"one",
	0,
};

class SerializableMock : public virtual io::IAttributeExchangingObject
{
public:

	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		out->addInt("valInt", valInt);
		out->addFloat("valFloat", valFloat);
		out->addString("valString", valString.c_str());
		out->addString("valStringW", valStringW.c_str());
		out->addBinary("valBinary", (void*)valBinary, BINARY_BLOCK_SIZE);
		out->addArray("valStringWArray", valStringWArray);
		out->addBool("valBool", valBool);
		out->addEnum("valEnum", valEnum, MockEnumNames);
		out->addColor("valColor", valColor);
		out->addColorf("valColorf", valColorf);
		out->addVector3d("valVector3df", valVector3df);
		out->addPosition2d("valPosition2di", valPosition2di);
		out->addRect("valRect", valRect);
		out->addMatrix("valMatrix", valMatrix);
		out->addQuaternion("valQuaternion", valQuaternion);
		out->addBox3d("valAabbox3df", valAabbox3df);
		out->addPlane3d("valPlane3df", valPlane3df);
		out->addTriangle3d("valTriangle3df", valTriangle3df);
		out->addLine2d("valLine2df", valLine2df);
		out->addLine3d("valLine3df", valLine3df);
		out->addTexture("valTexture", valTexture );
		out->addUserPointer("valPointer", valPointer);
	}

	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{
		valInt = in->getAttributeAsInt("valInt");
		valFloat = in->getAttributeAsFloat("valFloat");
		valString = in->getAttributeAsString("valString");
		valStringW = in->getAttributeAsStringW("valStringW");
		in->getAttributeAsBinaryData("valBinary", valBinary, BINARY_BLOCK_SIZE);
		valStringWArray = in->getAttributeAsArray("valStringWArray");
		valBool = in->getAttributeAsBool("valBool");
		valEnum = (EMockEnum)in->getAttributeAsEnumeration("valEnum", MockEnumNames);
		valColor = in->getAttributeAsColor("valColor");
		valColorf = in->getAttributeAsColorf("valColorf");
		valVector3df = in->getAttributeAsVector3d("valVector3df");
		valPosition2di = in->getAttributeAsPosition2d("valPosition2di");
		valRect = in->getAttributeAsRect("valRect");
		valMatrix = in->getAttributeAsMatrix("valMatrix");
		valQuaternion = in->getAttributeAsQuaternion("valQuaternion");
		valAabbox3df = in->getAttributeAsBox3d("valAabbox3df");
		valPlane3df = in->getAttributeAsPlane3d("valPlane3df");
		valTriangle3df = in->getAttributeAsTriangle3d("valTriangle3df");
		valLine2df = in->getAttributeAsLine2d("valLine2df");
		valLine3df = in->getAttributeAsLine3d("valLine3df");
		valTexture = in->getAttributeAsTexture("valTexture");
		valPointer = in->getAttributeAsUserPointer("valPointer");
	}

	bool operator==(const SerializableMock& other)
	{
		COMPARE(valInt, other.valInt);
		COMPARE(valFloat, other.valFloat);
		COMPARE(valString, other.valString);
		COMPARE(valStringW, other.valStringW);
		if ( memcmp( valBinary, other.valBinary, BINARY_BLOCK_SIZE) != 0 )
		{
			logTestString("Not identical %s in %s:%d", "valBinary",  __FILE__, __LINE__ );
			return false;
		}
		COMPARE(valStringWArray, other.valStringWArray);
		COMPARE(valBool, other.valBool);
		COMPARE(valEnum, other.valEnum);
		COMPARE(valColor, other.valColor);
		if ( valColorf.r != other.valColorf.r || valColorf.g != other.valColorf.g || valColorf.b != other.valColorf.b || valColorf.a != other.valColorf.a )
		{
			logTestString("Not identical %s in %s:%d", "valColorf",  __FILE__, __LINE__ );
			return false;
		}
		COMPARE(valVector3df, other.valVector3df);
		COMPARE(valPosition2di, other.valPosition2di);
		COMPARE(valRect, other.valRect);
		COMPARE(valMatrix, other.valMatrix);
		COMPARE(valQuaternion, other.valQuaternion);
		COMPARE(valAabbox3df, other.valAabbox3df);
		COMPARE(valPlane3df, other.valPlane3df);
		COMPARE(valTriangle3df, other.valTriangle3df);
		COMPARE(valLine2df, other.valLine2df);
		COMPARE(valLine3df, other.valLine3df);
//		valTexture;
		COMPARE(valPointer, other.valPointer);
		return true;
	}

	void reset()
	{
		valInt = 0;
		valFloat = 0.f;
		valString = "";
		valStringW = L"";
		memset(valBinary, 0, BINARY_BLOCK_SIZE);
		valStringWArray.clear();
		valBool = false;
		valEnum = EME_NONE;
		valColor.set(0,0,0,0);
		valColorf.set(0.f, 0.f, 0.f, 0.f);
		valVector3df.set(0.f, 0.f, 0.f);
		valPosition2di.set(0,0);
		valRect = core::rect<s32>(0,0,0,0);
		valMatrix.makeIdentity();
		valQuaternion.set(0,0,0,0);
		valAabbox3df.reset(0,0,0);
		valPlane3df.setPlane(vector3df(0.f,0.f,0.f), 0.f);
		valTriangle3df.set( vector3df(0.f,0.f,0.f), vector3df(0.f,0.f,0.f), vector3df(0.f,0.f,0.f) );
		valLine2df.setLine(0.f, 0.f, 0.f, 0.f);
		valLine3df.setLine(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
		valTexture = NULL;
		valPointer = 0;
	}

	void set()
	{
		valInt = 1;
		valFloat = 1.f;
		valString = "one";
		valStringW = L"ONE";
		memset(valBinary, 0xff, BINARY_BLOCK_SIZE);
		valStringWArray.push_back( stringw("ONE") );
		valStringWArray.push_back( stringw("TWO") );
		valStringWArray.push_back( stringw("THREE") );
		valBool = true;
		valEnum = EME_ONE;
		valColor.set(1,2,3,4);
		valColorf.set(1.f, 2.f, 3.f, 4.f);
		valVector3df.set(1.f, 2.f, 3.f);
		valPosition2di.set(1,2);
		valRect = core::rect<s32>(1,2,3,4);
		valMatrix = 99.9f;
		valQuaternion.set(1,2,3,4);
		valAabbox3df.reset(1,2,3);
		valPlane3df.setPlane(vector3df(1.f,2.f,3.f), 4.f);
		valTriangle3df.set( vector3df(1.f,2.f,3.f), vector3df(4.f,5.f,6.f), vector3df(7.f,8.f,9.f) );
		valLine2df.setLine(1.f, 2.f, 3.f, 4.f);
		valLine3df.setLine(1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
		valTexture = NULL;	// TODO
		valPointer = (void*)0xffffff;
	}

	s32 						valInt;
	f32 						valFloat;
	core::stringc 				valString;
	core::stringw 				valStringW;
	char 						valBinary[BINARY_BLOCK_SIZE];
	core::array<core::stringw> 	valStringWArray;
	bool 						valBool;
	EMockEnum					valEnum;
	video::SColor 				valColor;
	video::SColorf 				valColorf;
	core::vector3df 			valVector3df;
	core::position2di 			valPosition2di;
	core::rect<s32> 			valRect;
	core::matrix4 				valMatrix;
	core::quaternion 			valQuaternion;
	core::aabbox3df 			valAabbox3df;
	core::plane3df 				valPlane3df;
	core::triangle3df 			valTriangle3df;
	core::line2df 				valLine2df;
	core::line3df 				valLine3df;
	video::ITexture* 			valTexture;
	void* 						valPointer;
};

bool serializeAttributes()
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL, dimension2d<u32>(1, 1));
	assert(device);
	if(!device)
		return false;

	io::IFileSystem * fs = device->getFileSystem ();
	if ( !fs )
		return false;

	SerializableMock origMock, copyMock;
	origMock.set();
	copyMock.reset();

	io::IAttributes* attr = fs->createEmptyAttributes();
	origMock.serializeAttributes(attr, 0);
	copyMock.deserializeAttributes(attr, 0);

	return origMock == copyMock;
}
