------------
REQUIREMENTS
------------

To use Android you need to have installed:
- Android SDK (from http://developer.android.com)
- Android NDK (from http://developer.android.com)
- ant (a build tool commonly used for Java)
- A Java jdk (for example openjdk-6-jdk)
- GNU Make 3.81 or later
- A recent version of awk
- On Windows you need to have Cygwin (at least version 1.7) installed 

----------------------------
BUILDING Irrlicht & your App
----------------------------

1. Assign your Android SDK path to an ANDROID_HOME environment variable.
2. Add $ANDROID_HOME/tools and $ANDROID_HOME/platform-tools and the Android NDK main folder to your PATH environment variable.
3. Go to: source->Irrlicht->Android and call "ndk-build" or "ndk-build NDEBUG=1"
4. Go to: examples->27.HelloWorld_Mobile and call "ndk-build" or "ndk-build NDEBUG=1"
5. Call "ant debug" to create package
6. Connect device to PC (with USB debugging mode ON) or turn on emulator.
7. Call "adb -d install bin/HelloWorldMobile-debug.apk" (if you use emulator please add "-e" parameter instead of "-d") to install package on your device/emulator.

Troubleshooting:

Error: Unable to resolve project target 'android-10'
Solution: Run "android sdk" in sdk/tools and install API 10. 
          Alternatively you can probably (not yet tested) set another APP_PLATFORM 
		  in the Application.mk's for the project and for Irrlicht. In this case you 
		  should likely also change the android:minSdkVersion in the AndroidManifest.xml
		  
-----
FILES
-----

AndroidManifest.xml: 
	Every Android application needs one of those to describe the needs of the application. 
	Must have exactly this name.
    See http://developer.android.com/guide/topics/manifest/manifest-intro.html
					 
build.xml: 
	Ant build file to create the final package. 
	You might want to create a new one as described in the Android documentation:
	http://developer.android.com/tools/projects/projects-cmdline.html
	That will then also update project.properties.
	
project.properties
	Contains the build target (and maybe other project properties). Must exist.

jni: 
	A folder by this name must exist below the folder where you have build.xml. 
	Usually it contains the native (c/c++) source files, but in our case we put 
	the source-files one level higher (with LOCAL_PATH in Android.mk).
	 
jni/Android.mk: 
	The Makefile for the project. 
	Source-files in the project are added to LOCAL_SRC_FILES
	In the Irrlicht example it also copies the assets, but you can 
	also already create your project assets in the right place.
				
jni/Application.mk: 
	Optional file which for example restricts which modules are installed and 
	where you can set specific target architectures.
	More info about this can be found in the ndk docs.
					
res: 
	A folder with resources which districuted with your application and can be accessed via ID's. 
	Unfortunately no direct NDK access to resources at the time of writing this. So you either have
	to access them with java-code and copy to c++ somehow or you have to use hacks to read the format
	directly (which is done by some apps, but not future-safe and google recommends not doing that).
	Please check the official "App Resources" android developer documention, as this is rather complex.
	We use it only for the application icons in this example.
	 
assets: 
	Files in here are distributed with your app. It's acting like a read-only file system.
	
assets/media/Shaders: 
	Shader code needed by the OGLES2 driver to simulate a fixed function pipeline. 
	In the example this code is automatically copied within the Android.mk makefile.
	The path where the shaders are searched is set in the IRR_OGLES2_SHADER_PATH define in IrrCompileConfig.h
	The names are hardcoded so they have to be identical to those found in media/Shaders.
	You can rewrite the shaders, but ensure to add some working shaders files by those names.
	The OGLES1 driver doesn't need those files.

obj: 
	All object and library files mentioned in the Android.mk are put in here before linking.

libs: 
	Contains the binaries of your application after compilation. The application itself is a lib(probably because native code can't run directly but only as lib).

src: 
	The src folder is needed when you have Java sources and should only contain .java and .aidl files.
	Although the examples doesn't use Java the makefile creates this folder as the ant build.xml in the android sdk needs it.
