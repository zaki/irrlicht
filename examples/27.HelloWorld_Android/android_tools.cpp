// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "android_tools.h"

namespace irr 
{
namespace android
{

// Not all DisplayMetrics are available through the NDK. 
// So we access the Java classes with the JNI interface.
// You can access other Java classes available in Android in similar ways.
// Function based roughly on the code from here: http://stackoverflow.com/questions/13249164/android-using-jni-from-nativeactivity
bool getDisplayMetrics(android_app* app, SDisplayMetrics & metrics)
{
	if (!app || !app->activity || !app->activity->vm )
		return false;
	
	JNIEnv* jni = 0;
	app->activity->vm->AttachCurrentThread(&jni, NULL);
	if (!jni )
		return false;

	
	// get all the classes we want to access from the JVM
	jclass classNativeActivity = jni->FindClass("android/app/NativeActivity");
	jclass classWindowManager = jni->FindClass("android/view/WindowManager");
	jclass classDisplay = jni->FindClass("android/view/Display");
	jclass classDisplayMetrics = jni->FindClass("android/util/DisplayMetrics");
	
	if (!classNativeActivity || !classWindowManager || !classDisplay || !classDisplayMetrics)
	{
		app->activity->vm->DetachCurrentThread();
		return false;
	}
	
	
	// Get all the methods we want to access from the JVM classes
	// Note: You can get the signatures (third parameter of GetMethodID) for all 
	// functions of a class with the javap tool, like in the following example for class DisplayMetrics:
	// javap -s -classpath myandroidpath/adt-bundle-linux-x86_64-20131030/sdk/platforms/android-10/android.jar android/util/DisplayMetrics
	jmethodID idNativeActivity_getWindowManager = jni->GetMethodID( classNativeActivity
												, "getWindowManager"
												, "()Landroid/view/WindowManager;"); 
	jmethodID idWindowManager_getDefaultDisplay = jni->GetMethodID( classWindowManager
												, "getDefaultDisplay"
												, "()Landroid/view/Display;");
	jmethodID idDisplayMetrics_constructor = jni->GetMethodID( classDisplayMetrics
														, "<init>"
														, "()V");
	jmethodID idDisplay_getMetrics = jni->GetMethodID( classDisplay
										 , "getMetrics"
										 , "(Landroid/util/DisplayMetrics;)V");
	
	if (!idNativeActivity_getWindowManager || !idWindowManager_getDefaultDisplay || !idDisplayMetrics_constructor 
		|| !idDisplay_getMetrics)
	{
		app->activity->vm->DetachCurrentThread();
		return false;
	}
	

	// In Java the following code would be: getWindowManager().getDefaultDisplay().getMetrics(metrics);
	// Note: If you need to call java functions in time-critical places you can split getting the jmethodID's 
	// and calling the functions into separate functions as you only have to get the jmethodID's once.
	jobject windowManager = jni->CallObjectMethod(app->activity->clazz, idNativeActivity_getWindowManager);
	
	if (!windowManager)
	{
		app->activity->vm->DetachCurrentThread();
		return false;
	}
	jobject display = jni->CallObjectMethod(windowManager, idWindowManager_getDefaultDisplay);
	if (!display)
	{
		app->activity->vm->DetachCurrentThread();
		return false;
	}
	jobject displayMetrics = jni->NewObject( classDisplayMetrics, idDisplayMetrics_constructor);
	if (!displayMetrics)
	{
		app->activity->vm->DetachCurrentThread();
		return false;
	}
	jni->CallVoidMethod(display, idDisplay_getMetrics, displayMetrics);
	
	// access the fields of DisplayMetrics (we ignore the DENSITY constants)
	jfieldID idDisplayMetrics_widthPixels = jni->GetFieldID( classDisplayMetrics, "widthPixels", "I");
	jfieldID idDisplayMetrics_heightPixels = jni->GetFieldID( classDisplayMetrics, "heightPixels", "I");
	jfieldID idDisplayMetrics_density = jni->GetFieldID( classDisplayMetrics, "density", "F");
	jfieldID idDisplayMetrics_densityDpi = jni->GetFieldID( classDisplayMetrics, "densityDpi", "I");
	jfieldID idDisplayMetrics_scaledDensity = jni->GetFieldID( classDisplayMetrics, "scaledDensity", "F");
	jfieldID idDisplayMetrics_xdpi = jni->GetFieldID(classDisplayMetrics, "xdpi", "F");
	jfieldID idDisplayMetrics_ydpi = jni->GetFieldID(classDisplayMetrics, "ydpi", "F");
	
	if ( idDisplayMetrics_widthPixels )
		metrics.widthPixels = jni->GetIntField(displayMetrics, idDisplayMetrics_widthPixels);
	if ( idDisplayMetrics_heightPixels )
		metrics.heightPixels = jni->GetIntField(displayMetrics, idDisplayMetrics_heightPixels);
	if (idDisplayMetrics_density )
		metrics.density = jni->GetFloatField(displayMetrics, idDisplayMetrics_density);
	if (idDisplayMetrics_densityDpi)
		metrics.densityDpi = jni->GetIntField(displayMetrics, idDisplayMetrics_densityDpi);
	if (idDisplayMetrics_scaledDensity)
		metrics.scaledDensity = jni->GetFloatField(displayMetrics, idDisplayMetrics_scaledDensity);
	if ( idDisplayMetrics_xdpi )
		metrics.xdpi = jni->GetFloatField(displayMetrics, idDisplayMetrics_xdpi);
	if ( idDisplayMetrics_ydpi )
		metrics.ydpi = jni->GetFloatField(displayMetrics, idDisplayMetrics_ydpi);
	
	app->activity->vm->DetachCurrentThread();
	return true;
}

} // namespace android
} // namespace irr
