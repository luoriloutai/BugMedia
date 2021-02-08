//
// Created by Gshine on 2021/1/25.
//

#ifndef SLOWER_BUGMEDIAPICTURERENDERER_H
#define SLOWER_BUGMEDIAPICTURERENDERER_H

#include "BugMediaBaseRenderer.h"
#include "BugMediaRendererCommon.h"


class BugMediaPictureRenderer : public BugMediaBaseRenderer {
    void setShaderSource();

    void onRender();

    uint8_t *pixelData;
    GLint width;
    GLint height;

public:
    BugMediaPictureRenderer(uint8_t* data,GLint width,GLint height);
    ~BugMediaPictureRenderer();


};


#endif //SLOWER_BUGMEDIAPICTURERENDERER_H
