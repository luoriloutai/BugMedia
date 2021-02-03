package com.bugmedia.media;

public abstract class BaseRenderer {

    protected int id = -1;


    public void setPBufferSurface(int width, int height) {
        GraphicsBridge.setPBufferSurface(width, height, id);
    }

    public void destroy() {
        GraphicsBridge.destroy(id);
    }

    public void resizeView(int x, int y, int width, int height) {
        GraphicsBridge.resizeView(x, y, width, height, id);
    }

    public void startRenderer() {
        GraphicsBridge.startRenderer(id);
    }

}
