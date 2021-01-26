package com.bugmedia.media;

import android.view.Surface;

public class GraphicsEngine {
    static {
        System.loadLibrary("bugmedia");
    }



    Thread thread = new Thread(new Runnable() {
        @Override
        public void run() {
            bindCurrentThread();
            render();
        }
    });

    public void draw() {
        thread.start();
    }

    public native void setWindowSurface(Surface surface);

    public native void setPBufferSurface(int width, int height);

    private native void render();

    private native void bindCurrentThread();


}
