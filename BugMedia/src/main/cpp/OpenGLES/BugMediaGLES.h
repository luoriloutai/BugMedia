//
// Created by Gshine on 2021/3/3.
//

#ifndef SLOWER_BUGMEDIAGLES_H
#define SLOWER_BUGMEDIAGLES_H

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

#include <pthread.h>
#include "../EGL/BugMediaEGL.h"

class BugMediaGLES {

    GLuint glProgram{};
    pthread_t renderThread{};



    static void *renderRoutine(void *pVoid);

    void execRender();

    virtual const GLchar *getVertexShaderSource() = 0;

    virtual const GLchar *getFragmentShaderSource() = 0;

    static GLuint createShader(GLenum type, const GLchar *source);

    static GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);

    // 在线程中的渲染，不需要再开启新线程
    virtual void onRender()=0;

    GLboolean init();

protected:
    BugMediaEGL *pEGL{};
    GLsizei viewWidth{};
    GLsizei viewHeight{};
    GLint x{};
    GLint y{};

public:
    BugMediaGLES(ANativeWindow* nativeWindow);

    virtual ~BugMediaGLES();

    void render();

    void useProgram() const;

    GLuint getProgram() const;

    void resizeView(GLint x, GLint y, GLsizei width, GLsizei height);

};


#endif //SLOWER_BUGMEDIAGLES_H
