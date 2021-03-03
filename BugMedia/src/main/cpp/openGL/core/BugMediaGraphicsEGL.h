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

    EGLint getViewWidth() const;

    EGLint getViewHeight() const;

    void resizeView(EGLint x, EGLint y, EGLint width, EGLint height);

private:
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface windowSurface;
    EGLSurface PBufferSurface;
    ANativeWindow *window;
    EGLBoolean isRelease;

    surfaceTypeEnum surfaceType;
    EGLint viewWidth;
    EGLint viewHeight;

};


#endif //SLOWER_BUGMEDIAGRAPHICSEGL_H
