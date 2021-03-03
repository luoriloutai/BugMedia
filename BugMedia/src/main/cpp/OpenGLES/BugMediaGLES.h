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
//    const GLchar *vertexShaderSource = "attribute vec4 position; \n"
//                                       "attribute vec2 texcoord; \n"
//                                       "varying vec2 v_texcoord; \n"
//                                       "void main(void) \n"
//                                       "{ \n"
//                                       " gl_Position =  position; \n"
//                                       " v_texcoord = texcoord; \n"
//                                       "} \n";
//    const GLchar *fragmentShaderSource = "precision highp float; \n"
//                                         "varying highp vec2 v_texcoord; \n"
//                                         "uniform sampler2D texSampler; \n"
//                                         "void main() { \n"
//                                         " gl_FragColor = texture2D(texSampler, v_texcoord); \n"
//                                         "} \n";


    GLuint glProgram{};
    pthread_t renderThread{};
    BugMediaEGL *pEGL{};

    static void *renderRoutine(void *pVoid);

    void execRender();

    virtual GLchar *getVertexShaderSource() = 0;

    virtual GLchar *getFragmentShaderSource() = 0;

    static GLuint createShader(GLenum type, const GLchar *source);

    static GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);

    // 在线程中的渲染，不需要再开启新线程
    virtual void onRender()=0;

public:
    BugMediaGLES(ANativeWindow* nativeWindow);

    virtual ~BugMediaGLES();

    GLboolean init();

    void render();

    void useProgram() const;

    GLuint getProgram() const;

};


#endif //SLOWER_BUGMEDIAGLES_H
