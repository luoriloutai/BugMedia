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

    void render();

    void resizeView(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
    void swapBuffers();

    void setShaderSources(const GLchar *vertexShadersource, const GLchar *fragmentShadersource);

    //
    // 不使用缓冲区为顶点属性赋值
    //
    // name：顶点着色器中属性的名字
    // attribDim:属性维度
    // eleType:数组元素类型
    // normalized：是否标准化，对于无符号数映射到0到1，有符号数映射到-1到1
    // stride：步长，跨度，即一个数据占多大，当多种数据都放在一个数组中时用来跳过一组数据
    // array：数组对象
    GLuint setVertexAttribArray(const GLchar *name, GLint attribDim, GLenum eleType, GLboolean normalized,
                                GLsizei stride, const void *array);

    GLuint getProgram();

    void useProgram();

    // 使用2D texture0创建纹理对象并设置数据
    GLuint set2DTexture0(const GLchar *uniformTexSamplerName,uint8_t * data,GLint width,GLint height);
    // 解绑 2D Texture0纹理单元，并删除纹理
    void unbind2DTexture0(GLuint *texLocation);

    EGLint getViewWidth();
    EGLint getViewHeight();

private:
    BugMediaGraphicsEGL *pEGL;
    BugMediaGraphicsGLES *pGLES;

    // 设置Shader代码
    virtual void setShaderSource() = 0;

    // 开始绘制
    virtual void onRender() = 0;

    GLboolean isRelease;

    // C的线程函数，必须为静态
    static void *drawBackground(void *pVoid);

    //void drawingThreadFun(BugMediaGraphics *graphics);

    pthread_t drawThread;


};


#endif //SLOWER_BUGMEDIAGRAPHICS_H
