//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphics.h"
#include "BugMediaGraphicsCommon.h"
#include <thread>
#include <unistd.h>

using namespace std;

BugMediaGraphics::BugMediaGraphics() {

    // 由于绘图对象都要求在同一线程中，
    // 所以设计时构造函数里只初始化与绘图对象无关的对象，
    // 这些对象值可以在最终创建绘图对象时使用
    pEGL = new BugMediaGraphicsEGL();
    pGLES = new BugMediaGraphicsGLES();

}


void BugMediaGraphics::setWindowSurface(JNIEnv *env, jobject jSurface) {
    pEGL->setWindowSurface(env, jSurface);
}

void BugMediaGraphics::setPBufferSurface(EGLint width, EGLint height) {
    pEGL->setPBufferSurface(width, height);
}


void BugMediaGraphics::draw() {
    // 虚方法
    setShaderSource(); // 该方法初始化了Shader中的代码数据，并没有真正创建Shader。初始化在init()里。
    //

    //C语言的线程
    pthread_create(&drawThread, NULL, drawBackground, this);

    ////
    //    // C++11的thread，使用外部线程函数，不能在线程内初始化EGL，作废
    //    thread drawThread(&BugMediaGraphics::drawingThreadFun, this,this);

    //    //
    //    // C++11的thread，使用内部匿名线程（lambda表达式），同样不能在线程内初始化EGL，作废
    //    thread tDrawThread([this]{
    //        pEGL->init();
    //        pEGL->makeCurrent();
    //
    //        pGLES->init();
    //        pGLES->activeProgram();
    //        // 虚方法
    //        prepareDraw();
    //        // 虚方法
    //        startDraw();
    //        pEGL->swapBuffers();
    //
    //        LOGD("绘制结束");
    //    });

}

void BugMediaGraphics::release() {
#ifdef DEBUGAPP
    LOGD("Graphics开始释放资源");
#endif
    if (!isRelease) {
        delete pGLES;
        pGLES = NULL;
        delete pEGL;
        pEGL = NULL;

        isRelease = GL_TRUE;
    }
#ifdef DEBUGAPP
    LOGD("Graphics释放资源完毕");
#endif
}

BugMediaGraphics::~BugMediaGraphics() {
    if (!isRelease) {
        release();
    }
}


void BugMediaGraphics::setViewPort(GLint x, GLint y, GLsizei width, GLsizei height) {
    pGLES->setViewport(x, y, width, height);
}

// C线程执行函数,必须静态
void *BugMediaGraphics::drawBackground(void *pVoid) {

    BugMediaGraphics *graphics = (BugMediaGraphics *) pVoid;

    graphics->pEGL->init();
    graphics->pEGL->makeCurrent();
    graphics->pGLES->init();
    graphics->pGLES->activeProgram();

    // 虚方法
    graphics->startDraw();

#ifdef DEBUGAPP
    LOGD("绘制结束");
#endif
    return 0;
}

//////
//// 使用C++11的thread时，EGL不能在线程函数里初始化，作废
//void BugMediaGraphics::drawingThreadFun(BugMediaGraphics *graphics) {
//    graphics->pEGL->init();
//    graphics->pEGL->makeCurrent();
//    graphics->pGLES->init();
//    graphics->pGLES->activeProgram();
//    // 虚方法，不频繁变化的配置
//    graphics->prepareDraw();
//
//    // 虚方法，经常发生变化的配置和绘制过程
//    graphics->startDraw();
//    graphics->pEGL->swapBuffers();
//}

void BugMediaGraphics::resize(GLint x, GLint y, GLsizei width, GLsizei height) {
    pGLES->resize(x, y, width, height);
}









