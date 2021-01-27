//
// Created by Gshine on 2021/1/25.
//

#include "BugMediaBitmapRenderer.h"


void BugMediaBitmapRenderer::onProgramPrepared() {
    // 绘制前的准备。顶点坐标、缓冲等，能确定不改变的东西放在这里。

    // 顶点(x,y,z,w)，三维空间只有三维，最后一位是齐次坐标，将前面的三个量分别除以最后一个量得出
    // 一个三维向量(x/w,y/w,z/w)，这才是真正的三维坐标。最后一个值w是用来作调整的。
    // 一个顶点有二维的，有三维的，都扁平化地存储在一维数组里，因此我们需要手动指定哪部分表示一个顶点，
    // 即几个元素是一组，表示一个顶点。下面的坐标数组是以两个为一组表示一个顶点的，即一个二维坐标。
    // [(x,y),(x,y),(x,y)]相当于[vertex1,vertex2,vertex3],vertex=(x,y)。
    // 我把一个顶点由几个元素表示的数称为维度（vertex dim），二维坐标是2，三维坐标是3
    GLfloat vertexCoords[] = {
            -1.0, -1.0,
            1.0, -1.0,
            0, 1.0
    };

    GLsizeiptr vertexSize = sizeof(vertexCoords);
    GLsizeiptr eleSize = sizeof(GLfloat);
    // 顶点维度、顶点坐标数组大小（字节）、坐标数组每个元素的大小（字节）
    setVertexDim(2, vertexSize, eleSize);


    // 分配顶点缓冲区
    GLuint VBO;
    glGenBuffers(1, &VBO);
    // 可绑不同类型的多个Buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //  将顶点数据放入Buffer,之后的操作都从这里取数据
    // * GL_STATIC_DRAW ：数据不会或几乎不会改变。
    // * GL_DYNAMIC_DRAW：数据会被改变很多。
    // * GL_STREAM_DRAW ：数据每次绘制时都会改变。
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexCoords, GL_STATIC_DRAW);


    // 取着色器里定义的属性变量，取到其位置
    GLint aPosition = glGetAttribLocation(pGLESProgram->getProgram(), "aPosition");

    // 告诉OpenGL该如何解析顶点数据,为着色器属性变量设置值。
    // 该方法将从顶点缓冲区中获取数据，具体是哪个缓冲区取决于之前绑定的缓冲区
    //
    // 第二个参数表示一个点由几个元素构成
    // 第三个参数是坐标数组元素的数组类型
    // 第四个参数表示是否标准化坐标，即把坐标映射到0到1之间。
    // 第五个参数是步长，或叫跨度，即一个顶点所占用的字节数,即 顶点维度*数组中每个元素的大小
    // 第六个参数表示位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0。
    glVertexAttribPointer(aPosition, vertexDim, GL_FLOAT, GL_FALSE, vertexDim * eleSize, (void *) 0);

    // 启用顶点属性变量，默认是禁止的
    glEnableVertexAttribArray(aPosition);

    glViewport(0,0,500,300);

    // 纹理操作
    // glGenTextures(1, &texId);
    // glBindTexture(GL_TEXTURE_2D, texId);
    // 在最后释放纹理glBindTexture(GL_TEXTURE_2D, 0);这里不释放只作说明
    //
    // 设置纹理缩放效果
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //
    // 设置纹理在S和T方向上的排列效果,这是设置OpenGL纹理，左下角为原点，屏幕是左上角为原点
    // GL_CLAMP_TO_EDGE表示坐标夹在边界内，换句话说超过了边界1就设为1，小于0就设为0，总之就是不能超出去
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //
    //解码图片得到pixels数组
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // 纹理绘制到屏幕
    // *设置着色器
    // 规定窗口的大小： glViewport(0, 0, screenWidth, screenHeight)
    // *创建着色程序，使用它glUseProgram(mGLProgId);
    // *设置物体坐标
    // GLfloat vertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
    // glVertexAttribPointer(mGLVertexCoords, 2, GL_FLOAT, 0, 0, vertices);
    // glEnableVertexAttribArray(mGLVertexCoords);
    // *设置纹理坐标，textCoods1是opengl纹理坐标，textCoods2是屏幕纹理坐标，要判断纹理来自于哪里选择正确的纹理坐标，
    //  如果是把磁盘上的图片显示到屏幕就用第二个，否则第一个
    // GLfloat texCoords1[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    // GLfloat texCoords2[] = { 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
    // glVertexAttribPointer(mGLTextureCoords, 2, GL_FLOAT, 0, 0, texCoords2);
    // glEnableVertexAttribArray(mGLTextureCoords);
    // *指定将要绘制的纹理对象并且传递给对应的FragmentShader
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texId);
    // glUniform1i(mGLUniformTexture, 0);
    // 执行绘制操作：
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // 不需要纹理时删除
    // glDeleteTextures(1, &texId);

    /*
     * 演示使用glDrawElements
     *
     // 矩形顶点
     float vertices[] = {
            0.5f, 0.5f, 0.0f,   // 右上角
            0.5f, -0.5f, 0.0f,  // 右下角
            -0.5f, -0.5f, 0.0f, // 左下角
            -0.5f, 0.5f, 0.0f   // 左上角
    };
    // 该数组指定顶点的渲染顺序，哪几个连接在一起组成一个三角形
     GLfloat int indices[] = { // 注意索引从0开始!
            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
    };
    GLuint EBO;
    glGenBuffers(1, &EBO);
    // 缓冲类型为GL_ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // ...
    // 这里绘制时使用glDrawElements();
    // 参数一：绘制模式
    // 参数二：顶点数。定义了4个顶点，这里要绘制6个
    // 参数三：数组元素的类型
    // 参数四：在EBO中的偏移量
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
*/


}

void BugMediaBitmapRenderer::onDraw() {
    // 绘制过程设置,转换、拉伸等，会发生改变的东西放在这里。

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
}

void BugMediaBitmapRenderer::setVertexShader() {
    const char *vertextShaderSource = "attribute vec4 aPosition;"
                                      "void main() {"
                                      "  gl_Position = aPosition;"
                                      "}";
    const char* source= "attribute vec4 position; \n"
                        "attribute vec2 texcoord; \n"
                        "varying vec2 v_texcoord; \n"
                         "void main(void) \n"
                         "{ \n"
                         " gl_Position = position; \n"
                         " v_texcoord = texcoord; \n"
                         "} \n";
    BugMediaGraphics::setFragmentShader(&vertextShaderSource);
}

void BugMediaBitmapRenderer::setFragmentShader() {
    // 片元着色器内使用四维向量表示颜色，R，G，B，A，值从0到1
    const char *fragShaderSource = "precision mediump float;"
                                   "void main() {"
                                   "  gl_FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
                                   "}";
   const char* source = "precision highp float; \n"
                      "varying highp vec2 v_texcoord; \n"
                      "uniform sampler2D texSampler; \n"
                      "void main() { \n"
                            " gl_FragColor = texture2D(texSampler, v_texcoord); \n"
                       "} \n";
    BugMediaGraphics::setFragmentShader(&fragShaderSource);
}

void BugMediaBitmapRenderer::bindCurrentThread() {
    makeCurrent();
}

void BugMediaBitmapRenderer::setVertexDim(GLint dim, GLsizeiptr vertexSize, GLsizeiptr eleSize) {
    vertexDim = dim;

    GLint vertexLen = vertexSize / eleSize;
    vertexCount = vertexLen / vertexDim;
}

#ifdef __cplusplus
extern "C" {
#endif

BugMediaBitmapRenderer renderer;

void render() {
    renderer.setFragmentShader();
    renderer.setVertexShader();
    renderer.prepareProgram();

    //renderer.setVertexShader();
    //renderer.setFragmentShader();
    // 视频需要不断读取并显示，暂时没实现视频
    // while(){}
    renderer.draw();
    renderer.release();
}

#ifdef __cplusplus
};
#endif



//======== 以下为jni调用 ========



extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsEngine_render(JNIEnv *env, jobject thiz) {
    render();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsEngine_setPBufferSurface(JNIEnv *env, jobject thiz, jint width, jint height) {

    renderer.setPBufferSurface(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsEngine_setWindowSurface(JNIEnv *env, jobject thiz, jobject surface) {
    renderer.setWindowSurface(env, surface);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsEngine_bindCurrentThread(JNIEnv *env, jobject clazz) {
    renderer.bindCurrentThread();
}

