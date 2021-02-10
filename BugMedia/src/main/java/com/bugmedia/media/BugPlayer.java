package com.bugmedia.media;

import android.view.Surface;

public class BugPlayer {

    public BugPlayer(String url, Surface surface) {
        BugMediaBridge.createPlayer(url, surface);
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

    public void setPBufferWindow(int width, int height, int rendererId) {
        BugMediaBridge.setPBufferSurface(width, height, 0);
    }
}
