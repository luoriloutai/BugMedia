package com.bugmedia.media;

import android.view.Surface;

public class BugMediaBridge {
    static {
        System.loadLibrary("bugmedia");
    }

    private Surface surface;
    public BugMediaBridge(Surface surface){
        this.surface = surface;
    }

    public Surface getSurface(){
        return this.surface;
    }

    public static native void setWindowSurface(Surface surface,int rendererId);

    public static native void setPBufferSurface(int width,int height,int rendererId);

    public static native void destroy(int rendererId);

    protected static native void pause(int rendererId);

    public static native void play(int rendererId);

    public static native void stop(int rendererId);

    public static native void load(int rendererId);

    public static native void resizeView(int x,int y,int width,int height,int rendererId);

    public static native int createPictureRenderer(byte[] data,int width,int height);

    public static native int createPictureRenderer1(byte[] data,int width,int height,Surface surface);

    public static native void startRenderer(int rendererId);

    public static native void createPlayer(String url,Surface surface,int width,int height,int decoderBufferSize,boolean createPBufferSurface);



}
