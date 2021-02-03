package com.bugmedia.media;

import android.view.Surface;

public class GraphicsBridge {
    static {
        System.loadLibrary("bugmedia");
    }


    public native void setWindowSurface(Surface surface,int rendererId);

    public native void setPBufferSurface(int width,int height,int rendererId);

    public native void destroy(int rendererId);

    public native void pause(int rendererId);

    public native void resizeView(int x,int y,int width,int height,int rendererId);

    public native int createPictureRenderer(byte[] data,int width,int height);

    public native void startRenderer(int rendererId);

}
