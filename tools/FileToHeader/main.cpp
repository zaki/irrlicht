// Copyright (C) 2012 Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

/***
	This tool creates a .h file from a given input file by encoding it into a C string,
	allowing you to build your resources directly into your binaries, just like Irrlicht's
	built-in font.

	To distribute your app as a single executable file of minimal size:

	1. Put all your resources into a single directory and add it to Irrlicht's filesystem
	   as a folder through IFileSystem::addArchive. Develop and test your app as usual.
	2. Open IrrCompileConfig.h and comment out all the options that your app does not use.
	   This will reduce the size of the Irrlicht library considerably.
	     * You should remove the D3D video drivers, because they rely on external DLLs.
	     * You should keep either the TAR or ZIP archive loader, used in step 6.
	     * If you remove the JPEG loader, you don't have to say you use JPEG code in your
	       documentation.
	3. Recompile Irrlicht as a static library, editing the IRR_STATIC_LIB line in
	   IrrCompileConfig.h.
	   The next time you compile your application it will take a while longer, but
	   Irrlicht will be built into your binary.
	4. TAR or ZIP your resource directory using your favourite archiving tool (ie 7zip).
	     * If you chose ZIP but compiled without zlib, don't compress this archive or it
	       won't open.
	5. Run this tool to convert your resource file into a .h file, like so:
	     FileToHeader res.zip > EmbeddedResources.h
	6. Add the .h file to your project, create the embedded read file then mount as a
	   ZIP or TAR archive instead of the folder, like so:
	     io::IReadFile *f = io::createEmbeddedFile(device->getFileSystem(), "res.zip");
	     device->getFileSystem()->addFileArchive(f);
	     archive->drop();
	7. Recompile your app.
	     * If you use Microsoft's compiler, make sure your CRT (common run-time) is
	       the static library version, otherwise you'll have a dependency on the CRT DLLs.
	   Your binary should now be completely portable; you can distribute just the exe file.
	8. Optional: Use UPX (upx.sf.net) to compress your binary.
*/

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		// print usage
		cerr << "You must to specify at least one input file" << endl;
		cerr << "usage: " << argv[0] << "<file1> [file2...]" << endl;
		cerr << "outputs a header file to stdout, so for example use";
		return 1;
	}

	int i = 1;

	// write file header
	cout << "// File made by FileToHeader, part of the Irrlicht Engine" << endl
	     << endl
	     << "#ifndef _EMBEDDED_FILES_H_INCLUDED_" << endl
	     << "#define _EMBEDDED_FILES_H_INCLUDED_" << endl
	     << endl
	     << "#include \"irrTypes.h\"" << endl
	     << "#include \"IReadFile.h\"" << endl
	     << "#include \"IFileSystem.h\"" << endl
	     << endl
	     << "namespace irr" << endl
	     << "{" << endl
	     << "namespace io" << endl
	     << "{" << endl
	     << endl
	     << "	const c8* EmbeddedFileData[] = " << endl
	     << "	{" << endl;

	// store sizes and file names
	stringstream sizes;
	stringstream names;
	sizes << "const u32 EmbeddedFileSizes[] = {";
	names << "const c8* EmbeddedFileNames[] = {";
	int fileCount = 0;

	// char to hex digit table, probably doesn't help for speed due to fstream. better than using sprintf though
	char hextable[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

	while (i < argc)
	{
		// open and seek to end of file
		ifstream input;
		input.open(argv[i], ios::in | ios::binary | ios::ate);

		if (input.is_open())
		{
			int size = input.tellg();
			input.seekg(0, ios::beg);
			// read the file into RAM
			char *entireFile = new char[size];
			input.read(entireFile, size);

			if (fileCount)
			{
				sizes << ", ";
				names << ", ";
				cout  << "," << endl;
			}

			// save file size and name
			sizes << size;
			names << '"' << argv[i] << '"';

			// write the file data
			cout << "		\"";
			for (int count=0; count < size; ++count)
			{
				if (count && (count % 16) == 0)
					cout << "\"" << endl << "		\"";

				cout << "\\x" << hextable[(entireFile[count] >> 4) & 0xF] << hextable[entireFile[count] & 0x0F];
			}
			cout << "\"";

			delete [] entireFile;
			//
			input.close();

			fileCount++;
		}
		else
		{
			cerr << "Failed to open file: " << argv[i] << endl;
		}

		++i;
	}

	// close binary file list and write file metadata
	cout << endl
	     << "		, 0};" << endl
	     << endl
	     << "	const u32 EmbeddedFileCount = " << fileCount << ";" << endl
	     << "	" << sizes.str() << "};" << endl
	     << "	" << names.str() << "};" << endl
	     << endl;

	// write functions to create embedded IReadFiles
	cout << "	IReadFile* createEmbeddedFile(IFileSystem *fs, u32 index)" << endl
	     << "	{" << endl
	     << "		if (EmbeddedFileCount < index)" << endl
	     << "			return 0;" << endl
	     << endl
	     << "		return fs->createMemoryReadFile((void*)EmbeddedFileData[index], " << endl
	     << "		                                EmbeddedFileSizes[index], EmbeddedFileNames[index]);" << endl
	     << "	}" << endl
	     << endl
	     << "	IReadFile* createEmbeddedFile(IFileSystem *fs, path filename)" << endl
	     << "	{" << endl
	     << "		for (u32 i=0; i < EmbeddedFileCount; ++i)" << endl
	     << "			if (filename == EmbeddedFileNames[i])" << endl
	     << "				return createEmbeddedFile(fs, i);" << endl
	     << endl
	     << "		return 0;" << endl
	     << "	}" << endl
	     << endl;

	// write footer
	cout << "} // namespace io"  << endl
	     << "} // namespace irr" << endl
	     << endl
	     << "#endif // _EMBEDDED_FILES_H_INCLUDED_" << endl;

	return 0;
}

