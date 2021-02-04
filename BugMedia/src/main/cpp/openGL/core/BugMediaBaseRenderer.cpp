//
// Created by Gshine on 2021/1/27.
//

#include "BugMediaBaseRenderer.h"


void BugMediaBaseRenderer::configVertex(GLint dim, GLsizeiptr vertexArraySize, GLsizeiptr eleSize) {
    vertexDim = dim;
    GLint vertexArrayLen = vertexArraySize / eleSize;
    vertexCount = vertexArrayLen / vertexDim;
}

BugMediaBaseRenderer::BugMediaBaseRenderer() {
     vertexCount = 0;
    // 顶点维度，调用configVertex之后该值发生改变
     vertexDim = 2;
}


