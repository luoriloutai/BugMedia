//
// Created by Gshine on 2021/1/26.
//

#ifndef SLOWER_BUGMEDIAGRAPHICS_H
#define SLOWER_BUGMEDIAGRAPHICS_H

#include <jni.h>

#ifdef __cplusplus
extern "C"
{
#endif


#include <EGL/egl.h>
#include <EGL/eglext.h>

// OpenGL ES 2.0 渲染系统
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifdef __cplusplus
}
#endif

#include "BugMediaGraphicsEGL.h"
#include "BugMediaGraphicsGLES.h"
#include "BugMediaGraphicsShader.h"

class BugMediaGraphics {
public:
    BugMediaGraphics();

    void setVertexShader(const char **const source);

    void setFragmentShader(const char **const source);

    void setWindowSurface(JNIEnv *env, jobject jSurface);

    void setPBufferSurface(EGLint width, EGLint height);

    void prepareProgram();


    virtual void onProgramPrepared() {};

    virtual void onDraw() {};

    void release();

    ~BugMediaGraphics();

    void draw();

    void setViewPort(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
    void makeCurrent();

    BugMediaGraphicsEGL *pEGL = NULL;
    BugMediaGraphicsGLES *pGLES = NULL;
    BugMediaGraphicsShader *pVertextShader = NULL;
    BugMediaGraphicsShader *pFragmentShader = NULL;
    GLboolean isRelease = GL_FALSE;
};


#endif //SLOWER_BUGMEDIAGRAPHICS_H
