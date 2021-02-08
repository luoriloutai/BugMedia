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

    //渲染并返回是否结束条件
    bool render();

    State currentState = UNSTART;
    int64_t audioPts{};
    uint8_t *data{};
    int width = 0;
    int height = 0;
    BugMediaVideoLoader *videoLoader{};
    // 开始执行操作时的时间（毫秒）
    int64_t startTimeMs = -1;
    int64_t videoPst{};
    int64_t delay{};

public:
    BugMediaVideoRenderer(BugMediaVideoLoader *loader);

    ~BugMediaVideoRenderer();

    void play();

    void pause();

    void stop();

//    typedef int (*GetVideoFrameCallback)(uint8_t* data,int *width,int *height,void *ctx);
//
//    GetVideoFrameCallback getFrameData{};

};


#endif //SLOWER_BUGMEDIAVIDEORENDERER_H
