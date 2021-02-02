package com.bug.slower;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.BitmapFactory;
import android.media.MediaPlayer;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.SurfaceView;

import com.bugmedia.media.TestBitmapRenderer;
import com.bugmedia.media.TestTriangleRenderer;
import com.bugmedia.media.TestVideoRenderer;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        GLSurfaceView mSurfaceView = findViewById(R.id.sfv);
        mSurfaceView.setEGLContextClientVersion(2);

//// 测试三角形
//renderer = new TestTriangleRenderer();

//        // 测试图像
        renderer = new TestBitmapRenderer(BitmapFactory.decodeResource(getResources(), R.drawable.b1));

//        // 测试视频
//        renderer = new TestVideoRenderer("testfile.mp4");

        mSurfaceView.setRenderer(renderer);


    }

    private GLSurfaceView.Renderer renderer;

    @Override
    protected void onDestroy() {
        super.onDestroy();

        //((TestTriangleRenderer) renderer).release();
        //((TestBitmapRenderer)renderer).release();
    }
}