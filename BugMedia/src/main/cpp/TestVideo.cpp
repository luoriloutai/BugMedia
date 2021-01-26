//
// Created by Gshine on 2021/1/23.
//
#include <jni.h>
#include <android/native_window_jni.h>

ANativeWindow* window;

//extern "C" void Java_com_bugmedia_media_TestVideoRenderer_setSurface(JNIEnv *env, jclass clazz, jobject surface)
//{
//    // obtain a native window from a Java surface
//    if (window) {
//        ANativeWindow_release(window);
//        window = NULL;
//    }
//    window = ANativeWindow_fromSurface(env, surface);
//}