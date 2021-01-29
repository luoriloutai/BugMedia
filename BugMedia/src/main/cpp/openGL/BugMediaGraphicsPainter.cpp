//
// Created by Gshine on 2021/1/27.
//

// debug
// 调试用，最后移除这一项即可
//#define DEBUGIT



#ifdef DEBUGIT

#include "../Debug.h"

#else

#include "core/BugMediaGraphicsBaseRenderer.h"
#include <jni.h>
#include "BugMediaGraphicsPainter.h"
#include "BugMediaTriangleRenderer.h"
#include "core/BugMediaGraphicsCommon.h"

#endif




//^^^^^^^^^^^ jni ^^^^^^^^^^^^

extern "C" {

#ifdef DEBUGIT

#else

// 这个要与Java里的值对应
enum rendererType {
    TRIANGLE_RENDERER = 1,
    BITMAP_RENDERER = 2,
    VIDEO_RENDERER = 3
};

BugMediaGraphicsBaseRenderer *renderer = NULL;

#endif

void initRenderer(int renderType) {

#ifdef DEBUGIT

#else
    if (renderType == TRIANGLE_RENDERER) {
        renderer = new BugMediaTriangleRenderer();
    } else if (renderType == BITMAP_RENDERER) {

    } else if (renderType == VIDEO_RENDERER) {

    }
#endif

}


} // extern C


extern "C" JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_setWindowSurface(JNIEnv *env, jobject clazz, jobject surface) {
#ifdef DEBUGIT
    testCreate(getWindow(env, surface));
#else
    if (renderer != NULL) {
        renderer->setWindowSurface(env, surface);
    }
#endif
}


extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_setPBufferSurface(JNIEnv *env, jobject clazz, jint width, jint height) {
#ifdef DEBUGIT
#else
    if (renderer != NULL) {
        renderer->setPBufferSurface(width, height);
    }
#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_draw(JNIEnv *env, jobject clazz) {
#ifdef DEBUGIT
#else
    if (renderer != NULL) {
        renderer->draw();
    }
#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_selectRenderer(JNIEnv *env, jobject clazz, jint rendererType) {
#ifdef DEBUGIT

#else
    initRenderer(rendererType);
    if (renderer==NULL){
        LOGE("初始化渲染器失败");
    }
    LOGD("渲染器初始化成功");
#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_destroy(JNIEnv *env, jobject clazz) {
#ifdef DEBUGIT
#else
    if (renderer != NULL) {
        delete renderer;
    }
#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_pause(JNIEnv *env, jobject clazz) {
#ifdef DEBUGIT

#else

#endif
}

