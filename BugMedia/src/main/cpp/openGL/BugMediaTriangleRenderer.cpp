//
// Created by Gshine on 2021/1/27.
//

#include "BugMediaTriangleRenderer.h"
#include "core/BugMediaGraphicsCommon.h"

void BugMediaTriangleRenderer::setShaderSource() {
    //
    // 本方法只实现设置着色器代码，其它操作不应在这里。
    // 如果要配置着色器数据，请在prepareDraw()中编写代码。
    //

    const GLchar *vertextShaderSource = "attribute vec4 aPosition;\n"
                                        "void main(){\n"
                                        "  gl_Position = aPosition;\n"
                                        "}";
    const GLchar *fragShaderSource = "precision mediump float;\n"
                                     "void main(){\n"
                                     " gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                                     "}";

    this->pGLES->setShaderSource(vertextShaderSource, fragShaderSource);
}

void BugMediaTriangleRenderer::startDraw() {
#ifdef DEBUGAPP
    LOGD("startDraw开始");
#endif

    //
    // 顶点(x,y,z,w)，三维空间只有三维，最后一位是齐次坐标，将前面的三个量分别除以最后一个量得出
    // 一个三维向量(x/w,y/w,z/w)，这才是真正的三维坐标。最后一个值w是用来作调整的。
    // 一个顶点有二维的，有三维的，都扁平化地存储在一维数组里，因此我们需要手动指定哪部分表示一个顶点，
    // 即几个元素是一组，表示一个顶点。下面的坐标数组是以两个为一组表示一个顶点的，即一个二维坐标。
    // [(x,y),(x,y),(x,y)]相当于[vertex1,vertex2,vertex3],vertex=(x,y)。
    // 我把一个顶点由几个元素表示的数称为维度（vertex dim），二维坐标是2，三维坐标是3
    GLfloat vertexCoords[] = {
            -1.0, -1.0,
            1.0, -1.0,
            0.0, 1.0
    };

    GLsizeiptr vertexSize = sizeof(vertexCoords);
    GLsizeiptr eleSize = sizeof(GLfloat);
    // 顶点维度、顶点坐标数组大小（字节）、坐标数组每个元素的大小（字节）
    configVertex(2, vertexSize, eleSize);


    // 第一个参数是顶点着色器中的量的名字
    // 第二个参数表示一个点由几个元素构成
    // 第三个参数是坐标数组元素的数组类型
    // 第四个参数表示是否标准化坐标，即把坐标映射到0到1之间。
    // 第五个参数数组字节大小
    // 第六个参数表示位置数据在缓冲中起始位置的偏移量(Offset)。由于数据在数组的开头，所以这里是(viod*)0。
    GLuint aPosition = pGLES->setVertexAttribArray("aPosition", vertexDim, GL_FLOAT, GL_FALSE,sizeof(vertexCoords),vertexCoords,(void*)0);

    //setViewPort(50, 50, 800, 800);

    // 初始化背景色
    pGLES->enable(GL_BLEND);
    pGLES->blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    pGLES->clearColor(0.643, 0.776, 0.223, 1.0);


    // 下面这句很重要，没有这句屏幕一直闪
    pGLES->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 模式、在数组中的偏移、顶点数
    pGLES->drawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
    pEGL->swapBuffers();
#ifdef DEBUGAPP
    LOGD("顶点数:%d",vertexCount);
#endif
}



