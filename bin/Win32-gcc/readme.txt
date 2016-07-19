If you wish to compile Irrlicht for Win32-gcc you have several choices.

1. You can work from within a MinGW shell.
Go to the folder source/Irrlicht and run the Makefile with:
make win32 
Examples can be build by going into the folder of the example (for example examples/01.HelloWorld) and running the Makefile with:
make all_win32

2. Use the Code::Blocks IDE
There is a project file called Irrlicht-gcc.cbp in source/Irrlicht to compile just the engine.
Be sure to select a Windows target like "Win32 - release - accurate math - dll"

There is also Code::Blocks workspace file in the examples folder called BuildAllExamples.workspace
Again be sure to select a Windows target like "Win32 - release - accurate math - dll"
This workspace allows you to compile the engine together with all examples and tools.

