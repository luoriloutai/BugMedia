//
// Created by Gshine on 2021/1/27.
//

#include "BugMediaBaseRenderer.h"


void BugMediaBaseRenderer::configVertex(GLint dim, GLsizeiptr vertexArraySize, GLsizeiptr eleSize) {
    vertexDim = dim;
    GLint vertexArrayLen = vertexArraySize / eleSize;
    vertexCount = vertexArrayLen / vertexDim;
}


