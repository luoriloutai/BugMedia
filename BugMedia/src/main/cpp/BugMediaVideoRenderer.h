//
// Created by Gshine on 2021/1/31.
//

#ifndef SLOWER_BUGMEDIAVIDEORENDERER_H
#define SLOWER_BUGMEDIAVIDEORENDERER_H




#include "openGL/BugMediaBaseRenderer.h"
#include "BugMediaVideoLoader.h"

class BugMediaVideoRenderer : public BugMediaBaseRenderer {
    void setShaderSource();

    void startDraw();

    BugMediaVideoLoader *videoLoader{};
    BugMediaStateChangedCallback * callback{};

    void prepare();

    void render();

    enum State {
        STOP, PLAYING, PAUSE
    };

    State currentState;

public:
    BugMediaVideoRenderer(const char *url);

    ~BugMediaVideoRenderer();

    void play();

    void pause();

    void stop();

};


#endif //SLOWER_BUGMEDIAVIDEORENDERER_H
