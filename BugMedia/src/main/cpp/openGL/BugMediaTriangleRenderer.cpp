//
// Created by Gshine on 2021/1/27.
//
// 这个类实际上没什么作用，用作示例演示
//

#include "BugMediaTriangleRenderer.h"
#include "core/BugMediaGraphicsCommon.h"

void BugMediaTriangleRenderer::setShaderSource() {
    //
    // 本方法只实现设置着色器代码，其它操作不应在这里。
    //
    //

    const GLchar *vertextShaderSource = "attribute vec4 aPosition;\n"
                                        "void main(){\n"
                                        "  gl_Position = aPosition;\n"
                                        "}";
    const GLchar *fragShaderSource = "precision mediump float;\n"
                                     "void main(){\n"
                                     " gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                                     "}";

    setShaderSources(vertextShaderSource, fragShaderSource);
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
            -1.0f, -1.0f,
            1.0f, -1.0f,
            0.0f, 1.0f
    };

    GLsizeiptr vertexSize = sizeof(vertexCoords);
    GLsizeiptr eleSize = sizeof(GLfloat);
    // 顶点维度、顶点坐标数组大小（字节）、坐标数组每个元素的大小（字节）
    configVertex(2, vertexSize, eleSize);



    // 第一个参数是顶点着色器中的量的名字
    // 第二个参数表示一个点由几个元素构成
    // 第三个参数是坐标数组元素的数组类型
    // 第四个参数表示是否标准化坐标，即把坐标映射到0到1之间。
    // 第五个参数为数组
    GLuint aPosition = setVertexAttribArray("aPosition", vertexDim, GL_FLOAT, GL_FALSE, 0,vertexCoords);

//    //
//    /// 使用缓冲区设置顶点属性演示
//    //
//    GLuint VBO;
//    glGenBuffers(1, &VBO);
//
//    // 可绑不同类型的多个Buffer
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//
//    //  将顶点数据放入Buffer,之后的操作都从这里取数据
//    // * GL_STATIC_DRAW ：数据不会或几乎不会改变。
//    // * GL_DYNAMIC_DRAW：数据会被改变很多。
//    // * GL_STREAM_DRAW ：数据每次绘制时都会改变。
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
//
//    GLuint attribPosition = glGetAttribLocation(getProgram(),"aPosition");
//    // 该方法有两种用法：
//    // 1.使用缓冲区，上面代码保持不动，用法如下
//    // 最后一个参数表示所取数据在缓冲中起始位置的偏移量(Offset)。由于数据在数组的开头，所以这里是(viod*)0。
//    // 如果数据有偏移，则需要计算出偏移。该偏移是在一个跨度内的偏移，并不是在整个缓冲区的偏移
//    glVertexAttribPointer(attribPosition, vertexDim, GL_FLOAT, GL_FALSE, vertexDim * sizeof(GLfloat), (void *) 0);
//    // 2.不使用缓冲区，直接如下调用
//    glVertexAttribPointer(attribPosition, vertexDim, GL_FLOAT, GL_FALSE, 0, array);
//
//    // 这两句放在代码最后，当使用了缓冲时释放缓冲
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glDeleteBuffers(1, &VBO);

    // 初始化背景色
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.643, 0.776, 0.223, 1.0);

    // 下面这句很重要，没有这句屏幕一直闪
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 模式、在数组中的偏移、顶点数
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
    swapBuffers();

    release();
#ifdef DEBUGAPP
    LOGD("顶点数:%d", vertexCount);
#endif
}



