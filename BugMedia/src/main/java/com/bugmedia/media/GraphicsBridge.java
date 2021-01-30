package com.bugmedia.media;

import android.view.Surface;

public class GraphicsBridge {
    static {
        System.loadLibrary("bugmedia");
    }


    //三角形渲染器
    public final static  int TRIANGLE_RENDERER=1;
    // 位图渲染器
    public final static  int BITMAP_RENDERER=2;
    // 视频渲染器
    public final static int VIDEO_RENDERER=3;

    // 先调用这个方法
    public native void selectRenderer(int rendererType);

    public native void setWindowSurface(Surface surface);

    public native void setPBufferSurface(int width,int height);

    // repeat:是否重复绘制，用于不间断绘制
    public native void draw(boolean repeat);

    public native void destroy();

    public native void pause();

    public native void resize(int x,int y,int width,int height);

}
