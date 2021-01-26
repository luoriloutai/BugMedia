package com.bugmedia.media;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/*
* 渲染三角形
* 该实现不直接使用OpenGL和EGL，渲染及向窗口输出图像由GLSurfaceView实现
* 只需向GLSurfaceView提供渲染器
* */
public class TestTriangleRenderer implements GLSurfaceView.Renderer{



    // 每个顶点的坐标元素个数，(x,y,z)等形式里面的分量个数
    // 由于OpenGL坐标是以一维数组表示，所以需要指定几个元素为一组点
    // 这里只有二维，因此每两个元素表示一个点
    private int eleCountPerVertex =2;

    // 顶点个数，通过顶点数组可计算出来
    private int vertexCount=0;

    // OpenGL 中资源，用完要释放
    private int aPositionId=-1;
    private int vertexShaderId=-1;
    private int fragmentShaderId=-1;
    private int programId=-1;

    // 初始化方法要写在几个回调方法当中，要与绘图步骤在同一线程
    // 否则黑屏无法渲染，比如放在构造函数中，就无法渲染
    // 这里放在了onSurfaceCreated当中，只让它初始化一次
    void init() {


        float[] vertexCoords = {
                -1f, -1f,
                1f, -1f,
                0f, 1f
        };

        vertexCount = vertexCoords.length/eleCountPerVertex;

        ByteBuffer vertextByteBuffer = ByteBuffer.allocateDirect(vertexCoords.length * 4);
        vertextByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer vertextFloatBuffer = vertextByteBuffer.asFloatBuffer();
        vertextFloatBuffer.put(vertexCoords);
        vertextFloatBuffer.position(0);



        String vertextShaderCode = "attribute vec4 aPosition;" +
                "void main() {" +
                "  gl_Position = aPosition;" +
                "}";

        // 片元着色器内使用四维向量表示颜色，R，G，B，A，值从0到1
        String fragShaderCode = "precision mediump float;" +
                "void main() {" +
                "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);" +
                "}";

        vertexShaderId = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
        GLES20.glShaderSource(vertexShaderId, vertextShaderCode);
        GLES20.glCompileShader(vertexShaderId);
        int[] compiled = new int[1];
        GLES20.glGetShaderiv(vertexShaderId, GLES20.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            GLES20.glDeleteShader(vertexShaderId);
            Log.d("Load Shader Failed", "Compilation\n" + GLES20.glGetShaderInfoLog(vertexShaderId));
            return;
        }

        fragmentShaderId = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
        GLES20.glShaderSource(fragmentShaderId, fragShaderCode);
        GLES20.glCompileShader(fragmentShaderId);
        GLES20.glGetShaderiv(fragmentShaderId, GLES20.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            GLES20.glDeleteShader(fragmentShaderId);
            Log.d("Load Shader Failed", "Compilation\n" + GLES20.glGetShaderInfoLog(fragmentShaderId));
            return;
        }

        programId = GLES20.glCreateProgram();
        GLES20.glAttachShader(programId, vertexShaderId);
        GLES20.glAttachShader(programId, fragmentShaderId);

        GLES20.glLinkProgram(programId);

        GLES20.glUseProgram(programId);

        // 取着色器里定义的变量，取到其id或称为句柄
        aPositionId = GLES20.glGetAttribLocation(programId, "aPosition");


        // 启用变量
        GLES20.glEnableVertexAttribArray(aPositionId);


        // 为变量设置值。将之前定义的顶点数组与变量相关联。第二个参数表示一个点由几个元素构成
        GLES20.glVertexAttribPointer(aPositionId, eleCountPerVertex, GLES20.GL_FLOAT, false, 0, vertextFloatBuffer);

    }

    public void release(){
        GLES20.glDisableVertexAttribArray(aPositionId);
        GLES20.glDeleteShader(vertexShaderId);
        GLES20.glDeleteShader(fragmentShaderId);
        GLES20.glDeleteProgram(programId);
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
        //GLES20.glClearColor(0f, 0f, 0f, 0f);
        GLES20.glClearColor(0.643f, 0.776f, 0.223f, 1.0f);

        // 初始化着色器、顶点
        init();
    }

    @Override
    // i:width,i1:height
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        GLES20.glViewport(0, 0, i, i1);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {

        // 下面这句很重要，没有这句屏幕一直闪
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT|GLES20.GL_DEPTH_BUFFER_BIT);
        // 模式、在数组中的偏移、顶点数
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, vertexCount);


    }


}
