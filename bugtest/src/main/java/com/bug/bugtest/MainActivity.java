package com.bug.bugtest;

import androidx.appcompat.app.AppCompatActivity;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.MediaPlayer;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.bugmedia.media.GraphicsBridge;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    // 这个变量定义在方法外面，是因为在回调函数里初始化不能放在方法里面
    //private GraphicsEngine engine = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SurfaceView playView = findViewById(R.id.playerView);

        AssetManager assetManager = getAssets();
        try {
            AssetFileDescriptor fileDescriptor = assetManager.openFd("testfile.mp4");
            MediaPlayer player = new MediaPlayer();
            player.setDataSource(fileDescriptor.getFileDescriptor(), fileDescriptor.getStartOffset(),
                    fileDescriptor.getLength());
            fileDescriptor.close();

            SurfaceHolder holder = playView.getHolder();
            holder.addCallback(new SurfaceHolder.Callback() {
                @Override
                public void surfaceCreated(SurfaceHolder surfaceHolder) {
                    player.setSurface(surfaceHolder.getSurface());
                    player.prepareAsync();
                }

                @Override
                public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

                }

                @Override
                public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

                }
            });

            player.setOnPreparedListener(MediaPlayer::start);

        } catch (IOException e) {
            e.printStackTrace();
        }

        GLSurfaceView playView2 = findViewById(R.id.playerView2);

        String vertextShaderSource = "attribute vec4 aPosition;" +
                "void main() {" +
                "  gl_Position = aPosition;" +
                "}";

        // 片元着色器内使用四维向量表示颜色，R，G，B，A，值从0到1
        String fragShaderSource = "precision mediump float;" +
                "void main() {" +
                "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);" +
                "}";

        SurfaceView playView3 = findViewById(R.id.playerView3);

        SurfaceHolder v3Holder = playView3.getHolder();



        v3Holder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder surfaceHolder) {
                GraphicsBridge.selectRenderer(GraphicsBridge.TRIANGLE_RENDERER);
                GraphicsBridge.setWindowSurface(surfaceHolder.getSurface());
                GraphicsBridge.setViewport(0, 0, 500, 300);
                GraphicsBridge.draw();
            }

            @Override
            public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

            }
        });

//        GraphicsEngine engine=new GraphicsEngine();
//        //SurfaceTexture texture = new SurfaceTexture();
//        engine.setWindowSurface(playView2.getHolder().getSurface());
//


    }


}