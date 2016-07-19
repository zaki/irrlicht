The Win32-VisualStudio version is currently (Irrlicht 1.8) compiled with VS 2010 using the Windows 7.1 SDK as platform toolset.
You might get the necessary Windows Platform SDK here: http://msdn.microsoft.com/en-us/windows/bb980924.aspx

To link to that Irrlicht.dll you need to set platform toolset in your VS version to the same target or re-compile the Irrlicht.dll using another platform toolset.

To re-compile Irrlicht for Win32-VisualStudio:
There are several project files for different VS versions in source/Irrlicht.
Irrlicht10.0.sln is for VS 2010
Irrlicht11.0.sln is for VS 2012
Irrlicht12.0.sln is for VS 2013

To compile Irrlicht + all examples and all tools check the BuildAllExamples_*.sln files in the examples folder.

For newer VS versions you have update one of those projects (VS usually can do that automatically when you open an older solution file).

Currently each of those solutions does set the platform toolset "Windows 7.1 SDK" (to be compatible to each other). 
You might want to change that in the project settings and set it to your current version.
Make sure you use the same platform toolset in your application and in the engine.
Also when compiling examples each example has to use the same platform toolset as was used for the engine.

Platform should be Win32
Configuration is by default "Release"
But you can also chose "Debug" if you want Irrlicht with Debug information.
Static builds are possible but you have to additionally set the _IRR_STATIC_LIB_ define in the application when linking to a static Irrlicht.lib

