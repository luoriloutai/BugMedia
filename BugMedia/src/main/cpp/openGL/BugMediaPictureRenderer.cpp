//
// Created by Gshine on 2021/1/25.
//

#include "BugMediaPictureRenderer.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"


//
//    /*
//     * 演示使用glDrawElements
//     *
//     // 矩形顶点
//     float vertices[] = {
//            0.5f, 0.5f, 0.0f,   // 右上角
//            0.5f, -0.5f, 0.0f,  // 右下角
//            -0.5f, -0.5f, 0.0f, // 左下角
//            -0.5f, 0.5f, 0.0f   // 左上角
//    };
//    // 该数组指定顶点的渲染顺序，哪几个连接在一起组成一个三角形
//     GLfloat int indices[] = { // 注意索引从0开始!
//            0, 1, 3, // 第一个三角形
//            1, 2, 3  // 第二个三角形
//    };
//    GLuint EBO;
//    glGenBuffers(1, &EBO);
//    // 缓冲类型为GL_ELEMENT_ARRAY_BUFFER
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//    // ...
//    // 这里绘制时使用glDrawElements();
//    // 参数一：绘制模式
//    // 参数二：顶点数。定义了4个顶点，这里要绘制6个
//    // 参数三：数组元素的类型
//    // 参数四：在EBO中的偏移量
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//*/
//}






void BugMediaPictureRenderer::setShaderSource() {


    const char *vertextShaderSource = "attribute vec2 position; \n"
                                      "attribute vec2 texcoord; \n"
                                      "uniform mat4 view;\n"
                                      "varying vec2 v_texcoord; \n"
                                      "void main(void) \n"
                                      "{ \n"
                                      " gl_Position = view * vec4(position,0,1); \n"
                                      " v_texcoord = texcoord; \n"
                                      "} \n";

    const char *fragShaderSource = "precision highp float; \n"
                                   "varying highp vec2 v_texcoord; \n"
                                   "uniform sampler2D texSampler; \n"
                                   "void main() { \n"
                                   " gl_FragColor = texture2D(texSampler, v_texcoord); \n"
                                   "} \n";
    setShaderSources(vertextShaderSource, fragShaderSource);
}

void BugMediaPictureRenderer::startDraw() {
#ifdef DEBUGAPP
    LOGD("图像绘制开始");
#endif

    // 顶点坐标，以物体中心为原点
    // position属性是4维的，这里的坐标是2维的，应该会自动转换
    GLfloat vertices[] = {-1.0f, -1.0f,
                          1.0f, -1.0f,
                          -1.0f, 1.0f,
                          1.0f, 1.0f};

    // 顶点是以物体中心为原点的，这里取屏幕中心作为原点，
    // 取了四个角的点坐标放在数组中。顶点是用来绘图形的，
    // 简单的图形直接绘制，但复杂的效果不好直接绘制，比如
    // 绘制一张图片，做起来就相当费劲了。
    //
    // 如果把一张图片直接贴在图形上，不就大大简化了绘制的
    // 过程了吗。这种贴在图形上面的图像就叫做纹理。
    //
    // 要把图片即纹理贴到图形上，但是纹理坐标不是取屏幕中心为
    // 原点，如果直接把纹理贴到图形上有可能会出现纹理上下颠倒
    // 等各种奇怪现象。
    //
    // 这就需要转换，把顶点的各个坐标“翻译”成对应的纹理坐标。
    // 比如顶点坐标的屏幕左上角是(-1,1),那么它对应的纹理坐
    // 标就是(0,1)；如果转成屏幕坐标就是(0,0)。这个过程或者
    // 反过来说是将纹理坐标对应到顶点坐标，这样纹理贴到图形
    // 上时就不会出现上下颠倒等情况。


    // 纹理坐标，以物体左下角为原点，将顶点转成如下坐标
    GLfloat texCoords[] = {0.0f, 0.0f,
                           1.0f, 0.0f,
                           0.0f, 1.0f,
                           1.0f, 1.0f};

    // 屏幕坐标，以屏幕或物体左上角为原点，将顶点转为如下坐标
    GLfloat screenCoords[] = {0.0f, 1.0f,
                              1.0f, 1.0f,
                              0.0f, 0.0f,
                              1.0f, 0.0f};

    GLsizeiptr vertexSize = sizeof(vertices);
    GLsizeiptr eleSize = sizeof(GLfloat);
    // 顶点维度、顶点坐标数组大小（字节）、坐标数组每个元素的大小（字节）
    configVertex(2, vertexSize, eleSize);

    // 属性在着色器中是4维的，但是这里用数组并指定成二维也可以转成着色器需要的数据,
    // 猜测会根据给定的维度大小去取数据，这里只有二维，后二维估计使用默认值
    setVertexAttribArray("position", 2, GL_FLOAT, GL_FALSE, 0, vertices);
    // 要显示在屏幕上，所以用屏幕坐标
    setVertexAttribArray("texcoord", 2, GL_FLOAT, GL_FALSE, 0, screenCoords);

#ifdef DEBUGAPP
    LOGD("顶点属性设置完毕");
#endif


    //
    // 纹理操作
    //

    // 用图像数据创建2D纹理
    GLuint texId = set2DTexture0("texSampler", pixelData, width, height);

#ifdef DEBUGAPP
    LOGD("纹理创建完毕");
#endif

    //
    //设置纹理参数
    //
    // 过滤器设置。缩小设置
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // 过滤器设置。放大设置
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 对边界的约束。S表示横向，T表示纵向，纹理坐标以左下角为原点
    // GL_CLAMP_TO_EDGE表示将值控制在一定范围内，大于1设置为1，小于0设为0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //更新一个unform之前你必须先使用程序（调用glUseProgram)
    // 有动态变换的地方也应该先调用useProgram，否则绘制的图形不会变
    useProgram();

    // 屏幕旋转后进行缩放
    // 待完成


//    // Projection matrix
//    glm::mat4 Projection = glm::ortho(-(float_t) width / height, (float_t) width / height, -1.0f, 1.0f, 0.1f, 100.0f);
//    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
//    //glm::mat4 Projection = glm::perspective(45.0f,(float_t)width/height, 0.1f,100.f);
//
//    // View matrix
//    glm::mat4 View = glm::lookAt(
//            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
//            glm::vec3(0, 0, 0), // and looks at the origin
//            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//    );
//
//    // Model matrix
//    glm::mat4 Model = glm::mat4(1.0f);
//    Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0f));
//    Model = glm::rotate(Model, 20.0f, glm::vec3(1.0f, 0.0f, 0.0f));
//    Model = glm::rotate(Model, 15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
//    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
//
//    glm::mat4 viewMat = Projection * View * Model;
//    GLuint viewLoc = glGetUniformLocation(getProgram(), "view");
//    glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(viewMat));



    glm::mat4 viewMat(1.0f);
    GLuint viewLoc = glGetUniformLocation(getProgram(), "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

    //
    // 绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
    swapBuffers();

#ifdef DEBUGAPP
    LOGD("绘制完毕");
#endif

    //
    // 释放资源
    unbind2DTexture0(&texId);
    release();
}

BugMediaPictureRenderer::BugMediaPictureRenderer(uint8_t *data, GLint width, GLint height) {
    pixelData = data;
    this->width = width;
    this->height = height;
}

BugMediaPictureRenderer::~BugMediaPictureRenderer() {
    delete[] pixelData;

}