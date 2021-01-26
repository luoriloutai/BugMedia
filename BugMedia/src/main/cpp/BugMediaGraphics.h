//
// Created by Gshine on 2021/1/25.
//

#ifndef SLOWER_BUGMEDIAGRAPHICS_H
#define SLOWER_BUGMEDIAGRAPHICS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <EGL/egl.h>
#include <EGL/eglext.h>

// OpenGL ES 2.0 渲染系统
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>



#ifdef __cplusplus
}
#endif

#include <jni.h>
#include <android/log.h>

#define TAG "BugMediaOutput"
#define  LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

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

class BugMediaGraphicsProgram {
public:

    BugMediaGraphicsProgram(GLuint vertexShader, GLuint fragmentShader);

    GLuint getProgram();

    //void attachShader(GLuint shader);

    void release();

    ~BugMediaGraphicsProgram();

private:
    GLuint program = 0;

    GLboolean checkGLError(const char *op);

    GLboolean isRelease = GL_FALSE;

};

class BugMediaGraphicsEGL {
public:
    BugMediaGraphicsEGL();

    EGLBoolean setWindowSurface(JNIEnv *env, jobject jSurface);

    EGLBoolean setPBufferSurface(EGLint width, EGLint height);
    EGLDisplay getDisplay();
    EGLSurface getWindowSurface();
    EGLSurface getPBufferSurface();

    void makeCurrent();

    void release();

    ~BugMediaGraphicsEGL();

private:
    EGLDisplay display = NULL;
    EGLConfig config = NULL;
    EGLContext context = NULL;
    EGLSurface windowSurface = NULL;
    EGLSurface PBufferSurface = NULL;
    ANativeWindow *window = NULL;
    EGLBoolean isRelease = EGL_FALSE;

    EGLBoolean init();
};

class BugMediaGraphics {
public:
    BugMediaGraphics();

    void setVertexShader(const char **const source);

    void setFragmentShader(const char **const source);

    void setWindowSurface(JNIEnv *env, jobject jSurface);

    void setPBufferSurface(EGLint width, EGLint height);

    void prepareProgram();


    virtual void onProgramPrepared(){};

    virtual void onDraw(){};

    void release();

    ~BugMediaGraphics();

    void draw();

protected:
    void makeCurrent();
    void setViewPort(GLint x,GLint y,GLsizei width,GLsizei height);
    BugMediaGraphicsEGL *pEGL = NULL;
    BugMediaGraphicsProgram *pGLESProgram = NULL;
    BugMediaGraphicsShader *pVertextShader = NULL;
    BugMediaGraphicsShader *pFragmentShader = NULL;
    GLboolean isRelease = GL_FALSE;
};





#endif //SLOWER_BUGMEDIAGRAPHICS_H
