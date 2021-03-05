package com.bug.bugtest;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.ThemedSpinnerAdapter;

import com.bugmedia.media.BugPlayer;
import com.bugmedia.media.PictureRenderer;

import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {

    SurfaceHolder holder = null;

    PictureRenderer pictureRenderer1;

    BugPlayer player1;


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            Log.d("BugMedia", "已授权");

        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SurfaceView view = findViewById(R.id.playerView3);
        holder = view.getHolder();
        //holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        if (ContextCompat.checkSelfPermission(MainActivity.this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(MainActivity.this,
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 1);
        } else {

            holder.addCallback(new SurfaceHolder.Callback() {
                @Override
                public void surfaceCreated(SurfaceHolder surfaceHolder) {
//                Bitmap pic = BitmapFactory.decodeResource(getResources(),R.drawable.icequeen);
//                int byteLen= pic.getByteCount();
//                ByteBuffer buffer = ByteBuffer.allocateDirect(byteLen);
//                pic.copyPixelsToBuffer(buffer);
//                byte[] picBytes=buffer.array();
//                int width=pic.getWidth();
//                int height = pic.getHeight();
//                int len = picBytes.length;



                    // 涉及到读写权限了，需要请求权限，AndroidManifest加权限后这里还需要请求授权，6.0以下的不需要
                    if (ContextCompat.checkSelfPermission(MainActivity.this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                        ActivityCompat.requestPermissions(MainActivity.this,
                                new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 1);
                    } else {
                        String dir = Environment.getExternalStorageDirectory().getPath();
                        String path = dir + "/testfile.mp4";
                        //String path = dir + "/bingo.aac";
                        //String path = dir + "/shang.mp3";
                        player1 = new BugPlayer(path, surfaceHolder.getSurface());
                        player1.load();
                    }


                }

                @Override
                public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

                    player1.resizeView(0,0,i1,i2);
                }

                @Override
                public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
                    player1.destroy();
                    //pictureRenderer1.destroy();
                }
            });


        }




        Button playBtn = findViewById(R.id.playButton);
        Button pauseBtn = findViewById(R.id.pauseButton);
        Button stopBtn = findViewById(R.id.stopButton);

        playBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                player1.play();
            }
        });

        pauseBtn.setOnClickListener(view1 -> player1.pause());

        stopBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                player1.stop();
            }
        });


//        LinearLayout parent = findViewById(R.id.parentContainer);
//        SurfaceView view = new SurfaceView(MainActivity.this);
//        parent.addView(view,0);
//        view.getLayoutParams().width = getWindowManager().getDefaultDisplay().getWidth();
//        view.getLayoutParams().height = getWindowManager().getDefaultDisplay().getWidth();
        //createMediaPlayer();


    }

    private void createMediaPlayer() {

//        setContentView(R.layout.activity_main);
//        SurfaceView playView = findViewById(R.id.playerView);
//
//        AssetManager assetManager = getAssets();
//        try {
//            AssetFileDescriptor fileDescriptor = assetManager.openFd("testfile.mp4");
//            MediaPlayer player = new MediaPlayer();
//            player.setDataSource(fileDescriptor.getFileDescriptor(), fileDescriptor.getStartOffset(),
//                    fileDescriptor.getLength());
//            fileDescriptor.close();
//
//            SurfaceHolder holder = playView.getHolder();
//            holder.addCallback(new SurfaceHolder.Callback() {
//                @Override
//                public void surfaceCreated(SurfaceHolder surfaceHolder) {
//                    player.setSurface(surfaceHolder.getSurface());
//                    player.prepareAsync();
//                }
//
//                @Override
//                public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
//
//                }
//
//                @Override
//                public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
//
//                }
//            });
//
//            player.setOnPreparedListener(MediaPlayer::start);
//
//        } catch (IOException e) {
//            e.printStackTrace();
//        }


    }



}