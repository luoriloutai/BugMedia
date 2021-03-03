package com.bugmedia.media;

public abstract class BaseRenderer {

    protected int id = -1;


    public void setPBufferSurface(int width, int height) {
        BugMediaBridge.setPBufferSurface(width, height, id);
    }

    public void destroy() {
        BugMediaBridge.destroy(id);
    }

    public void resizeView(int x, int y, int width, int height) {
        BugMediaBridge.resizeView(x, y, width, height, id);
    }

    public void startRenderer() {
        BugMediaBridge.startRenderer(id);
    }

}
