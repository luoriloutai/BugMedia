package com.bugmedia.media;

import android.view.Surface;

public abstract class BaseRenderer {

    protected int id = -1;


    public void setWindowSurface(Surface surface) {
        GraphicsBridge.setWindowSurface(surface, id);
    }

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
