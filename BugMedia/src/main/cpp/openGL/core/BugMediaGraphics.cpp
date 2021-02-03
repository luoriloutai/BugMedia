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

void BugMediaGraphics::resizeView(GLint x, GLint y, GLsizei width, GLsizei height) {
    pEGL->resizeView(x,y,width,height);
}

void BugMediaGraphics::swapBuffers() {
    pEGL->swapBuffers();
}

void BugMediaGraphics::setShaderSources(const GLchar *vertexShadersource, const GLchar *fragmentShadersource) {
    pGLES->setShaderSource(vertexShadersource, fragmentShadersource);
}

GLuint BugMediaGraphics::setVertexAttribArray(const GLchar *name, GLint attribDim, GLenum eleType, GLboolean normalized,
                                              GLsizei stride, const void *array) {
    GLuint attribPosition = glGetAttribLocation(pGLES->getProgram(), name);
    glVertexAttribPointer(attribPosition, attribDim, eleType, normalized, stride, array);

    // 启用顶点属性变量，默认是禁止的
    glEnableVertexAttribArray(attribPosition);
    return attribPosition;

}

GLuint BugMediaGraphics::getProgram() {
    return pGLES->getProgram();
}

GLuint BugMediaGraphics::set2DTexture0(const GLchar *uniformTexSamplerName, uint8_t *data, GLint width, GLint height) {
    GLuint textureId = -1;
    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // 给纹理对象设置数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    GLuint samplerId = glGetUniformLocation(pGLES->getProgram(), uniformTexSamplerName);
    // 将激活的纹理单元传送到着色器中,相当于给着色器中的sampler赋值。
    // 第二个参数表示激活的是哪个纹理单元，这取决于前面glActiveTexture()参数，
    // GL_TEXTURE[n]后面的数字就是第二个参数的值。
    glUniform1i(samplerId, 0);


    return textureId;
}

void BugMediaGraphics::unbind2DTexture0(GLuint *texLocation) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, texLocation);
}

void BugMediaGraphics::useProgram() {
    pGLES->activeProgram();
}

EGLint BugMediaGraphics::getViewWidth() {
    return pEGL->getViewWidth();
}

EGLint BugMediaGraphics::getViewHeight() {
    return pEGL->getViewHeight();
}









