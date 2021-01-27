//
// Created by Gshine on 2021/1/25.
//

#ifndef SLOWER_BUGMEDIABITMAPRENDERER_H
#define SLOWER_BUGMEDIABITMAPRENDERER_H

#include "openGL/core/BugMediaGraphics.h"
#include "SetShader.h"


class BugMediaBitmapRenderer : public BugMediaGraphics, SetShader {
protected:
    void onProgramPrepared();

    void onDraw();
    void setVertexDim(GLint dim,GLsizeiptr vertexSize,GLsizeiptr eleSize);

    GLint vertexCount = 0;
    GLint vertexDim=2;

public:
    void bindCurrentThread();

    void setFragmentShader();

    void setVertexShader();

};


#endif //SLOWER_BUGMEDIABITMAPRENDERER_H
