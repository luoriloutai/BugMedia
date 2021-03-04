//
// Created by Gshine on 2021/3/3.
//

#ifndef SLOWER_BUGMEDIAVIDEORENDERER_H
#define SLOWER_BUGMEDIAVIDEORENDERER_H

#include <semaphore.h>
#include "OpenGLES/BugMediaGLES.h"
#include "BugMediaCommon.h"
#include "BugMediaVideoFrame.h"

extern "C" {
#include "include/ffmpeg/libavutil/time.h"
}

class BugMediaVideoRenderer : public virtual BugMediaGLES {
    sem_t playSem{};
    State currentState = UNSTART;
    // 开始执行操作时的时间（毫秒）
    int64_t startTimeMs = -1;
    int64_t videoPts{};
    int64_t audioPts{};
    int64_t delay{};
    GLuint texture{};

    const GLchar *getVertexShaderSource();

    const GLchar *getFragmentShaderSource();

    void onRender();

    // 渲染一次，如果失败返回GL_FALSE，否则返回GL_TRUE
    GLboolean renderOnce();

    void prepare();


public:
    typedef BugMediaVideoFrame *(*GetVideoFrameCallback)(void *ctx);

    typedef int64_t (*GetAudioPtsCallback)(void *ctx);

    BugMediaVideoRenderer(ANativeWindow *window,GetVideoFrameCallback getVideoFrameCallback,
                          GetAudioPtsCallback getAudioPtsCallback, void *callbackContext);

    ~BugMediaVideoRenderer();

    void play();

    void pause();

    void stop();

private:
    GetVideoFrameCallback getVideoFrame{};
    GetAudioPtsCallback getAudioPts{};
    void *callbackContext{};

};


#endif //SLOWER_BUGMEDIAVIDEORENDERER_H
