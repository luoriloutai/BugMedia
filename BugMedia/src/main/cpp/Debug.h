//
// Created by Gshine on 2021/1/29.
//

#ifndef SLOWER_DEBUG_H
#define SLOWER_DEBUG_H

extern "C" {
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/native_window_jni.h>
#include <pthread.h>
#include "openGL/core/BugMediaGraphicsCommon.h"

};

ANativeWindow *getWindow(JNIEnv *env, jobject jSurface) {
    return ANativeWindow_fromSurface(env, jSurface);
}

EGLDisplay display = NULL;
EGLConfig config = NULL;
EGLContext context = NULL;
EGLSurface windowSurface = NULL;
EGLSurface PBufferSurface = NULL;
ANativeWindow *_window;

void initEGL() {
    LOGD("EGL开始初始化");
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay error:%d\n", eglGetError());


    }
    if (display==NULL){
        LOGE("显示器没取到");
    }

    EGLBoolean ok = eglInitialize(display, NULL, NULL);
    if (!ok) {
        LOGE("eglInitialize error:%d\n", eglGetError());

    }


    // android下的配置
    const EGLint configAttribs[] = {EGL_RED_SIZE, 8,
                                    EGL_GREEN_SIZE, 8,
                                    EGL_BLUE_SIZE, 8,
                                    EGL_ALPHA_SIZE, 8,
                                    EGL_BUFFER_SIZE, 32,
                                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                    EGL_NONE}; // 必须以EGL_NONE结尾

    EGLint numConfigs = 0;

    // 选择符合配置选项的config
    // config_size:选择符合条件的多少个config输出到传入的参数Config中
    // numConfigs:符合条件的配置有多少个，输出到numConfigs
    // 第三个参数可以不传，这时numConfigs的值就是找到的符合条件的eglconfig的个数，
    // 然后，再将numConfigs传到第四个参数，同时传一个接收eglconfig的数组到第三个参数，可以获取所有符合条件的eglconfig
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig() returned error %d", eglGetError());

    }
LOGD("config conout:%d",numConfigs);

    if (config == NULL) {
        LOGE("选择配置失败：%d\n", eglGetError());

    }

    EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

    if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT,
                                     contextAttribs))) {
        LOGE("eglCreateContext() returned error %d", eglGetError());

    }


    LOGD("EGL初始化完成");
}

void createSurface(){
    EGLint format;
    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE("eglGetConfigAttrib() returned error %d", eglGetError());

    }

    ANativeWindow_setBuffersGeometry(_window, 0, 0, format);
    if (!(windowSurface = eglCreateWindowSurface(display, config, _window, 0))) {
        LOGE("eglCreateWindowSurface() returned error %d", eglGetError());

    }
}

// 重要：必须为静态的才能使外部变量有效
 static void * threadFunc(void *w) {
    initEGL();
    LOGD("dispaly 为空： %s",display==NULL?"是":"否");
    LOGD("windowSurface 为空： %s",windowSurface==NULL?"是":"否");
    LOGD("context 为空： %s",context==NULL?"是":"否");
    //
    // 这特么的，必须要在线程函数里面创建Surface，然后绑定线程才行，不然着色器等都创建失败
    createSurface();
    LOGD("windowSurface 为空： %s",windowSurface==NULL?"是":"否");
    //
    if (!eglMakeCurrent(display, windowSurface, windowSurface, context)){
        LOGE("makeCurrent失败啦 %d",eglGetError());
    }
    GLuint shader = 0;
    if ((shader = glCreateShader(GL_VERTEX_SHADER)) == 0) {
        LOGE("shader创建失败啦：%d", glGetError());
    } else {
        LOGD("shader ：%d", shader);
        glDeleteShader(shader);
    }

    GLuint pHandler = 0;
    if ((pHandler = glCreateProgram()) == 0) {
        LOGE("着色程序创建失败啦：%d", glGetError());
    } else {
        LOGD("handler :%d", pHandler);
        glDeleteProgram(pHandler);
    }



    if (_window != NULL) {
        ANativeWindow_release(_window);
    }
    if (windowSurface != NULL) {
        eglDestroySurface(display, windowSurface);
        windowSurface = NULL;
    }
    if (PBufferSurface != NULL) {
        eglDestroySurface(display, PBufferSurface);
        PBufferSurface = NULL;
    }
    if (context != NULL) {
        eglDestroyContext(display, context);
        context = NULL;
    }
    return NULL;
}

void testCreate(ANativeWindow *aNativeWindow) {

    _window = aNativeWindow;
    pthread_t threadHandler;
    pthread_create(&threadHandler, NULL, threadFunc, aNativeWindow);


}

#endif //SLOWER_DEBUG_H
