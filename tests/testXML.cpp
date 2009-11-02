// Copyright (C) 2009 Christian Stehno
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;

/** Tests for XML handling */
bool testXML(void)
{
	IrrlichtDevice *device = createDevice(video::EDT_NULL, dimension2du(400, 200));

	io::IXMLReaderUTF8* reader = device->getFileSystem()->createXMLReaderUTF8("media/test.xml");
	if (!reader)
	{
		logTestString("Could not create XML reader.\n");
		return false;
	}

	const core::stringc expected[] = {
		"a", "b", "c"
	};

	bool retVal = true;
	u32 i=0;
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (expected[i++] != reader->getNodeName())
			{
				logTestString("Did not find expected string in XML element name.\n");
				retVal = false;
				break;
			}
		}
	}

	reader->drop();
	return retVal;
}

