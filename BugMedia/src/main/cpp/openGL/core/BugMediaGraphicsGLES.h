//
// Created by Gshine on 2021/1/26.
//

#ifndef SLOWER_BUGMEDIAGRAPHICSGLES_H
#define SLOWER_BUGMEDIAGRAPHICSGLES_H

#ifdef __cplusplus
extern "C"
{
#endif

// OpenGL ES 2.0 渲染系统
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#ifdef __cplusplus
}
#endif


class BugMediaGraphicsGLES {

public:
    // 着色器，供渲染程序使用
    class Shader {
    public:
        Shader(GLenum type, const GLchar *source);

        ~Shader();

        void release();

        GLuint instance();

        void init();

    private:
        GLuint handler = 0;
        GLenum type = NULL;
        const GLchar *source = NULL;
        GLboolean isRelease = GL_FALSE;
    };

    // 渲染程序
    class Program {
    public:
        Program();

        ~Program();

        void active();

        GLuint instance();

        void release();

        void init(BugMediaGraphicsGLES::Shader *vertexShader,
                  BugMediaGraphicsGLES::Shader *fragmentShader);


    private:
        GLuint handler = 0;
        GLboolean isRelease = GL_FALSE;

        GLboolean checkGLError(const char *op);
    };

    // 视野
    struct Viewport {
        // x轴方向的偏移量
        GLint x = 0;
        // y轴方向的偏移量
        GLint y = 0;
        // 宽度。默认300
        GLsizei width = 300;
        // 高度。默认300
        GLsizei height = 300;
    };

    BugMediaGraphicsGLES();

    void release();

    void setShaderSource(const GLchar *vertexShadersource, const GLchar *fragmentShadersource);

    void activeProgram();


    // 设置顶点属性
    // name:顶点着色器中的量的名字
    // vertexDim:表示一个点由几个元素构成
    // eleType:坐标数组元素的数据类型
    // normalized:是否标准化坐标，即把坐标映射到0到1之间。
    // arraySize:数组的字节长度（sizeof）
    // array:顶点数组
    GLuint setVertexAttribArray(const GLchar *name, GLint attribDim, GLenum eleType, GLboolean normalized,
                                GLsizei stride,const void *array);

    GLuint getAttribLocation(const GLchar* name);

    // 使用texture0创建纹理对象并设置数据
    GLuint set2DTexture0(const GLchar *uniformTexSamplerName,uint8_t * data,GLint width,GLint height);

    void unbind2DTexture0(GLuint *texLocation);


    void drawArrays(GLenum mode, GLint first, GLsizei count);

    void drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);

    ~BugMediaGraphicsGLES();

    void init();

    void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);

    void resize(GLint x, GLint y, GLsizei width, GLsizei height);

private:
    Shader *pVertexShader = NULL;
    Shader *pFragmentShader = NULL;
    Program *pProgram = NULL;
    Viewport viewport;

    void setViewport(Viewport v);

    GLboolean checkGLError(const char *op);

    GLboolean isRelease = GL_FALSE;


};


#endif //SLOWER_BUGMEDIAGRAPHICSGLES_H
