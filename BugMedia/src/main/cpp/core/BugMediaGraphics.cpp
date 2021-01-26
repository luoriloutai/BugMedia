//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphics.h"
#include "BugMediaGraphicsCommon.h"

BugMediaGraphics::BugMediaGraphics() {
    pEGL = new BugMediaGraphicsEGL();
}

void BugMediaGraphics::setFragmentShader(const char **const source) {
    // 这种情况不用手动释放指针
    //BugMediaGraphicsShader fragShader(GL_FRAGMENT_SHADER, source);
    //pFragmentShader = &fragShader;

    pFragmentShader = new BugMediaGraphicsShader(GL_FRAGMENT_SHADER, source);

}

void BugMediaGraphics::setVertexShader(const char **const source) {
    // 这种情况不用手动释放指针
//    BugMediaGraphicsShader vertShader(GL_VERTEX_SHADER, source);
//    pVertextShader = &vertShader;

    pVertextShader = new BugMediaGraphicsShader(GL_VERTEX_SHADER, source);
}

void BugMediaGraphics::setWindowSurface(JNIEnv *env, jobject jSurface) {
    pEGL->setWindowSurface(env, jSurface);
}

void BugMediaGraphics::setPBufferSurface(EGLint width, EGLint height) {
    pEGL->setPBufferSurface(width, height);
}


void BugMediaGraphics::prepareProgram() {
    if (pVertextShader == NULL || pFragmentShader == NULL) {
        throw "必须先设置着色器：调用setXXXShader()方法";
    }
    GLuint vertexShader = pVertextShader->getShader();
    GLuint fragmentShader = pFragmentShader->getShader();
    pGLES = new BugMediaGraphicsGLES(vertexShader, fragmentShader);
    GLuint program = pGLES->getProgram();

    // 着色器链接到着色程序后就可以删除了
    pVertextShader->release();
    pFragmentShader->release();
    // 自己实现绘制前的准备工作
    onProgramPrepared();

    // 激活着色程序
    glUseProgram(program);
}


void BugMediaGraphics::draw() {

    onDraw();
    //
    EGLDisplay display = pEGL->getDisplay();
    EGLSurface PBufferSurface = pEGL->getPBufferSurface();
    if (PBufferSurface != NULL) {
        eglSwapBuffers(display, PBufferSurface);
    }
    EGLSurface windowSurface = pEGL->getWindowSurface();
    if (windowSurface != NULL) {
        eglSwapBuffers(display, windowSurface);
    }
}

void BugMediaGraphics::release() {
    if (!isRelease) {


        delete pVertextShader;
        pVertextShader = NULL;
        delete pFragmentShader;
        pFragmentShader = NULL;
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
    pEGL->makeCurrent();
}

void BugMediaGraphics::setViewPort(GLint x, GLint y, GLsizei width, GLsizei height) {
    pGLES->setViewport(x, y, width, height);
}
