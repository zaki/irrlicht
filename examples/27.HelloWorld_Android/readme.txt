To use Android you need to have installed:
- Android SDK (from http://developer.android.com)
- Android NDK (from http://developer.android.com)
- ant (a build tool commonly used for Java)
- A Java jdk (for example openjdk-6-jdk)

1. Assign your Android SDK path to an ANDROID_HOME environment variable.
2. Add $ANDROID_HOME/tools and $ANDROID_HOME/platform-tools and the Android NDK main folder to your PATH environment variable.
3. Go to: source->Irrlicht->Android and call "ndk-build" or "ndk-build NDEBUG=1"
4. Go to: examples->27.HelloWorld_Mobile and call "ndk-build" or "ndk-build NDEBUG=1"
5. Call "ant debug" to create package
6. Connect device to PC (with USB debugging mode ON) or turn on emulator.
7. Call "adb -d install bin/HelloWorldMobile-debug.apk" (if you use emulator please add "-e" parameter instead of "-d") to install package on your device/emulator.

Troubleshooting:

Error: Unable to resolve project target 'android-10'
Solution: Run "android sdk" in sdk/tools and install API 10
