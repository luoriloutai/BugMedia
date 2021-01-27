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


class BugMediaGraphics {
public:
    BugMediaGraphics();

    void setShaderSource(const GLchar **const vertexShadersource, const GLchar **const fragmentShadersource);

    void setWindowSurface(JNIEnv *env, jobject jSurface);

    void setPBufferSurface(EGLint width, EGLint height);

    // 绘制之前的配置等不变的东西在这里写
    virtual void prepare() = 0;

    // 真正绘制的地方调用draw()会调用
    virtual void onDraw() = 0;


    void release();

    virtual ~BugMediaGraphics(); // 防止多态性导致的子类析构函数不执行。有虚方法的类的析构函数一般应定义为虚析构函数

    void draw();

    void setViewPort(GLint x, GLint y, GLsizei width, GLsizei height);

private:
    void drawingFunction();
    // 绘制时开启一个线程，然后调用该方法将上下文绑定至线程
    void makeCurrent();

    BugMediaGraphicsEGL *pEGL = NULL;
    BugMediaGraphicsGLES *pGLES = NULL;
    GLboolean isRelease = GL_FALSE;
};


#endif //SLOWER_BUGMEDIAGRAPHICS_H
