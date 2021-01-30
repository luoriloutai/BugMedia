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


    // 告诉OpenGL该如何解析顶点数据,为着色器属性变量设置值。
    // 该方法将从顶点缓冲区中获取数据，具体是哪个缓冲区取决于之前绑定的缓冲区
    // 第一个参数是顶点着色器中的量的名字
    // 第二个参数表示一个点由几个元素构成
    // 第三个参数是坐标数组元素的数组类型
    // 第四个参数表示是否标准化坐标，即把坐标映射到0到1之间。
    // 第五个参数是步长，或叫跨度，即一个顶点所占用的字节数,即 顶点维度*数组中每个元素的大小
    // 第六个参数表示位置数据在缓冲中起始位置的偏移量(Offset)。但这里貌似可以传入我们的数组
    GLuint setVertexAttribArray(const GLchar *name, GLint vertexDim, GLenum eleType, GLboolean normalized,
                                GLsizei stride, const void *pointer);

    void enable(GLenum cap);

    void blendFunc(GLenum sfactor, GLenum dfactor);

    void clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

    void clear(GLbitfield mask);

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
