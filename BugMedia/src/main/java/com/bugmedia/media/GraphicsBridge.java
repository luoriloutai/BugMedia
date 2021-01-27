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
    public static native void selectRenderer(int rendererType);

    public static native void setWindowSurface(Surface surface);

    public static native void setViewport(int x, int y, int width, int height);

    public static native void setPBufferSurface(int width,int height);

    public static native void draw();

    public static native void stop();

    public static native void pause();

}
