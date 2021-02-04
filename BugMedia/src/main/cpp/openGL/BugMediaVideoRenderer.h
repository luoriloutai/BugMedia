//
// Created by Gshine on 2021/1/31.
//

#ifndef SLOWER_BUGMEDIAVIDEORENDERER_H
#define SLOWER_BUGMEDIAVIDEORENDERER_H


#include "core/BugMediaBaseRenderer.h"
#include "../BugMediaVideoLoader.h"

class BugMediaVideoRenderer : public BugMediaBaseRenderer {
    void setShaderSource();

    void startDraw();

    BugMediaVideoLoader *videoDecoder;

    void prepare();

    void render();

    enum State {
        STOP, PLAYING, PAUSE
    };

    State currentState;

public:
    BugMediaVideoRenderer(BugMediaVideoLoader *decoder);

    ~BugMediaVideoRenderer();

    void play();

    void pause();

    void stop();
};


#endif //SLOWER_BUGMEDIAVIDEORENDERER_H
