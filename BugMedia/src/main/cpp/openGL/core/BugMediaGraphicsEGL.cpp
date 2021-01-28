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

}

EGLBoolean BugMediaGraphicsEGL::init(EGLContext sharedContext) {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay error:%d\n", eglGetError());
        return EGL_FALSE;

    }
LOGD("display 获取完成");
    EGLBoolean ok = eglInitialize(display, NULL, NULL);
    if (!ok) {
        LOGE("eglInitialize error:%d\n", eglGetError());
        return EGL_FALSE;
    }
    LOGD("初始化display完成");
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

    LOGD("配置选择完成");

    if (config == NULL) {
        LOGE("选择配置失败：%d\n", eglGetError());
        return EGL_FALSE;
    }
    LOGD("配置获取完成");
    EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

    if (!(context = eglCreateContext(display, config, NULL == sharedContext ? EGL_NO_CONTEXT : sharedContext,
                                     contextAttribs))) {
        LOGE("eglCreateContext() returned error %d", eglGetError());
        LOGD("context获取失败");
        return EGL_FALSE;
    }
    LOGD("context 创建完成");
    return EGL_TRUE;
}

void BugMediaGraphicsEGL::setPBufferSurface(EGLint width, EGLint height) {
    this->width = width;
    this->height = height;
    surfaceType = PBUFFER_SURFACE;
}

// 只有windowSurface可以显示
void BugMediaGraphicsEGL::setWindowSurface(JNIEnv *env, jobject jSurface) {

    surfaceType = WINDOW_SURFACE;
    // 这里只创建window，供创建绘制Surface使用。Surface要在绘制线程里创建，
    // context当中包含了绘制所需的数据，所谓的绑定到线程不过就是让线程知道从哪取数据，从context中取，
    // 在Surface里绘制
    window = ANativeWindow_fromSurface(env, jSurface);

}

void BugMediaGraphicsEGL::release() {
    if (window != NULL) {
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

void BugMediaGraphicsEGL::makeCurrent() {

//问题所在：
// 所有环境相关的东西都应该在同一个线程创建
    if (!init(NULL)){
        LOGE("初始化环境失败");
    }

    //windowSurface
    if (surfaceType == WINDOW_SURFACE) {
        EGLint format;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
            LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
        }
        ANativeWindow_setBuffersGeometry(window, 0, 0, format);
        if (!(windowSurface = eglCreateWindowSurface(display, config, window, 0))) {
            LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
        }

        eglMakeCurrent(display, windowSurface, windowSurface, context);

    } else if (surfaceType == PBUFFER_SURFACE) {
        //PBufferSurface

        EGLint PbufferAttributes[] = {
                EGL_WIDTH, width,
                EGL_HEIGHT, height,
                EGL_NONE};
        if (!(PBufferSurface = eglCreatePbufferSurface(display, config, PbufferAttributes))) {
            LOGE("eglCreatePbufferSurface() returned error %d", eglGetError());
        }
        eglMakeCurrent(display, PBufferSurface, PBufferSurface, context);
    }

    LOGD("makeCurrent 结束");
}

EGLBoolean BugMediaGraphicsEGL::swapBuffers() {
    // 只有windowSurface能交换显示缓冲
    return eglSwapBuffers(display, windowSurface);
}
