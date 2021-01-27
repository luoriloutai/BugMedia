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
    class Shader {
    public:
        Shader(GLenum type, const GLchar *const *source);

        ~Shader();

        void release();

        GLuint instance();

    private:
        GLuint handler = 0;

        GLboolean isRelease = GL_FALSE;
    };

    class Program {
    public:
        Program(Shader *vertexShader, Shader *fragmentShader);

        ~Program();

        void active();
        GLuint instance();

        void release();

    private:
        GLuint handler = 0;
        GLboolean isRelease = GL_FALSE;

        GLboolean checkGLError(const char *op);
    };

    BugMediaGraphicsGLES();

    void release();

    void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);

    void setShaderSource(const GLchar **const vertexShadersource, const GLchar **const fragmentShadersource);

    void activeProgram();

    ~BugMediaGraphicsGLES();


private:

    GLboolean checkGLError(const char *op);

    GLboolean isRelease = GL_FALSE;
    Shader *pVertexShader = NULL;
    Shader *pFragmentShader = NULL;
    Program *pProgram = NULL;

};


#endif //SLOWER_BUGMEDIAGRAPHICSGLES_H
