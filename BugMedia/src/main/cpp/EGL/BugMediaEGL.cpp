//
// Created by Gshine on 2021/3/3.
//

#include "BugMediaEGL.h"
#include "../BugMediaPublicDefine.h"

EGLSurface BugMediaEGL::createWindowSurface(ANativeWindow *nativeWindow) {

    if (nativeWindow == nullptr) {
        LOGE("argument nativeWindow is null");
        return nullptr;
    }
    EGLint format{};
    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE("eglGetConfigAttrib() error %d", eglGetError());
        release();
        return nullptr;
    }
    ANativeWindow_setBuffersGeometry(nativeWindow, 0, 0, format);

    EGLSurface surface{};
    if (!(surface = eglCreateWindowSurface(display, config, nativeWindow, nullptr))) {
        LOGE("eglCreateWindowSurface() error %d", eglGetError());
        release();
        return nullptr;
    }
    return surface;
}

void BugMediaEGL::release() {
    if (isRelease){
        return;
    }
    if (windowSurface != EGL_NO_SURFACE) {
        eglDestroySurface(display, windowSurface);
        windowSurface = EGL_NO_SURFACE;
    }
    if (PBufferSurface != EGL_NO_SURFACE) {
        eglDestroySurface(display, PBufferSurface);
        PBufferSurface = EGL_NO_SURFACE;
    }
    if (window != nullptr) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    if (display != EGL_NO_DISPLAY && context != EGL_NO_CONTEXT) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(display, context);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    isRelease = true;
}

EGLSurface BugMediaEGL::createPBufferSurface(int width, int height) {
    EGLSurface surface{};
    EGLint PbufferAttributes[] = {EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE};
    if (!(surface = eglCreatePbufferSurface(display, config, PbufferAttributes))) {
        LOGE("eglCreatePbufferSurface() error %d", eglGetError());
    }
    return surface;
}

EGLBoolean BugMediaEGL::makeCurrent(EGLSurface eglSurface) {
    return eglMakeCurrent(display, eglSurface, eglSurface, context);
}

EGLBoolean BugMediaEGL::swapBuffers(EGLSurface eglSurface) {
    return eglSwapBuffers(display, eglSurface);
}

void BugMediaEGL::unmakeCurrent() {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void BugMediaEGL::destroySurface(EGLSurface eglSurface) {
    eglDestroySurface(display, eglSurface);
    eglSurface = EGL_NO_SURFACE;
}

EGLBoolean BugMediaEGL::init(EGLContext sharedContext) {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay error:%d", eglGetError());
        return EGL_FALSE;
    }

    if (!eglInitialize(display, nullptr, nullptr)) {
        LOGE("eglInitialize() error:%d", eglGetError());
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
        LOGE("eglChooseConfig() error:%d", eglGetError());
        return EGL_FALSE;
    }

    if (config == nullptr) {
        LOGE("config is null");
        return EGL_FALSE;
    }

    EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

    if (!(context = eglCreateContext(display, config, nullptr == sharedContext ? EGL_NO_CONTEXT : sharedContext,
                                     contextAttribs))) {
        LOGE("eglCreateContext() error:%d", eglGetError());
        return EGL_FALSE;
    }

    return EGL_TRUE;
}

EGLBoolean BugMediaEGL::init() {
    return init(nullptr);
}

BugMediaEGL::BugMediaEGL(ANativeWindow *nativeWindow) {
    window = nativeWindow;
}

BugMediaEGL::~BugMediaEGL() {
    release();
}

EGLBoolean BugMediaEGL::createDefaultWindowSurface(ANativeWindow *nativeWindow) {
    windowSurface = createWindowSurface(nativeWindow);
    if (windowSurface == nullptr) {
        return EGL_FALSE;
    }
    return EGL_TRUE;
}

EGLBoolean BugMediaEGL::createDefaultPBufferSurface(int width, int height) {
    PBufferSurface = createPBufferSurface(width, height);
    if (PBufferSurface == nullptr) {
        return EGL_FALSE;
    }
    return EGL_TRUE;
}

EGLBoolean BugMediaEGL::makeDefaultWindowSurfaceCurrent() {
    return makeCurrent(windowSurface);
}

EGLBoolean BugMediaEGL::makeDefaultPBufferSurfaceCurrent() {
    return makeCurrent(PBufferSurface);
}

EGLBoolean BugMediaEGL::swapDefaultBuffers() {
    return swapBuffers(windowSurface);
}


