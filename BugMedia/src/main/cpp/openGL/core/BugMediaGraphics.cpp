//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphics.h"
#include <thread>

using namespace std;

BugMediaGraphics::BugMediaGraphics() {
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
    setShader();
    pGLES->activeProgram();

    // 线程中不断绘制直至结束
    //
    //std::thread drawingThread(&BugMediaGraphics::drawingFunction);// 调用类成员函数作为线程函数，要取函数地址
    // 匿名函数方式,[]内为捕获列表，参数可以放在这里,在此范围内的函数就可以使用，调用不需加this->...
    std::thread drawBackground([this] {
        makeCurrent();
        //
        onDraw();
        //
        pEGL->swapBuffers();
    });

}

void BugMediaGraphics::release() {
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

void BugMediaGraphics::makeCurrent() {
    if (pEGL != NULL) {
        pEGL->makeCurrent();
    }

}

void BugMediaGraphics::setViewPort(GLint x, GLint y, GLsizei width, GLsizei height) {
    if (pGLES == NULL) {
        return;
    }
    pGLES->viewport(x, y, width, height);
}

//void
//BugMediaGraphics::setShaderSource(const GLchar **const vertexShadersource, const GLchar **const fragmentShadersource) {
//    if (pGLES == NULL) {
//        return;
//    }
//    pGLES->setShaderSource(vertexShadersource, fragmentShadersource);
//}







