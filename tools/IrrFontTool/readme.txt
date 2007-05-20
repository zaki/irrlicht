There are two tools available for creating fonts for irrlicht, both are supported.

oldFontTool:
	only works in Windows, creates a simple image file containing all data for displaying fonts.
	Those file contain no alpha informations and are limited in their character set.
	use the IrrFontTool.exe file in this directory.

newFontTool: 
	a more sophisticated font tool supporting alpha channels, anti aliasing, 
	different character sets, vector fonts and other operating systems than
	just windows. It will create multiple image files and an .xml file 
	containing meta information for the generated font.
	You can find it as FontTool.exe in the /bin directory.