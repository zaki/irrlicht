#include "CFontTool.h"
#include "IXMLWriter.h"

using namespace irr;

const int fontsizes[] = {4,6,8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,56,68,72,0};

inline u32 getTextureSizeFromSurfaceSize(u32 size)
{
	int ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}

// windows specific
#ifdef _IRR_WINDOWS_

	const DWORD charsets[] = {	ANSI_CHARSET, DEFAULT_CHARSET, OEM_CHARSET, BALTIC_CHARSET, GB2312_CHARSET, CHINESEBIG5_CHARSET,
								EASTEUROPE_CHARSET, GREEK_CHARSET, HANGUL_CHARSET, MAC_CHARSET, RUSSIAN_CHARSET,
								SHIFTJIS_CHARSET, SYMBOL_CHARSET, TURKISH_CHARSET, VIETNAMESE_CHARSET, JOHAB_CHARSET,
								ARABIC_CHARSET, HEBREW_CHARSET, THAI_CHARSET, 0};

	const wchar_t *setnames[] = {L"ANSI", L"All Available", L"OEM", L"Baltic", L"Chinese Simplified", L"Chinese Traditional",
								L"Eastern European", L"Greek", L"Hangul", L"Macintosh", L"Russian",
								L"Japanese", L"Symbol", L"Turkish", L"Vietnamese", L"Johab",
								L"Arabic", L"Hebrew", L"Thai", 0};

	// callback for adding font names
	int CALLBACK EnumFontFamExProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme,
					DWORD FontType, LPARAM lParam)
	{
		CFontTool* t = (CFontTool*) lParam;
		t->FontNames.push_back( core::stringw(lpelfe->elfFullName));
		return 1;
	}

	//
	// Constructor
	//

	CFontTool::CFontTool(IrrlichtDevice* device) : FontSizes(fontsizes),
			Device(device), UseAlphaChannel(false),
			// win specific
			dc(0)
	{
		// init display context
		dc = CreateDC(L"DISPLAY", L"DISPLAY", 0 ,0 );

		// populate list of available character set names
		for (int i=0; setnames[i] != 0; ++i)
			CharSets.push_back( core::stringw(setnames[i]));

		selectCharSet(0);
	}

	void CFontTool::selectCharSet(u32 currentCharSet)
	{
		if ( currentCharSet >= CharSets.size() )
			return;

		LOGFONTW lf;
		lf.lfFaceName[0] = L'\0';
		lf.lfCharSet = (BYTE) charsets[currentCharSet];
		// HRESULT hr; // no error checking(!)

		// clear font list
		FontNames.clear();

		// create list of available fonts
		EnumFontFamiliesExW( dc, (LPLOGFONTW) &lf, (FONTENUMPROCW) EnumFontFamExProc, (LPARAM) this, 0);
	}

	bool CFontTool::makeBitmapFont(u32 fontIndex, u32 charsetIndex, s32 fontSize, u32 textureWidth, u32 textureHeight, bool bold, bool italic, bool aa, bool alpha)
	{
		if (fontIndex >= FontNames.size() || charsetIndex >= CharSets.size() )
			return false;

		UseAlphaChannel = alpha;
		u32 currentImage = 0;

		// create the font
		HFONT font = CreateFontW(
			-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSY), 72), 0,
			0,0,
			bold ? FW_BOLD : 0,
			italic, 0,0, charsets[charsetIndex], 0,0,
			aa ? ANTIALIASED_QUALITY : 0,
			0, FontNames[fontIndex].c_str() );

		if (!font)
			return false;

		SelectObject(dc, font);
		SetTextAlign (dc,TA_LEFT | TA_TOP | TA_NOUPDATECP);

		// get rid of the current textures/images
		for (u32 i=0; i<currentTextures.size(); ++i)
			currentTextures[i]->drop();
		currentTextures.clear();

		for (u32 i=0; i<currentImages.size(); ++i)
			currentImages[i]->drop();
		currentImages.clear();

		// clear current image mappings
		CharMap.clear();
		// clear array
		Areas.clear();

		// get information about this font's unicode ranges.
		s32 size = GetFontUnicodeRanges( dc, 0);
		c8 *buf = new c8[size];
		LPGLYPHSET glyphs = (LPGLYPHSET)buf;

		GetFontUnicodeRanges( dc, glyphs);

//		s32 TotalCharCount = glyphs->cGlyphsSupported;

		s32 currentx=0, currenty=0, maxy=0;

		for (u32 range=0; range < glyphs->cRanges; range++)
		{
			WCRANGE* current = &glyphs->ranges[range];

			maxy=0;

			// loop through each glyph and write its size and position
			for (s32 ch=current->wcLow; ch< current->wcLow + current->cGlyphs; ch++)
			{
				wchar_t currentchar = ch;

				if ( IsDBCSLeadByte((BYTE) ch))
					continue;	// surragate pairs unsupported

				// get the dimensions
				SIZE size;
				ABC abc;
				GetTextExtentPoint32W(dc, &currentchar, 1, &size);
				SFontArea fa;
				fa.underhang = 0;
				fa.overhang  = 0;

				if (GetCharABCWidthsW(dc, currentchar, currentchar, &abc)) // for unicode fonts, get overhang, underhang, width
				{
					size.cx = abc.abcB;
					fa.underhang  = abc.abcA;
					fa.overhang   = abc.abcC;

					if (abc.abcB-abc.abcA+abc.abcC<1)
						continue;	// nothing of width 0
				}
				if (size.cy < 1)
					continue;

				//GetGlyphOutline(dc, currentchar, GGO_METRICS, &gm, 0, 0, 0);

				//size.cx++; size.cy++;

				// wrap around?
				if (currentx + size.cx > (s32) textureWidth)
				{
					currenty += maxy;
					currentx = 0;
					if ((u32)(currenty + maxy) > textureHeight)
					{
						currentImage++; // increase Image count
						currenty=0;
					}
					maxy = 0;
				}
				// add this char dimension to the current map

				fa.rectangle = core::rect<s32>(currentx, currenty, currentx + size.cx, currenty + size.cy);
				fa.sourceimage = currentImage;

				CharMap.insert(currentchar, Areas.size());
				Areas.push_back( fa );

				currentx += size.cx +1;

				if (size.cy+1 > maxy)
					maxy = size.cy+1;
			}
		}
		currenty += maxy;

		u32 lastTextureHeight = getTextureSizeFromSurfaceSize(currenty);

		// delete the glyph set
		delete buf;

		currentImages.set_used(currentImage+1);
		currentTextures.set_used(currentImage+1);

		for (currentImage=0; currentImage < currentImages.size(); ++currentImage)
		{
			core::stringc logmsg = "Creating image ";
			logmsg += (s32) (currentImage+1);
			logmsg += " of ";
			logmsg += (s32) currentImages.size();
			Device->getLogger()->log(logmsg.c_str());
			// no need for a huge final texture
			u32 texHeight = textureHeight;
			if (currentImage == currentImages.size()-1 )
				texHeight = lastTextureHeight;

			// make a new bitmap
			HBITMAP bmp = CreateCompatibleBitmap(dc, textureWidth, texHeight);
			HDC bmpdc = CreateCompatibleDC(dc);

			LOGBRUSH lbrush;
			lbrush.lbColor = RGB(0,0,0);
			lbrush.lbHatch = 0;
			lbrush.lbStyle = BS_SOLID;

			HBRUSH brush = CreateBrushIndirect(&lbrush);
			HPEN pen = CreatePen(PS_NULL, 0, 0);

			HGDIOBJ oldbmp = SelectObject(bmpdc, bmp);
			HGDIOBJ oldbmppen = SelectObject(bmpdc, pen);
			HGDIOBJ oldbmpbrush = SelectObject(bmpdc, brush);
			HGDIOBJ oldbmpfont = SelectObject(bmpdc, font);

			SetTextColor(bmpdc, RGB(255,255,255));

			Rectangle(bmpdc, 0,0,textureWidth,texHeight);
			SetBkMode(bmpdc, TRANSPARENT);

			// draw the letters...

			// iterate through the tree
			core::map<wchar_t, u32>::Iterator it = CharMap.getIterator();
			while (!it.atEnd())
			{
				s32 currentArea = (*it).getValue();
				wchar_t wch = (*it).getKey();
				// sloppy but I couldnt be bothered rewriting it
				if (Areas[currentArea].sourceimage == currentImage)
				{
					// draw letter
					s32 sx = Areas[currentArea].rectangle.UpperLeftCorner.X - Areas[currentArea].underhang;
					TextOutW(bmpdc, sx, Areas[currentArea].rectangle.UpperLeftCorner.Y, &wch, 1);

					// if ascii font...
					//SetPixel(bmpdc, Areas[currentArea].rectangle.UpperLeftCorner.X, Areas[currentArea].rectangle.UpperLeftCorner.Y, RGB(255,255,0));// left upper corner mark
				}
				it++;
			}

			// copy the font bitmap into a new irrlicht image
			BITMAP b;
			PBITMAPINFO pbmi;
			WORD    cClrBits;
			u32 cformat;

			// Retrieve the bitmap color format, width, and height.
			GetObject(bmp, sizeof(BITMAP), (LPSTR)&b);

			// Convert the color format to a count of bits.
			cClrBits = (WORD)(b.bmPlanes * b.bmBitsPixel);

			if (cClrBits <= 8) // we're not supporting these
				cformat = -1;
			else if (cClrBits <= 16)
				cformat = video::ECF_A1R5G5B5;
			else if (cClrBits <= 24)
				cformat = video::ECF_R8G8B8;
			else
				cformat = video::ECF_A8R8G8B8;

			pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
						sizeof(BITMAPINFOHEADER));

			// Initialize the fields in the BITMAPINFO structure.

			pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbmi->bmiHeader.biWidth = b.bmWidth;
			pbmi->bmiHeader.biHeight = b.bmHeight;
			pbmi->bmiHeader.biPlanes = b.bmPlanes;
			pbmi->bmiHeader.biBitCount = b.bmBitsPixel;

			// If the bitmap is not compressed, set the BI_RGB flag.
			pbmi->bmiHeader.biCompression = BI_RGB;

			// Compute the number of bytes in the array of color
			// indices and store the result in biSizeImage.
			// For Windows NT, the width must be DWORD aligned unless
			// the bitmap is RLE compressed. This example shows this.
			// For Windows 95/98/Me, the width must be WORD aligned unless the
			// bitmap is RLE compressed.
			pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
							* pbmi->bmiHeader.biHeight;
			// Set biClrImportant to 0, indicating that all of the
			// device colors are important.
			pbmi->bmiHeader.biClrImportant = 0;

			LPBYTE lpBits;              // memory pointer

			PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER) pbmi;
			lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

			GetDIBits(dc, bmp, 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS);

			// DEBUG- copy to clipboard
			//OpenClipboard(hWnd);
			//EmptyClipboard();
			//SetClipboardData(CF_BITMAP, bmp);
			//CloseClipboard();

			// flip bitmap
			s32 rowsize = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8;
			c8 *row = new c8[rowsize];
			for (s32 i=0; i < (pbih->biHeight/2); ++i)
			{
				// grab a row
				memcpy(row, lpBits + (rowsize * i), rowsize);
				// swap row
				memcpy(lpBits + (rowsize * i), lpBits + ((pbih->biHeight-1 -i) * rowsize ) , rowsize);
				memcpy(lpBits + ((pbih->biHeight-1 -i) * rowsize ), row , rowsize);
			}

			bool ret = false;

			if (cformat == video::ECF_A8R8G8B8)
			{
				// in this case the font should have an alpha channel, but since windows doesn't draw one
				// we have to set one manually by going through all the pixels.. *sigh*

				u8* m;
				for (m = lpBits; m < lpBits + pbih->biSizeImage; m+=4)
				{
					if (UseAlphaChannel)
					{
						if (m[0] > 0) // pixel has colour
						{
							m[3]=m[0];  // set alpha
							m[0]=m[1]=m[2] = 255; // everything else is full
						}
					}
					else
						m[3]=255; // all pixels are full alpha
				}

			}
			else if (cformat == video::ECF_A1R5G5B5)
			{
				u8* m;
				for (m = lpBits; m < lpBits + pbih->biSizeImage; m+=2)
				{
					WORD *p = (WORD*)m;
					if (m[0] > 0 || !UseAlphaChannel) // alpha should be set
						*p |= 0x8000; // set alpha bit
				}
			}
			else
			{
				cformat = -1;
			}

			// make a texture from the image
			if (cformat != -1)
			{
				// turn mip-mapping off
				bool b = Device->getVideoDriver()->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
				currentImages[currentImage] = Device->getVideoDriver()->createImageFromData((video::ECOLOR_FORMAT)cformat, core::dimension2d<s32>(textureWidth,texHeight), (void*)lpBits);
				Device->getVideoDriver()->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS,b);
			}
			else
			{
				Device->getLogger()->log("Couldn't create font, your pixel format is unsupported.");
			}

			// free memory and windows resources
			// sloppy I know, but I only intended to create one image at first.
			delete [] row;
			LocalFree(pbmi);
			GlobalFree(lpBits);
			DeleteDC(bmpdc);
			DeleteObject(brush);
			DeleteObject(pen);
			DeleteObject(bmp);

			if (currentImages[currentImage])
			{
				// add texture
				currentTextures[currentImage] = Device->getVideoDriver()->addTexture("GUIFontImage",currentImages[currentImage]);
				currentTextures[currentImage]->grab();
			}
			else
			{
				Device->getLogger()->log("Something went wrong, aborting.");
				// drop all images
				DeleteObject(font);
				return false;
			}
		} // looping through each texture
		DeleteObject(font);
		return true;
	}

#else

	/*
		Currently only windows is supported.
		If anyone makes a Linux/OSX implementation, please post a patch to the tracker for inclusion :)
	*/

	CFontTool::CFontTool(IrrlichtDevice *device) : FontSizes(fontsizes),
		Device(device), UseAlphaChannel(false)
	{
		device->setWindowCaption(L"Unable to create fonts, your OS is not supported :-(");

		int fontCount = 0;
		char **fontList = XListFonts((Display*)device->getVideoDriver()->getExposedVideoData().OpenGLLinux.X11Display, "-*", 2048, &fontCount);

		for (int i=0; i<fontCount; ++i)
		{
			core::stringw tmp(fontList[i]);
			tmp = tmp.subString(1, tmp.findNext(L'-', 1)-1);
			if (!CharSets.size() || (tmp != CharSets.getLast()))
				CharSets.push_back( tmp );
		}
		XFreeFontNames(fontList);

		selectCharSet(0);
	}

	void CFontTool::selectCharSet(u32 currentCharSet)
	{
		if ( currentCharSet >= CharSets.size() )
			return;

		FontNames.clear();

		int fontCount = 0;
		char familyName[1024];
		snprintf(familyName, 1024, "-%ls-*", CharSets[currentCharSet].c_str());
		char **fontList = XListFonts((Display*)Device->getVideoDriver()->getExposedVideoData().OpenGLLinux.X11Display, familyName, 2048, &fontCount);

		for (int i=0; i<fontCount; ++i)
		{
			core::stringw tmp(fontList[i]);
			s32 pos = tmp.findNext(L'-', 1)+1;
			tmp = tmp.subString(pos, tmp.findNext(L'-', pos)-pos);
			if (!FontNames.size() || (tmp != FontNames.getLast()))
				FontNames.push_back( tmp );
		}
		XFreeFontNames(fontList);
	}

	bool CFontTool::makeBitmapFont(	u32 fontIndex, u32 charsetIndex,
					s32 fontSize, u32 texturewidth, u32 textureHeight,
					bool bold, bool italic, bool aa, bool alpha)
	{
		if (fontIndex >= FontNames.size() || charsetIndex >= CharSets.size() )
			return false;

		Display* display = (Display*)Device->getVideoDriver()->getExposedVideoData().OpenGLLinux.X11Display;
		UseAlphaChannel = alpha;
		u32 currentImage = 0;

		char familyName[1024];
		snprintf(familyName, 1024, "-%ls-%ls-%s-%s-*", CharSets[charsetIndex].c_str(), FontNames[fontIndex].c_str(), bold?"bold":"medium", italic?"i":"r");
		XFontStruct* fontStruct = XLoadQueryFont(display, familyName);
		if (!fontStruct)
			return false;

		// get rid of the current textures/images
		for (u32 i=0; i<currentTextures.size(); ++i)
			currentTextures[i]->drop();
		currentTextures.clear();

		for (u32 i=0; i<currentImages.size(); ++i)
			currentImages[i]->drop();
		currentImages.clear();

		// clear current image mappings
		CharMap.clear();
		// clear array
		Areas.clear();

		printf("dir %u minc %u maxc %u min %u max %u, all %d def %u #prop %d asc %d desc %d\n",
			fontStruct->direction,
			fontStruct->min_char_or_byte2,
			fontStruct->max_char_or_byte2,
			fontStruct->min_byte1,
			fontStruct->max_byte1,
			fontStruct->all_chars_exist,
			fontStruct->default_char,
			fontStruct->n_properties,
			fontStruct->ascent,
			fontStruct->descent);
#if 0
			XFontProp *properties;
			XCharStruct min_bounds;  /* minimum bounds over all existing char */
			XCharStruct max_bounds;  /* maximum bounds over all existing char */
			XCharStruct *per_char;   /* first_char to last_char information */
#endif

#if 0
		for (s32 ch=current->wcLow; ch< current->wcLow + current->cGlyphs; ch++)
		{
			wchar_t currentchar = ch;

			// get the dimensions
			SIZE size;
			ABC abc;
			GetTextExtentPoint32W(dc, &currentchar, 1, &size);
			SFontArea fa;
			fa.underhang = 0;
			fa.overhang  = 0;

			if (GetCharABCWidthsW(dc, currentchar, currentchar, &abc)) // for unicode fonts, get overhang, underhang, width
			{
				size.cx = abc.abcB;
				fa.underhang  = abc.abcA;
				fa.overhang   = abc.abcC;

				if (abc.abcB-abc.abcA+abc.abcC<1)
					continue;	// nothing of width 0
			}
			if (size.cy < 1)
				continue;

			// wrap around?
			if (currentx + size.cx > (s32) textureWidth)
			{
				currenty += maxy;
				currentx = 0;
				if ((u32)(currenty + maxy) > textureHeight)
				{
					currentImage++; // increase Image count
					currenty=0;
				}
				maxy = 0;
			}
			// add this char dimension to the current map

			fa.rectangle = core::rect<s32>(currentx, currenty, currentx + size.cx, currenty + size.cy);
			fa.sourceimage = currentImage;

			CharMap.insert(currentchar, Areas.size());
			Areas.push_back( fa );

			currentx += size.cx +1;

			if (size.cy+1 > maxy)
				maxy = size.cy+1;
		}
		currenty += maxy;

		u32 lastTextureHeight = getTextureSizeFromSurfaceSize(currenty);

		// delete the glyph set
		delete buf;

		currentImages.set_used(currentImage+1);
		currentTextures.set_used(currentImage+1);

		for (currentImage=0; currentImage < currentImages.size(); ++currentImage)
		{
			core::stringc logmsg = "Creating image ";
			logmsg += (s32) (currentImage+1);
			logmsg += " of ";
			logmsg += (s32) currentImages.size();
			Device->getLogger()->log(logmsg.c_str());
			// no need for a huge final texture
			u32 texHeight = textureHeight;
			if (currentImage == currentImages.size()-1 )
				texHeight = lastTextureHeight;

			// make a new bitmap
		}
#endif

		XFreeFont(display, fontStruct);
		Device->getLogger()->log("Your OS is unsupported! It won't work I tell you!");
		return false;
	}
#endif

	CFontTool::~CFontTool()
	{
#ifdef _IRR_WINDOWS_
		// destroy display context
		if (dc)
			DeleteDC(dc);
#endif

		// drop textures+images
		for (u32 i=0; i<currentTextures.size(); ++i)
			currentTextures[i]->drop();
		currentTextures.clear();

		for (u32 i=0; i<currentImages.size(); ++i)
			currentImages[i]->drop();
		currentImages.clear();
	}

bool CFontTool::saveBitmapFont(const c8 *filename, const c8* format)
{
	if (currentImages.size() == 0)
	{
		Device->getLogger()->log("No image data to write, aborting.");
		return false;
	}

	core::stringc fn = filename;
	core::stringc imagename = filename;
	fn += ".xml";

	io::IXMLWriter *writer = Device->getFileSystem()->createXMLWriter(fn.c_str());

	// header and line breaks
	writer->writeXMLHeader();
	writer->writeLineBreak();

	// write information
	writer->writeElement(L"font", false, L"type", L"bitmap");
	writer->writeLineBreak();
	writer->writeLineBreak();

	// write images and link to them
	for (u32 i=0; i<currentImages.size(); ++i)
	{
		imagename = filename;
		imagename += (s32)i;
		imagename += ".";
		imagename += format;
		Device->getVideoDriver()->writeImageToFile(currentImages[i],imagename.c_str());

		writer->writeElement(L"Texture", true,
				L"index", core::stringw(i).c_str(),
				L"filename", core::stringw(imagename.c_str()).c_str(),
				L"hasAlpha", UseAlphaChannel ? L"true" : L"false");
		writer->writeLineBreak();
	}

	writer->writeLineBreak();

	// write each character
	core::map<wchar_t, u32>::Iterator it = CharMap.getIterator();
	while (!it.atEnd())
	{
		SFontArea &fa = Areas[(*it).getValue()];

		wchar_t c[2];
		c[0] = (*it).getKey();
		c[1] = L'\0';
		core::stringw area, under, over, image;
		area  = fa.rectangle.UpperLeftCorner.X;
		area += L", ";
		area += fa.rectangle.UpperLeftCorner.Y;
		area += L", ";
		area += fa.rectangle.LowerRightCorner.X;
		area += L", ";
		area += fa.rectangle.LowerRightCorner.Y;

		core::array<core::stringw> names;
		core::array<core::stringw> values;
		names.clear();
		values.clear();
		// char
		names.push_back(core::stringw(L"c"));
		values.push_back(core::stringw(c));
		// image number
		if (fa.sourceimage != 0)
		{
			image = (s32) fa.sourceimage;
			names.push_back(core::stringw(L"i"));
			values.push_back(image);
		}
		// rectangle
		names.push_back(core::stringw(L"r"));
		values.push_back(area);

		if (fa.underhang != 0)
		{
			under = fa.underhang;
			names.push_back(core::stringw(L"u"));
			values.push_back(under);
		}
		if (fa.overhang != 0)
		{
			over = fa.overhang;
			names.push_back(core::stringw(L"o"));
			values.push_back(over);
		}
		writer->writeElement(L"c", true, names, values);

		writer->writeLineBreak();
		it++;
	}

	writer->writeClosingTag(L"font");

	writer->drop();

	Device->getLogger()->log("Bitmap font saved.");

	return true;
}
