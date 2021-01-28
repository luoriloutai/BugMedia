//
// Created by Gshine on 2021/1/27.
//
#include "core/BugMediaGraphicsBaseRenderer.h"
#include <jni.h>
#include "BugMediaGraphicsPainter.h"
#include "BugMediaTriangleRenderer.h"
#include "core/BugMediaGraphicsCommon.h"

//^^^^^^^^^^^ jni ^^^^^^^^^^^^

extern "C" {

// 这个要与Java里的值对应
enum rendererType {
    TRIANGLE_RENDERER = 1,
    BITMAP_RENDERER = 2,
    VIDEO_RENDERER = 3
};


BugMediaGraphicsBaseRenderer *renderer = NULL;

void initRenderer(int renderType) {
    if (renderType == TRIANGLE_RENDERER) {
        renderer = new BugMediaTriangleRenderer();
    } else if (renderType == BITMAP_RENDERER) {

    } else if (renderType == VIDEO_RENDERER) {

    }
}


} // extern C


extern "C" JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_setWindowSurface(JNIEnv *env, jobject clazz, jobject surface) {
    if (renderer != NULL) {
        renderer->setWindowSurface(env, surface);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_setViewport(JNIEnv *env, jobject clazz, jint x, jint y, jint width,
                                                   jint height) {
    if (renderer != NULL) {
        renderer->setViewPort(x, y, width, height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_setPBufferSurface(JNIEnv *env, jobject clazz, jint width, jint height) {
    if (renderer != NULL) {
        renderer->setPBufferSurface(width, height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_draw(JNIEnv *env, jobject clazz) {

    if (renderer != NULL) {
        renderer->draw();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_selectRenderer(JNIEnv *env, jobject clazz, jint rendererType) {

    initRenderer(rendererType);
    if (renderer==NULL){
        LOGE("初始化渲染器失败");
    }
    LOGD("渲染器初始化成功");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_destroy(JNIEnv *env, jobject clazz) {
    if (renderer != NULL) {
        delete renderer;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_pause(JNIEnv *env, jobject clazz) {
    // TODO: implement pause()
}

