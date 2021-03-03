//
// Created by Gshine on 2021/3/3.
//

#ifndef SLOWER_BUGMEDIAEGL_H
#define SLOWER_BUGMEDIAEGL_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

/*
 * 提供默认的WindowSurface和PBufferSurface，
 * 调用方法名带有“Default”的方法直接使用。
 *
 * 此外也可以调用其他创建WindowSurface和PBufferSurface
 * 的方法来创建自己的对象，由自己控制哪个Surface绑定到线程
 * */

class BugMediaEGL {
    EGLDisplay display{};
    EGLConfig config{};
    EGLContext context{};
    ANativeWindow* window{};
    EGLSurface windowSurface{};
    EGLSurface PBufferSurface{};
    EGLBoolean isRelease= false;
public:
    // 使用自己控制的窗口、WindowSurface
    BugMediaEGL();

    // 使用默认窗口、默认WindowSurface
    BugMediaEGL(ANativeWindow* nativeWindow);

    ~BugMediaEGL();

    EGLBoolean init(EGLContext sharedContext);

    EGLBoolean init();

    // 类内部提供一个WindowSurface
    // 创建默认的WindowSurface
    EGLBoolean createDefaultWindowSurface();

    // 类内部提供一个PBufferSurface
    // 创建默认的PBufferSurface
    EGLBoolean createDefaultPBufferSurface(int width,int height);

    // 将默认 WindowSurface 绑定到当前线程
    EGLBoolean makeDefaultWindowSurfaceCurrent();

    // 将默认PBufferSurface绑定到当前线程
    EGLBoolean makeDefaultPBufferSurfaceCurrent();

    EGLBoolean swapDefaultBuffers();

    EGLSurface createWindowSurface(ANativeWindow* nativeWindow);

    EGLSurface createPBufferSurface(int width, int height);

    EGLBoolean makeCurrent(EGLSurface eglSurface);

    void unmakeCurrent();

    EGLBoolean swapBuffers(EGLSurface eglSurface);

    void destroySurface(EGLSurface eglSurface);

    void release();

};


#endif //SLOWER_BUGMEDIAEGL_H
