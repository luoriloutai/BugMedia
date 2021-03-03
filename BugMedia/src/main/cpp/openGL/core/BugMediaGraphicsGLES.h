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
        GLuint handler;
        GLenum type;
        const GLchar *source;
        GLboolean isRelease;
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
        GLuint handler;
        GLboolean isRelease;

        GLboolean checkGLError(const char *op);
    };

    BugMediaGraphicsGLES();

    void release();

    void setShaderSource(const GLchar *vertexShadersource, const GLchar *fragmentShadersource);

    void activeProgram();

    GLuint getProgram();


    ~BugMediaGraphicsGLES();

    void init();


private:
    Shader *pVertexShader;
    Shader *pFragmentShader;
    Program *pProgram;

    GLboolean checkGLError(const char *op);

    GLboolean isRelease;


};


#endif //SLOWER_BUGMEDIAGRAPHICSGLES_H
