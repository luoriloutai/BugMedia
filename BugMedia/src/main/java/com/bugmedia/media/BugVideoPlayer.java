package com.bugmedia.media;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class BugVideoPlayer extends GLSurfaceView {
    public BugVideoPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public BugVideoPlayer(Context context) {
        this(context, null);
    }

    private void init() {
        setEGLContextClientVersion(2);
        //setRenderer(new BugVideoRederer());
    }




}

