//
// Created by Gshine on 2021/1/25.
//

#ifndef SLOWER_BUGMEDIABITMAPRENDERER_H
#define SLOWER_BUGMEDIABITMAPRENDERER_H

#include "core/BugMediaGraphicsBaseRenderer.h"


class BugMediaBitmapRenderer : public BugMediaGraphicsBaseRenderer {
    void setShaderSource();

    void prepareDraw();

    void startDraw();

};


#endif //SLOWER_BUGMEDIABITMAPRENDERER_H
