//
// Created by Gshine on 2021/1/26.
//

#ifndef SLOWER_BUGMEDIAGRAPHICSEGL_H
#define SLOWER_BUGMEDIAGRAPHICSEGL_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <EGL/egl.h>
#include <EGL/eglext.h>


#ifdef __cplusplus
}
#endif

#include <jni.h>
#include "BugMediaGraphicsCommon.h"


class BugMediaGraphicsEGL {
public:
    BugMediaGraphicsEGL();

    void setWindowSurface(JNIEnv *env, jobject jSurface);

    void setPBufferSurface(EGLint width, EGLint height);

    EGLBoolean swapBuffers();

    void release();

    ~BugMediaGraphicsEGL();

    void init();

private:
    EGLDisplay display = NULL;
    EGLConfig config = NULL;
    EGLContext context = NULL;
    EGLSurface windowSurface = NULL;
    EGLSurface PBufferSurface = NULL;
    ANativeWindow *window = NULL;
    EGLBoolean isRelease = EGL_FALSE;
    EGLint width = 0;
    EGLint height = 0;
    EGLint surfaceType = NONE_SURFACE;

    EGLBoolean init(EGLContext sharedContext);
};


#endif //SLOWER_BUGMEDIAGRAPHICSEGL_H
