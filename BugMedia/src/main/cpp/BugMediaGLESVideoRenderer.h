//
// Created by Gshine on 2021/1/31.
//

#ifndef SLOWER_BUGMEDIAGLESVIDEORENDERER_H
#define SLOWER_BUGMEDIAGLESVIDEORENDERER_H

extern "C" {
#include "include/ffmpeg/libavutil/time.h"
}


#include "glm/ext.hpp"
#include "openGL/BugMediaBaseRenderer.h"
#include "BugMediaCommon.h"
#include "BugMediaVideoFrame.h"
#include <semaphore.h>


class BugMediaGLESVideoRenderer : virtual public BugMediaBaseRenderer {
    void setShaderSource();

    void onRender();

    void prepare();

    //渲染并返回是否结束条件
    bool renderOnce();

    State currentState = UNSTART;
    int64_t audioPts{};
    uint8_t *data{};

    // 开始执行操作时的时间（毫秒）
    int64_t startTimeMs = -1;

    int64_t videoPts{};
    int64_t delay{};
    GLuint texId{}; // 纹理id
    JNIEnv *env{};
    jobject surface{};
    EGLint width{};
    EGLint height{};
    bool createPBufferSurface = false;
    bool rendering = false;
    sem_t playSem{};
    JavaVM *javaVm{};




public:
    typedef BugMediaVideoFrame *(*GetVideoFrameCallback)(void *ctx);

    typedef int64_t (*GetAudioPtsCallback)(void *ctx);

    BugMediaGLESVideoRenderer(GetVideoFrameCallback getVideoFrameCallback, GetAudioPtsCallback getAudioPtsCallback,
                              void *callbackContext, JNIEnv *env, jobject surface, EGLint width, EGLint height,
                              bool createPBufferSurface);


    ~BugMediaGLESVideoRenderer();

    void play();

    void pause();

    void stop();

    void render();


private:
    GetVideoFrameCallback getVideoFrame{};
    GetAudioPtsCallback getAudioPts{};
    void *callbackContext{};

};


#endif //SLOWER_BUGMEDIAGLESVIDEORENDERER_H
