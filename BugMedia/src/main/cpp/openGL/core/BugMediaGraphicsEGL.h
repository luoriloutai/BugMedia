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


class BugMediaGraphicsEGL {
public:
    BugMediaGraphicsEGL();

    EGLBoolean setWindowSurface(JNIEnv *env, jobject jSurface);

    EGLBoolean setPBufferSurface(EGLint width, EGLint height);

    EGLBoolean swapBuffers();

    void makeCurrent();

    void release();

    ~BugMediaGraphicsEGL();

private:
    EGLDisplay display = NULL;
    EGLConfig config = NULL;
    EGLContext context = NULL;
    EGLSurface windowSurface = NULL;
    EGLSurface PBufferSurface = NULL;
    ANativeWindow *window = NULL;
    EGLBoolean isRelease = EGL_FALSE;

    EGLBoolean init();
};


#endif //SLOWER_BUGMEDIAGRAPHICSEGL_H
