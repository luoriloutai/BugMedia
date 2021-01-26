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

    BugMediaGraphicsGLES(GLuint vertexShader, GLuint fragmentShader);

    GLuint getProgram();

    //void attachShader(GLuint shader);

    void release();
    void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);

    ~BugMediaGraphicsGLES();

private:
    GLuint program = 0;

    GLboolean checkGLError(const char *op);

    GLboolean isRelease = GL_FALSE;
};


#endif //SLOWER_BUGMEDIAGRAPHICSGLES_H
