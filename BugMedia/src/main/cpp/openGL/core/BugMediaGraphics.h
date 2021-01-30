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
#include <pthread.h>


class BugMediaGraphics {
public:
    BugMediaGraphics();

    void setWindowSurface(JNIEnv *env, jobject jSurface);

    void setPBufferSurface(EGLint width, EGLint height);

    void release();

    virtual ~BugMediaGraphics(); // 防止多态性导致的子类析构函数不执行。有虚方法的类的析构函数一般应定义为虚析构函数

    void draw(GLboolean repeat);

    void setViewPort(GLint x, GLint y, GLsizei width, GLsizei height);

    void resize(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
    BugMediaGraphicsEGL *pEGL = NULL;
    BugMediaGraphicsGLES *pGLES = NULL;

private:
    // 设置Shader代码
    virtual void setShaderSource() = 0;

    // 绘制前准备工作：设置shader里的变量值、配置各种在绘制时不变的信息
    virtual void prepareDraw() = 0;

    // 开始绘制
    virtual void startDraw() = 0;

    GLboolean isRelease = GL_FALSE;

    // C的线程函数，必须为静态
    static void *drawBackground(void *pVoid);

    void drawingThreadFun(BugMediaGraphics *graphics);

    pthread_t drawThread;
    pthread_mutex_t tMutex;
    pthread_cond_t tCond;

    enum State{
        STOP,
        PAUSE,
        RUNNING
    };

    State runState=RUNNING;
    GLboolean repeat=GL_FALSE;

};


#endif //SLOWER_BUGMEDIAGRAPHICS_H
