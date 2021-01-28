//
// Created by Gshine on 2021/1/27.
//

#include "BugMediaGraphicsBaseRenderer.h"


void BugMediaGraphicsBaseRenderer::configVertex(GLint dim, GLsizeiptr vertexArraySize, GLsizeiptr eleSize) {
    vertexDim = dim;
    GLint vertexArrayLen = vertexArraySize / eleSize;
    vertexCount = vertexArrayLen / vertexDim;
}
