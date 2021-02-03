package com.bug.bugtest;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.bugmedia.media.BaseRenderer;
import com.bugmedia.media.GraphicsBridge;
import com.bugmedia.media.PictureRenderer;

import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {



    SurfaceHolder holder = null;

    PictureRenderer pictureRenderer1;
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
        holder = view.getHolder();
        //holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        holder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder surfaceHolder) {
                Bitmap pic = BitmapFactory.decodeResource(getResources(),R.drawable.icequeen);
                int byteLen= pic.getByteCount();
                ByteBuffer buffer = ByteBuffer.allocateDirect(byteLen);
                pic.copyPixelsToBuffer(buffer);
                byte[] picBytes=buffer.array();
                int width=pic.getWidth();
                int height = pic.getHeight();
                int len = picBytes.length;



                pictureRenderer1 = new PictureRenderer(picBytes,width,height);

                pictureRenderer1.setWindowSurface(surfaceHolder.getSurface());

                pictureRenderer1.startRenderer();;
                Log.d("bugmedia","绘图器创建完成");
            }

            @Override
            public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
                pictureRenderer1.resizeView(0, 0, i1, i2);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
                pictureRenderer1.destroy();
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


    }


}