package com.bugmedia.media;

import android.view.Surface;


public class PictureRenderer extends BaseRenderer{

    public PictureRenderer(byte[] data,int picWidth,int picHeight,Surface surface){
        id=GraphicsBridge.createPictureRenderer(data,picWidth,picHeight);
        GraphicsBridge.setWindowSurface(surface,id);
    }


}
