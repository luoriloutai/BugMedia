package com.bugmedia.media;

import android.view.Surface;

public class BugPlayer {

    public BugPlayer(String url, Surface surface) {
        this(url, surface, 0, 0, 10, false);

    }

    public BugPlayer(String url, Surface surface, int decoderBufferSize) {
        this(url, surface, 0, 0, decoderBufferSize, false);

    }

    public BugPlayer(String url, Surface surface, int width, int height, int decoderBufferSize,
                     boolean createPBufferSurface) {
        BugMediaBridge.createPlayer(url, surface, width, height, decoderBufferSize, createPBufferSurface);
    }

    public void play() {
        BugMediaBridge.play(0);
    }

    public void pause() {
        BugMediaBridge.pause(0);
    }

    public void stop() {
        BugMediaBridge.stop(0);
    }

    public void destroy() {
        BugMediaBridge.destroy(0);
    }

    public void resizeView(int x, int y, int width, int height){
        BugMediaBridge.resizeView(x,y,width,height,0);
    }

    public void load(){
        BugMediaBridge.load(0);
    }

    public void setPBufferWindow(int width, int height, int rendererId) {
        BugMediaBridge.setPBufferSurface(width, height, 0);
    }
}
