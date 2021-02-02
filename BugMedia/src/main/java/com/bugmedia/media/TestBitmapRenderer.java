package com.bugmedia.media;

import android.graphics.Bitmap;
import android.opengl.GLES10;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.opengl.Matrix;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * 渲染图像。以图像作为纹理
 * 该实现不直接使用OpenGL和EGL，渲染及向窗口输出图像由GLSurfaceView实现
 */
public class TestBitmapRenderer implements GLSurfaceView.Renderer {
    private Bitmap mBitmap;


    public TestBitmapRenderer(Bitmap bitmap) {
        mBitmap = bitmap;

    }

    private final String TAG = "TESTBITMAP";

    // 每个顶点的坐标元素个数，(x,y,z)等形式里面的分量个数
    // 由于OpenGL坐标是以一维数组表示，所以需要指定几个元素为一组点
    // 这里只有二维，因此每两个元素表示一个点
    private int eleCountPerVertex = 2;

    // 顶点个数，通过顶点数组可计算出来
    private int vertexCount = 0;

    // 投影矩阵，用于变换坐标
    private float[] mProjMatrix = new float[16];
    // 相机位置矩阵
    private float[] mLookAtMatrix = new float[16];
    // 最终变换结果矩阵，赋给着色器
    private float[] mTransformMatrix = new float[16];

    // 下面这些资源在最后要释放，定义为全局变量以供释放时使用
    private int aPositionId = -1;
    private int aCoordinateId = -1;
    private int programId = -1;
    private int uTextureId = -1;
    private int vertexShaderId = -1;
    private int fragmentShaderId = -1;
    private int[] textureIds;
    private int uMetrixId = -1;


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

        // 屏幕坐标，左上角为（0，0）
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


        // 变换未完善
        String vertextShaderCode = "attribute vec4 aPosition;" +
                "attribute vec2 aCoordinate;" +
                "uniform mat4 uMatrix;" +
                "varying vec2 vCoordinate;" +
                "void main() {" +
                "  gl_Position = aPosition*uMatrix;" +
                "  vCoordinate = aCoordinate;" +
                "}";

        // 片元着色器中把颜色指定成了纹理
        // vCoordinate从顶点着色器拿到的，标记为varying的变量从顶点着色器传递到片元着色器
        // 这里将图像显示为黑白，滤镜就是这么实现的
        // 显示原图，只需将color赋给gl_FragColor
        String fragShaderCode = "precision mediump float;" +
                "uniform sampler2D uTexture;" +
                "varying vec2 vCoordinate;" +
                "void main() {" +
                "  vec4 color = texture2D(uTexture, vCoordinate);" +
                "  float gray = (color.r + color.g + color.b)/3.0;" +
                "  gl_FragColor = vec4(gray, gray, gray, 1.0);" +
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

        int programId = GLES20.glCreateProgram();
        GLES20.glAttachShader(programId, vertexShaderId);
        GLES20.glAttachShader(programId, fragmentShaderId);

        GLES20.glLinkProgram(programId);

        GLES20.glUseProgram(programId);

        // 取着色器里定义的变量，取到其id
        aPositionId = GLES20.glGetAttribLocation(programId, "aPosition");
        aCoordinateId = GLES20.glGetAttribLocation(programId, "aCoordinate");
        uTextureId = GLES20.glGetUniformLocation(programId, "uTexture");
        uMetrixId = GLES20.glGetUniformLocation(programId, "uMatrix");


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
        //激活一个纹理单元
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        //绑定纹理ID到纹理单元
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureId);


        //将激活的纹理单元传递到着色器里面，第二个参数要与前面GL_Texture后面的数字一致
        // 注意到纹理设置与普通变量设置不同，并不是使用glVertexAttribPointer
        GLES20.glUniform1i(uTextureId, 0);
        //设置纹理参数。纹理表现形式，是平铺或是拉伸等
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        if (!mBitmap.isRecycled()) {
            //绑定图片到被激活的纹理单元
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, mBitmap, 0);

        }

        Matrix.setLookAtM(mLookAtMatrix, 0, 0, 0, 4f, 0f, 0f, 0f, 0f, 1.0f, 0.0f);
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
        GLES20.glClearColor(0f, 0f, 0f, 0f);

        // 初始化着色器、顶点
        init();
    }


    @Override
    // i:width,i1:height
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        GLES20.glViewport(0, 0, i, i1);
        float ratio = (float) i / i1;

        Matrix.frustumM(mProjMatrix, 0, -ratio, ratio, -1, 1, 3, 4);
        //
        // 注意：这两个矩阵相乘时，投影矩阵在前，相机位置矩阵在后，否则黑屏
        Matrix.multiplyMM(mTransformMatrix, 0, mProjMatrix, 0, mLookAtMatrix, 0);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);

        //  为矩阵设置参数
        GLES20.glUniformMatrix4fv(uMetrixId, 1, false, mTransformMatrix, 0);


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
