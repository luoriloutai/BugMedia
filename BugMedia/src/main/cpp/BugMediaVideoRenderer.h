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

    void prepare();

    void render();

    enum State {
        STOP, PLAYING, PAUSE
    };

    State currentState;

    uint8_t* data{};
    int width=0;
    int height=0;

    BugMediaVideoLoader * videoLoader{};

public:
    BugMediaVideoRenderer(BugMediaVideoLoader * loader);

    ~BugMediaVideoRenderer();

    void play();

    void pause();

    void stop();

//    typedef int (*GetVideoFrameCallback)(uint8_t* data,int *width,int *height,void *ctx);
//
//    GetVideoFrameCallback getFrameData{};

};


#endif //SLOWER_BUGMEDIAVIDEORENDERER_H
