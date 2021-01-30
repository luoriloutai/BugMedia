//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphics.h"
#include "BugMediaGraphicsCommon.h"
#include <thread>

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
    ////
    //  该方法用于绘制，有可能在循环中不断调用，把它和init()分开就是为了这个目的
    ////

    // 虚方法，经常发生变化的配置和绘制过程
    startDraw();

    // 绘制好后要交换前台与后台缓冲，以使图像显示。（双缓冲机制）
    pEGL->swapBuffers();

    ////^^^^
    //  以下方式作废，原因是多个对象不能使线程互不干扰
    //    // 虚方法
    //    setShaderSource(); // 该方法初始化了Shader中的代码数据，并没有真正创建Shader。初始化在init()里。
    //    //
    //
    //    //C语言的线程，对线程函数有要求，要求必须是静态的，且返回类型为void*
    //    // 这在对象里使用多线程会出现错乱, 因此决定将线程启动放在java端，
    //    // 一个对象一个线程，互不干扰
    //    pthread_create(&drawThread, NULL, drawBackground, this);

    ////^^^^
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

// C线程执行函数,必须静态，且返回类型为void*,
// 由于方法是静态的，所有对象共享一个，那么
// 当多个对象都启用线程后调用的都是这一个方法，势必会产生竞态,
// 这样就不能创建多个对象使用了。决定将线程启动放在Java端。该方法没有继续编写下去的必要。
void *BugMediaGraphics::drawBackground(void *pVoid) {

    BugMediaGraphics *graphics = (BugMediaGraphics *) pVoid;

    graphics->pEGL->init();
    graphics->pEGL->makeCurrent();
    graphics->pGLES->init();
    graphics->pGLES->activeProgram();
    // 虚方法，不频繁变化的配置
    graphics->prepareDraw();

    // 虚方法，经常发生变化的配置和绘制过程
    graphics->startDraw();

    // 绘制好后要交换前台与后台缓冲，以使图像显示。（双缓冲机制）
    graphics->pEGL->swapBuffers();

//    bool running = true;
//    while (running) {
//
//        switch (graphics->runState) {
//            case RUNNING:
//
//                break;
//            case PAUSE:
//
//                break;
//            case STOP:
//
//                break;
//            default:
//                running= false;
//                break;
//
//        }
//
//    }

#ifdef DEBUGAPP
    LOGD("绘制结束");
#endif
    return 0;
}

////
// 使用C++11的thread时，EGL不能在线程函数里初始化，作废
void BugMediaGraphics::drawingThreadFun(BugMediaGraphics *graphics) {
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
}

void BugMediaGraphics::resize(GLint x, GLint y, GLsizei width, GLsizei height) {
    pGLES->resize(x, y, width, height);
}

// 拆出来这部分为了在java端控制流程，因为线程的问题，这些东西都要在启动的新线程中初始化
void BugMediaGraphics::init() {
    setShaderSource(); // 该方法初始化了Shader中的代码数据，并没有真正创建Shader。初始化在init()里。
    pEGL->init();
    pEGL->makeCurrent();
    pGLES->init();
    pGLES->activeProgram();
    // 虚方法，不频繁变化的配置
    prepareDraw();
}









