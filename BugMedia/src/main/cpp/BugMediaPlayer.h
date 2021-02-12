//
// Created by Gshine on 2021/2/12.
//

#ifndef SLOWER_BUGMEDIAPLAYER_H
#define SLOWER_BUGMEDIAPLAYER_H
#include <jni.h>
#include "BugMediaSLESAudioRenderer.h"
#include "BugMediaGLESVideoRenderer.h"
#include "BugMediaVideoLoader.h"

extern "C"{
#include <EGL/eglplatform.h>
}


class BugMediaPlayer {
    BugMediaSLESAudioRenderer *audioRenderer{};
    BugMediaGLESVideoRenderer *videoRenderer{};
    BugMediaVideoLoader *loader{};

    static BugMediaAudioFrame * getAudioFrameCallback(void *ctx);
    BugMediaAudioFrame *getAudioFrame();
    static BugMediaVideoFrame *getVideoFrameCallback(void *ctx);
    BugMediaVideoFrame *getVideoFrame();
public:
    ~BugMediaPlayer();
    BugMediaPlayer(const char *url, JNIEnv *env, jobject surface, EGLint width, EGLint height,
                   bool createPBufferSurface);
    void load();
    void play();
    void pause();
    void stop();
    void resizeView(GLint x, GLint y, GLsizei width, GLsizei height);


};


#endif //SLOWER_BUGMEDIAPLAYER_H
