//
// Created by Gshine on 2021/1/31.
//

#ifndef SLOWER_BUGMEDIAVIDEORENDERER_H
#define SLOWER_BUGMEDIAVIDEORENDERER_H


#include "core/BugMediaBaseRenderer.h"

class BugMediaVideoRenderer: public BugMediaBaseRenderer {
    void setShaderSource();

    void startDraw();
};


#endif //SLOWER_BUGMEDIAVIDEORENDERER_H
