//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAPLAYER_H
#define SLOWER_BUGMEDIAPLAYER_H

extern "C" {
#include "include/ffmpeg/libavutil/time.h"
}


#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include "BugMediaSLESAudioRenderer.h"
#include "BugMediaGLESVideoRenderer.h"
#include "BugMediaFFmpegDecoder.h"
#include <queue>
#include "BugMediaVideoRenderer.h"
#include <android/native_window_jni.h>

using namespace std;


class BugMediaPlayer {
    int maxBufferSize;
    const char *url = nullptr;
    bool isRelease = false;
    bool isAudioEnd = false;
    bool isVideoEnd = false;
    BugMediaFFmpegDecoder *audioDecoder{};
    BugMediaFFmpegDecoder *videoDecoder{};
    int64_t audioPts{};
    BugMediaSLESAudioRenderer *audioRenderer{};
    //BugMediaGLESVideoRenderer *videoRenderer{};
    BugMediaVideoRenderer *videoRenderer{};
    int duration{};
    JNIEnv *env{}; // 需要在线程内获取
    jobject surface{};
    EGLint width{};
    EGLint height{};
    bool loaded = false;
    JavaVM *javaVm{};
    ANativeWindow *nativeWindow{};


    static BugMediaAudioFrame *getAudioFrameCallback(void *ctx);

    static BugMediaVideoFrame *getVideoFrameCallback(void *ctx);

    static int64_t getAudioPtsCallback(void *ctx);

    void release();



public:
    static const int DEFAULT_BUFFER_SIZE = 10;

    BugMediaPlayer(const char *url, int decoderBufferSize, JNIEnv *env,
                   jobject surface, EGLint width, EGLint height);

    BugMediaPlayer(const char *url, int bufferSize, JNIEnv *env, jobject surface);

    ~BugMediaPlayer();

    BugMediaAudioFrame *getAudioFrame();

    BugMediaVideoFrame *getVideoFrame();


    int64_t getAudioPts() const;


    void switchAudioChannel(int ch);

    void switchVideoChannel(int ch);

    void setBufferSize(int size);

    void load();

    void play();

    void pause();

    void stop();

    void destroy();


    void resizeView(GLint x, GLint y, GLsizei width, GLsizei height);

};


#endif //SLOWER_BUGMEDIAPLAYER_H
