//
// Created by Gshine on 2021/1/31.
//

#ifndef SLOWER_BUGMEDIAGLESVIDEORENDERER_H
#define SLOWER_BUGMEDIAGLESVIDEORENDERER_H


#include "openGL/BugMediaBaseRenderer.h"
#include "BugMediaCommon.h"
#include "BugMediaVideoFrame.h"


class BugMediaGLESVideoRenderer : virtual public BugMediaBaseRenderer {
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

    // 开始执行操作时的时间（毫秒）
    int64_t startTimeMs = -1;
    int64_t videoPst{};
    int64_t delay{};
    GLuint texId{}; // 纹理id

public:

    void play();

    void pause();

    void stop();

    typedef BugMediaVideoFrame *(*GetVideoFrameCallback)(void *ctx);

    typedef int64_t (*GetAudioPtsCallback)(void * ctx);

    GetAudioPtsCallback getAudioPts{};

    BugMediaGLESVideoRenderer(GetVideoFrameCallback getVideoFrameCallback, JNIEnv *env, jobject surface, EGLint width,
                              EGLint height);

    ~BugMediaGLESVideoRenderer();
private:
    GetVideoFrameCallback getVideoFrame{};
};


#endif //SLOWER_BUGMEDIAGLESVIDEORENDERER_H
