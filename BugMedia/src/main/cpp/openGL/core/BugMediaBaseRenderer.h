//
// Created by Gshine on 2021/1/27.
//

#ifndef SLOWER_BUGMEDIABASERENDERER_H
#define SLOWER_BUGMEDIABASERENDERER_H

#include "BugMediaGraphics.h"


class BugMediaBaseRenderer : public BugMediaGraphics {
public:

    int id;

    BugMediaBaseRenderer() {}

    virtual ~BugMediaBaseRenderer() {}

protected:
    //配置顶点
    //dim：维度，二维为2，三维为3
    // vertexArraySize:顶点数组的字节大小
    // eleSize:顶点数组元素的字节大小
    void configVertex(GLint dim, GLsizeiptr vertexArraySize, GLsizeiptr eleSize);

    // 顶点个数
    GLint vertexCount = 0;
    // 顶点维度，调用config之后该值发生改变
    GLint vertexDim = 2;

};


#endif //SLOWER_BUGMEDIABASERENDERER_H
