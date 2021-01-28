//
// Created by Gshine on 2021/1/27.
//

#include "BugMediaTriangleRenderer.h"
#include "core/BugMediaGraphicsCommon.h"

void BugMediaTriangleRenderer::setShader() {
    const char *vertextShaderSource = "attribute vec4 aPosition;"
                                      "void main() {"
                                      "  gl_Position = aPosition;"
                                      "}";
    const char *fragShaderSource = "precision mediump float;"
                                   "void main() {"
                                   "  gl_FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
                                   "}";

    //setShaderSource(&vertextShaderSource,&fragShaderSource);
    this->pGLES->setShaderSource(&vertextShaderSource, &fragShaderSource);

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
    configVertex(2, vertexSize, eleSize);
    pGLES->setVertexAttribArray("aPosition", vertexDim, GL_FLOAT, GL_FALSE, vertexDim * eleSize, vertexCoords);

    //glViewport(0,0,500,300);
    //pGLES->viewport(0, 0, 500, 300);

    // 初始化背景色
    pGLES->enable(GL_BLEND);
    pGLES->blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    pGLES->clearColor(0.643f, 0.776f, 0.223f, 1.0f);


}

void BugMediaTriangleRenderer::onDraw() {
    LOGD("onDraw开始");
    // 下面这句很重要，没有这句屏幕一直闪
    pGLES->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 模式、在数组中的偏移、顶点数
    pGLES->drawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
}

