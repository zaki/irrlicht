-----
SETUP
-----

To use Android you need to have installed:
- Android SDK (from http://developer.android.com)
- Android NDK (from http://developer.android.com)
- ant (a build tool commonly used for Java)
- A Java jdk (for example openjdk-6-jdk)
- GNU Make 3.81 or later
- A recent version of awk
- On Windows you need to have Cygwin (at least version 1.7) installed 

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
	Every Android application needs one of those to describe the needs of the applicaiton. 
	Must have exactly this name.
    See http://developer.android.com/guide/topics/manifest/manifest-intro.html
					 
build.xml: 
	Ant build file to create the final package

jni: 
	A folder by this name _must_ exist. 
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
	A folder with resources which are compiled into your application and can be accessed via ID's. 
	There is also support for things like different target resolutions.
	Please check the official "App Resources" android developer documention, as this is rather complex.
	 
assets: 
	Files in here are distributed with your app. It's acting like a typical file system.

obj: 
	(not sure... but seems all objects files needed for linking are put in here before linking - even the libs)

libs: 
	The result of compiling your application is always a lib (probably because native code can't run directly but only as lib).

src: 
	Created by our makefile because the ant build.xml in the android sdk needs it for some reason. It's empty (so far?).

----------
DEPLOYMENT
----------
When you require EDT_OGLES2 your android:glEsVersion in AndroidManifest.xml should be "0x00020000"
while for EDT_OGLES1 it should be "0x00010000".
