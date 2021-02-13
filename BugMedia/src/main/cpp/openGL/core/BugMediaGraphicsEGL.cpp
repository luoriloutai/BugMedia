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
    display = EGL_NO_DISPLAY;
    config = nullptr;
    context = EGL_NO_CONTEXT;
    windowSurface = EGL_NO_SURFACE;
    PBufferSurface = EGL_NO_SURFACE;
    window = nullptr;
    isRelease = EGL_FALSE;

    surfaceType = NO_SURFACE;
    viewWidth = 0;
    viewHeight = 0;
}

EGLBoolean BugMediaGraphicsEGL::init(EGLContext sharedContext) {
#ifdef DEBUGAPP
    LOGD("EGL开始初始化");
#endif
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
#ifdef DEBUGAPP
        LOGE("eglGetDisplay error:%d\n", eglGetError());
#endif
        return EGL_FALSE;

    }

    EGLBoolean ok = eglInitialize(display, nullptr, nullptr);
    if (!ok) {
#ifdef DEBUGAPP
        LOGE("eglInitialize error:%d\n", eglGetError());
#endif
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
    // config_size:选择符合条件的多少个config输出到传入的参数Config中
    // numConfigs:符合条件的配置有多少个，输出到numConfigs
    // 第三个参数可以不传，这时numConfigs的值就是找到的符合条件的eglconfig的个数，
    // 然后，再将numConfigs传到第四个参数，同时传一个接收eglconfig的数组到第三个参数，可以获取所有符合条件的eglconfig
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
#ifdef DEBUGAPP
        LOGE("eglChooseConfig() returned error %d", eglGetError());
#endif
        return EGL_FALSE;
    }


    if (config == nullptr) {
#ifdef DEBUGAPP
        LOGE("选择配置失败：%d\n", eglGetError());
#endif
        return EGL_FALSE;
    }

    EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

    if (!(context = eglCreateContext(display, config, nullptr == sharedContext ? EGL_NO_CONTEXT : sharedContext,
                                     contextAttribs))) {
#ifdef DEBUGAPP
        LOGE("eglCreateContext() returned error %d", eglGetError());
#endif
        return EGL_FALSE;
    }
#ifdef DEBUGAPP
    LOGD("EGL初始化完成");
#endif
    return EGL_TRUE;
}

void BugMediaGraphicsEGL::setPBufferSurface(EGLint width, EGLint height) {
    this->viewWidth = width;
    this->viewHeight = height;
    surfaceType = PBUFFER_SURFACE;
}

// 只有windowSurface可以显示
void BugMediaGraphicsEGL::setWindowSurface(JNIEnv *env, jobject jSurface) {
    surfaceType = WINDOW_SURFACE;
    // 这里只创建window，供创建绘制Surface使用。Surface要在绘制线程里创建，
    // context当中包含了绘制所需的数据，所谓的绑定到线程不过就是让线程知道从哪取数据，从context中取，
    // 在Surface里绘制,EGLSurface必须要在线程里创建，创建之后可用于绑定至线程
    window = ANativeWindow_fromSurface(env, jSurface);
}

void BugMediaGraphicsEGL::release() {
#ifdef DEBUGAPP
    LOGD("EGL开始释放资源");
#endif
    if (!isRelease) {

        if (windowSurface != EGL_NO_SURFACE) {
            eglDestroySurface(display, windowSurface);
            windowSurface = EGL_NO_SURFACE;
        }
        if (PBufferSurface != EGL_NO_SURFACE) {
            eglDestroySurface(display, PBufferSurface);
            PBufferSurface = EGL_NO_SURFACE;
        }
        eglDestroyContext(display, context);
        if (window != nullptr) {
            ANativeWindow_release(window);
            window = nullptr;
        }
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        display = EGL_NO_DISPLAY;
        context = EGL_NO_CONTEXT;
        isRelease = EGL_TRUE;
    }
#ifdef DEBUGAPP
    LOGD("EGL资源释放完毕");
#endif

}

BugMediaGraphicsEGL::~BugMediaGraphicsEGL() {
    if (!isRelease) {
        release();
    }

}


EGLBoolean BugMediaGraphicsEGL::swapBuffers() {
    // 只有windowSurface能交换显示缓冲
    return eglSwapBuffers(display, windowSurface);
}

void BugMediaGraphicsEGL::init() {

// 所有环境相关的东西都应该在同一个线程创建
    if (!init(nullptr)) {
        LOGE("初始化环境失败");
    }

}

EGLBoolean BugMediaGraphicsEGL::makeCurrent() {
#ifdef DEBUGAPP
    LOGD("绑定线程开始");
#endif
    //windowSurface
    if (surfaceType == WINDOW_SURFACE) {
        EGLint format;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
            LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
            return EGL_FALSE;
        }
#ifdef DEBUGAPP
        LOGD("makeCurrent()之前的Surface是否为空: %s", windowSurface == nullptr ? "是" : "否");
#endif
        ANativeWindow_setBuffersGeometry(window, 0, 0, format);
        if (!(windowSurface = eglCreateWindowSurface(display, config, window, 0))) {
            LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
            return EGL_FALSE;
        }
#ifdef DEBUGAPP
        LOGD("执行Surface创建，是否为空:%s", windowSurface == nullptr ? "是" : "否");
        LOGD("display是否为空 %s", display == nullptr ? "是" : "否");
        LOGD("context是否为空 %s", context == nullptr ? "是" : "否");
#endif
        if (!eglMakeCurrent(display, windowSurface, windowSurface, context)) {
            LOGE("eglMakeCurrent() error:%s\n", eglGetError());
            return EGL_FALSE;
        }

    } else if (surfaceType == PBUFFER_SURFACE) {
        //PBufferSurface

        EGLint PbufferAttributes[] = {
                EGL_WIDTH, viewWidth,
                EGL_HEIGHT, viewHeight,
                EGL_NONE};
        if (!(PBufferSurface = eglCreatePbufferSurface(display, config, PbufferAttributes))) {
            LOGE("eglCreatePbufferSurface() returned error %d", eglGetError());
            return EGL_FALSE;
        }
        if (!eglMakeCurrent(display, PBufferSurface, PBufferSurface, context)) {
            LOGE("eglMakeCurrent error:%s\n", eglGetError());
            return EGL_FALSE;
        }
    }
#ifdef DEBUGAPPAPP
    LOGD("绑定线程完成");
#endif
    return EGL_TRUE;
}

EGLint BugMediaGraphicsEGL::getViewWidth() const {
    return viewWidth;
}

EGLint BugMediaGraphicsEGL::getViewHeight() const {
    return viewHeight;
}

void BugMediaGraphicsEGL::resizeView(EGLint x, EGLint y, EGLint width, EGLint height) {
    viewWidth = width;
    viewHeight = height;
}

