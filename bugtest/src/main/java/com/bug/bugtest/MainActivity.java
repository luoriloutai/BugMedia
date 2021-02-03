package com.bug.bugtest;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.bugmedia.media.GraphicsBridge;

import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {


    GraphicsBridge graphicsBridge = new GraphicsBridge();
    SurfaceHolder holder = null;
    int picRendererId;

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


                picRendererId = graphicsBridge.createPictureRenderer(picBytes, width, height);
                Log.d("bugmedia", "图像渲染器id："+picRendererId);
                graphicsBridge.setWindowSurface(surfaceHolder.getSurface(),picRendererId);

                graphicsBridge.startRenderer(picRendererId);;
                Log.d("bugmedia","绘图器创建完成");
            }

            @Override
            public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
                graphicsBridge.resizeView(0, 0, i1, i2,picRendererId);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
                graphicsBridge.destroy(picRendererId);
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