//
// Created by Gshine on 2021/1/31.
//

#ifndef SLOWER_BUGMEDIAGLESVIDEORENDERER_H
#define SLOWER_BUGMEDIAGLESVIDEORENDERER_H


#include "openGL/BugMediaBaseRenderer.h"
#include "BugMediaVideoLoader.h"
#include "interfaces/BugMediaRenderer.h"


class BugMediaGLESVideoRenderer :virtual protected BugMediaBaseRenderer, public BugMediaRenderer {
    void setShaderSource();

    void onRender();

    void prepare();

    //渲染并返回是否结束条件
    bool renderOnce();

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
    GLuint texId{}; // 纹理id

public:
    BugMediaGLESVideoRenderer(BugMediaVideoLoader *loader);

    ~BugMediaGLESVideoRenderer();

    void play();

    void pause();

    void stop();

    void render();

//    typedef int (*GetVideoFrameCallback)(uint8_t* data,int *width,int *height,void *ctx);
//
//    GetVideoFrameCallback getFrameData{};

};


#endif //SLOWER_BUGMEDIAGLESVIDEORENDERER_H
