package com.bug.bugtest;

import androidx.appcompat.app.AppCompatActivity;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.MediaPlayer;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.text.Layout;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.LinearLayout;

import com.bugmedia.media.GraphicsBridge;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {



    GraphicsBridge graphicsBridge=new GraphicsBridge();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

//        LinearLayout parent = findViewById(R.id.parentContainer);
//        SurfaceView view = new SurfaceView(MainActivity.this);
//        parent.addView(view,0);
//        view.getLayoutParams().width = getWindowManager().getDefaultDisplay().getWidth();
//        view.getLayoutParams().height = getWindowManager().getDefaultDisplay().getWidth();
        SurfaceView view = findViewById(R.id.playerView3);
        SurfaceHolder holder = view.getHolder();
        //holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        holder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder surfaceHolder) {
                graphicsBridge.selectRenderer(GraphicsBridge.TRIANGLE_RENDERER);
                graphicsBridge.setWindowSurface(surfaceHolder.getSurface());
                graphicsBridge.setViewport(0, 0, 500, 300);
                graphicsBridge.draw();
            }

            @Override
            public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
                graphicsBridge.destroy();
            }
        });



        /*
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


*/
        //GLSurfaceView playView2 = findViewById(R.id.playerView2);


//        SurfaceView playView3 = findViewById(R.id.playerView3);
//
//        SurfaceHolder v3Holder = playView3.getHolder();
//
//
//        v3Holder.addCallback(new SurfaceHolder.Callback() {
//            @Override
//            public void surfaceCreated(SurfaceHolder surfaceHolder) {
//                GraphicsBridge.selectRenderer(GraphicsBridge.TRIANGLE_RENDERER);
//                GraphicsBridge.setWindowSurface(surfaceHolder.getSurface());
//                GraphicsBridge.setViewport(0, 0, 500, 300);
//                GraphicsBridge.draw();
//            }
//
//            @Override
//            public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
//
//            }
//
//            @Override
//            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
//                GraphicsBridge.stop();
//            }
//        });


    }


}