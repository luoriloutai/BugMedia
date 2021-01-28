//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphics.h"
#include <thread>
#include "BugMediaGraphicsCommon.h"

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


    // 线程中不断绘制直至结束
    //
    //std::thread drawingThread(&BugMediaGraphics::drawingFunction);// 调用类成员函数作为线程函数，要取函数地址
    // 匿名函数方式,[]内为捕获列表，参数可以放在这里,在此范围内的函数就可以使用，调用不需加this->...
    std::thread drawBackground([this] {
        // 虚方法
        setShaderSource(); // 该方法初始化了Shader中的代码数据，并没有真正创建Shader。初始化在init()里。
        //

        // 环境初始化
        init();

        // 虚方法
        prepareDraw();
        //

        //pGLES->activeProgram();

        // 这里可能需要循环不断绘制
        // 虚方法
        startDraw();
        //
        pEGL->swapBuffers();
        LOGD("绘制结束");
        // 循环结束
    });

}

void BugMediaGraphics::release() {
    LOGD("开始释放资源");
    if (!isRelease) {
        delete pGLES;
        pGLES = NULL;
        delete pEGL;
        pEGL = NULL;

        isRelease = GL_TRUE;
    }

}

BugMediaGraphics::~BugMediaGraphics() {
    if (!isRelease) {
        release();
    }
}

void BugMediaGraphics::init() {
    pEGL->init();
    pGLES->init();
}

void BugMediaGraphics::viewPort(GLint x, GLint y, GLsizei width, GLsizei height) {
    glViewport(x, y, width, height);
}









