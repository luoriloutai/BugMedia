//
// Created by Gshine on 2021/1/26.
// 着色器
//

#ifndef SLOWER_BUGMEDIAGRAPHICSSHADER_H
#define SLOWER_BUGMEDIAGRAPHICSSHADER_H

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

class BugMediaGraphicsShader {
public:

    void release();

    BugMediaGraphicsShader(GLenum type, const GLchar **const source);

    ~BugMediaGraphicsShader();

    GLuint getShader();

private:
    GLuint shader = 0;
    GLboolean isRelease = GL_FALSE;
};


#endif //SLOWER_BUGMEDIAGRAPHICSSHADER_H
