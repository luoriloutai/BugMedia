//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphicsEGL.h"
#include "BugMediaGraphicsCommon.h"
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#include <android/native_window.h>
#include <android/native_window_jni.h>

BugMediaGraphicsEGL::BugMediaGraphicsEGL() {
    if (!init()) {
        throw "EGL初始化失败";
    }
}

EGLBoolean BugMediaGraphicsEGL::init() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay error:%d\n", eglGetError());
        return EGL_FALSE;

    }

    EGLBoolean ok = eglInitialize(display, NULL, NULL);
    if (!ok) {
        LOGE("eglInitialize error:%d\n", eglGetError());
        return EGL_FALSE;
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
    // config_size:选择符合条件的多少个config输出到传入的参数gConfig中
    // numConfigs:符合条件的配置有多少个，输出到numConfigs
    // 第三个参数可以不传，这时numConfigs的值就是找到的符合条件的eglconfig的个数，
    // 然后，再将numConfigs传到第四个参数，同时传一个接收eglconfig的数组到第三个参数，可以获取所有符合条件的eglconfig
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig() returned error %d", eglGetError());
        return EGL_FALSE;
    }
    if (config == NULL) {
        LOGE("选择配置失败：%d\n", eglGetError());
        return EGL_FALSE;
    }
    EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

    if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs))) {
        LOGE("eglCreateContext() returned error %d", eglGetError());
        return EGL_FALSE;
    }

    return EGL_TRUE;
}

EGLBoolean BugMediaGraphicsEGL::setPBufferSurface(EGLint width, EGLint height) {

    EGLint PbufferAttributes[] = {
            EGL_WIDTH, width,
            EGL_HEIGHT, height,
            EGL_NONE};
    if (!(PBufferSurface = eglCreatePbufferSurface(display, config, PbufferAttributes))) {
        LOGE("eglCreatePbufferSurface() returned error %d", eglGetError());
        return EGL_FALSE;
    }

    // Surface和Context绑定到当前线程
    //eglMakeCurrent(display, PBufferSurface, PBufferSurface, context);
    return EGL_TRUE;
}

// 只有windowSurface可以显示
EGLBoolean BugMediaGraphicsEGL::setWindowSurface(JNIEnv *env, jobject jSurface) {

    EGLint format;
    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
        return EGL_FALSE;
    }
    window = ANativeWindow_fromSurface(env, jSurface);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
    if (!(windowSurface = eglCreateWindowSurface(display, config, window, 0))) {
        LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
        return EGL_FALSE;
    }
    //eglMakeCurrent(display, windowSurface, windowSurface, context);
    return EGL_TRUE;
}

void BugMediaGraphicsEGL::release() {
    if (window!=NULL){
        ANativeWindow_release(window);
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
    isRelease = EGL_TRUE;

}

BugMediaGraphicsEGL::~BugMediaGraphicsEGL() {
    if (!isRelease) {
        release();
    }

}

EGLDisplay BugMediaGraphicsEGL::getDisplay() {
    return display;
}

EGLSurface BugMediaGraphicsEGL::getWindowSurface() {
    return windowSurface;
}

EGLSurface BugMediaGraphicsEGL::getPBufferSurface() {
    return PBufferSurface;
}

void BugMediaGraphicsEGL::makeCurrent() {
    if (windowSurface != NULL) {
        eglMakeCurrent(display, windowSurface, windowSurface, context);
    }
    // PBufferSurface不能swap,只能复制或绑定到纹理
    if (PBufferSurface != NULL) {
        eglMakeCurrent(display, PBufferSurface, PBufferSurface, context);
    }
}