package com.bugmedia.media;

import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * 渲染视频
 */
public class TestVideoRenderer implements GLSurfaceView.Renderer {

    public TestVideoRenderer(String filePath) {
        this.filePath = filePath;
    }

    static {
        System.loadLibrary("bugmedia");
    }

    public native void setSurface(SurfaceTexture surface);

    private String filePath = "";

    // 每个顶点的坐标元素个数，(x,y,z)等形式里面的分量个数
    // 由于OpenGL坐标是以一维数组表示，所以需要指定几个元素为一组点
    // 这里只有二维，因此每两个元素表示一个点
    private int eleCountPerVertex = 2;

    // 顶点个数，通过顶点数组可计算出来
    private int vertexCount = 0;

    private boolean updateSurface = false;

    // 下面这些资源在最后要释放，定义为全局变量以供释放时使用
    private int aPositionId = -1;
    private int aCoordinateId = -1;
    private int programId = -1;
    private int uTextureId = -1;
    private int vertexShaderId = -1;
    private int fragmentShaderId = -1;
    private int[] textureIds;
    SurfaceTexture surfaceTexture;


    // 初始化方法要写在几个回调方法当中，要与绘图步骤在同一线程
    // 否则黑屏无法渲染，比如放在构造函数中就无法渲染
    // 这里调用放在了onSurfaceCreated当中，只让它初始化一次
    void init() {

        // 世界坐标，屏幕中心为原点
        // 四个顶点：左下，右下，左上，右上，依定义顺序每三个顶点构成一个三角形
        // 四个顶点组成两个三角形，两个三角形组成一个长方形
        float[] vertexCoords = {
                -1f, -1f,
                1f, -1f,
                -1f, 1f,
                1f, 1f
        };

        vertexCount = vertexCoords.length / eleCountPerVertex;


        // Float数据类型占多少字节
        int sizeOfFloat = Float.SIZE / 8; // Float.SIZE是以bit为单位的数，需要除以8得到字节数
        //Log.i("FloatSize",Float.toString(sizeOfFloat) );
        // 为顶点坐标分配空间，allocateDirect直分配内存，不需要将内存空间复制到JVM，少了一个复制操作，提高效率
        ByteBuffer vertextByteBuffer = ByteBuffer.allocateDirect(vertexCoords.length * sizeOfFloat);
        vertextByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer vertextFloatBuffer = vertextByteBuffer.asFloatBuffer();
        vertextFloatBuffer.put(vertexCoords);
        vertextFloatBuffer.position(0);

        // 纹理坐标，即屏幕坐标，左上角为（0，0）
        float[] textureCoords = {
                0f, 1f,
                1f, 1f,
                0f, 0f,
                1f, 0f

        };


        ByteBuffer textureByteBuffer = ByteBuffer.allocateDirect(textureCoords.length * sizeOfFloat);
        textureByteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer textureFloatBuffer = textureByteBuffer.asFloatBuffer();
        textureFloatBuffer.put(textureCoords);
        textureFloatBuffer.position(0);

        String vertextShaderCode = "attribute vec4 aPosition;" +
                "attribute vec2 aCoordinate;" +
                "uniform mat4 uMatrix;" +
                "varying vec2 vCoordinate;" +
                "void main() {" +
                "  gl_Position = aPosition;" +
                "  vCoordinate = aCoordinate;" +
                "}";

        // 片元着色器
        // vCoordinate从顶点着色器拿到的，标记为varying的变量从顶点着色器传递到片元着色器
        // 第一行，渲染视频使用，将YUV输出为RGB。注意末尾加换行，否则编译错误
        // 第四行，扩展纹理单元，不是普通的图像纹理
        String fragShaderCode = "#extension GL_OES_EGL_image_external : require\n" +
                "precision mediump float;" +
                "varying vec2 vCoordinate;" +
                "uniform samplerExternalOES uTexture;" +
                "void main() {" +
                "  gl_FragColor=texture2D(uTexture, vCoordinate);" +
                "}";

        vertexShaderId = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
        GLES20.glShaderSource(vertexShaderId, vertextShaderCode);
        GLES20.glCompileShader(vertexShaderId);
        int[] compiled = new int[1];
        GLES20.glGetShaderiv(vertexShaderId, GLES20.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            Log.d("Load Shader Failed", "Compilation\n" + GLES20.glGetShaderInfoLog(vertexShaderId));
            return;
        }

        fragmentShaderId = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
        GLES20.glShaderSource(fragmentShaderId, fragShaderCode);
        GLES20.glCompileShader(fragmentShaderId);
        GLES20.glGetShaderiv(fragmentShaderId, GLES20.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            Log.d("Load Shader Failed", "Compilation\n" + GLES20.glGetShaderInfoLog(fragmentShaderId));
            return;
        }

        int programId = GLES20.glCreateProgram();
        GLES20.glAttachShader(programId, vertexShaderId);
        GLES20.glAttachShader(programId, fragmentShaderId);

        GLES20.glLinkProgram(programId);

        GLES20.glUseProgram(programId);

        // 取着色器里定义的变量，取到其id
        aPositionId = GLES20.glGetAttribLocation(programId, "aPosition");
        aCoordinateId = GLES20.glGetAttribLocation(programId, "aCoordinate");
        uTextureId = GLES20.glGetUniformLocation(programId, "uTexture");
        // 启用变量
        GLES20.glEnableVertexAttribArray(aPositionId);
        GLES20.glEnableVertexAttribArray(aCoordinateId);

        // 为变量设置值。将之前定义的顶点数组与变量相关联。第二个参数表示一个点由几个元素构成
        GLES20.glVertexAttribPointer(aPositionId, eleCountPerVertex, GLES20.GL_FLOAT, false, 0, vertextFloatBuffer);
        GLES20.glVertexAttribPointer(aCoordinateId, eleCountPerVertex, GLES20.GL_FLOAT, false, 0, textureFloatBuffer);



        // 纹理，即图形贴图，如果没有纹理，就只绘制图形。有了纹理，纹理会覆盖在图形表面
        // 在该例中，用一张图片作为纹理，就会用这张图片填充用顶点表示的区域。
        textureIds = new int[1]; // 生成的纹理的id存储的地方
        GLES20.glGenTextures(1, textureIds, 0); //生成纹理,第一个参数表示生成几个
        int mTextureId = textureIds[0]; // 生成了一个纹理，这里使用一个
        //激活指定纹理单元
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        //绑定纹理ID到纹理单元
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, mTextureId);
        //将激活的纹理单元传递到着色器里面
        GLES20.glUniform1i(uTextureId, 0);
        //配置边缘过渡参数
        // 使用扩展
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        surfaceTexture = new SurfaceTexture(mTextureId);
        surfaceTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
            @Override
            synchronized public void onFrameAvailable(SurfaceTexture surfaceTexture) {

                // 防止多线程访问导致混乱，使用同步方法
                // 当有新数据可用时调用该方法
                updateSurface = true;
            }
        });

    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
        GLES20.glClearColor(0f, 0f, 0f, 0f);

        // 在这里初始化
        init();
    }

    @Override
    // i:width,i1:height
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        GLES20.glViewport(0, 0, i, i1);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
synchronized (this){
    if(updateSurface){
        surfaceTexture.updateTexImage();
        updateSurface=false;
    }
}
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, vertexCount);
    }


    public void release() {
        GLES20.glDisableVertexAttribArray(aPositionId);
        GLES20.glDisableVertexAttribArray(aCoordinateId);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glDeleteTextures(1, textureIds, 0);
        GLES20.glDeleteShader(vertexShaderId);
        GLES20.glDeleteShader(fragmentShaderId);
        GLES20.glDeleteProgram(programId);
    }
}
