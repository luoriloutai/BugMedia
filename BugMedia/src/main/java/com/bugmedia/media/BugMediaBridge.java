package com.bugmedia.media;

import android.view.Surface;

public class BugMediaBridge {
    static {
        System.loadLibrary("bugmedia");
    }


    public static native void setWindowSurface(Surface surface,int rendererId);

    public static native void setPBufferSurface(int width,int height,int rendererId);

    public static native void destroy(int rendererId);

    protected static native void pause(int rendererId);

    public static native void play(int rendererId);

    public static native void stop(int rendererId);

    public static native void load();

    public static native void resizeView(int x,int y,int width,int height,int rendererId);

    public static native int createPictureRenderer(byte[] data,int width,int height);

    public static native void startRenderer(int rendererId);

    public static native void createPlayer(String url,Surface surface,int width,int height,int decoderBufferSize,boolean createPBufferSurface);



}
