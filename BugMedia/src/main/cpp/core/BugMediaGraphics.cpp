//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphics.h"

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
    pGLES->activeProgram();
    onDraw();
    //
    EGLDisplay display = pEGL->getDisplay();
    EGLSurface PBufferSurface = pEGL->getPBufferSurface();
    if (PBufferSurface != NULL) {
        eglSwapBuffers(display, PBufferSurface);
    }
    EGLSurface windowSurface = pEGL->getWindowSurface();
    if (windowSurface != NULL) {
        eglSwapBuffers(display, windowSurface);// 只有windowSurface才可以交换
    }
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
    pGLES->setViewport(x, y, width, height);
}

void
BugMediaGraphics::setShaderSource(const GLchar **const vertexShadersource, const GLchar **const fragmentShadersource) {
    if (pGLES == NULL) {
        return;
    }
    pGLES->setShaderSource(vertexShadersource, fragmentShadersource);
}




