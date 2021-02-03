package com.bugmedia.media;

import android.view.Surface;


public class PictureRenderer extends BaseRenderer{

    public PictureRenderer(byte[] data,int picWidth,int picHeight){
        id=GraphicsBridge.createPictureRenderer(data,picWidth,picHeight);
    }


}
