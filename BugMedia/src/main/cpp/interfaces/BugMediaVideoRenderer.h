//
// Created by Gshine on 2021/2/9.
//

#ifndef SLOWER_BUGMEDIAVIDEORENDERER_H
#define SLOWER_BUGMEDIAVIDEORENDERER_H
#include <jni.h>
#include "BugMediaRenderer.h"

#ifdef __cplusplus
extern "C"
{
#endif


#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

#ifdef __cplusplus
}
#endif

class BugMediaVideoRenderer:virtual public BugMediaRenderer{
public:
    virtual ~BugMediaVideoRenderer(){};
    virtual void setWindowSurface(JNIEnv *env, jobject jSurface)=0;
    virtual void setPBufferSurface(EGLint width, EGLint height)=0;
    virtual void resizeView(GLint x, GLint y, GLsizei width, GLsizei height)=0;
};


#endif //SLOWER_BUGMEDIAVIDEORENDERER_H
