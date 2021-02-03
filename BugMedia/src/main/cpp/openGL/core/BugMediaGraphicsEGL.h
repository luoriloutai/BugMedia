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

    EGLBoolean makeCurrent();

    void release();

    ~BugMediaGraphicsEGL();

    void init();
    EGLBoolean init(EGLContext sharedContext);
    EGLint getViewWidth();
    EGLint getViewHeight();
    void resizeView(EGLint x, EGLint y, EGLint width, EGLint height);

private:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig config = NULL;
    EGLContext context = EGL_NO_CONTEXT;
    EGLSurface windowSurface = EGL_NO_SURFACE;
    EGLSurface PBufferSurface = EGL_NO_SURFACE;
    ANativeWindow *window = NULL;
    EGLBoolean isRelease = EGL_FALSE;

    surfaceTypeEnum surfaceType=NO_SURFACE;
    EGLint viewWidth = 0;
    EGLint viewHeight = 0;

};


#endif //SLOWER_BUGMEDIAGRAPHICSEGL_H
