//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAPLAYER_H
#define SLOWER_BUGMEDIAPLAYER_H

extern "C" {
#include "include/ffmpeg/libavcodec/avcodec.h"
#include "include/ffmpeg/libavformat/avformat.h"
#include "include/ffmpeg/libswscale/swscale.h"
#include "include/ffmpeg/libswresample/swresample.h"
#include "include/ffmpeg/libavutil/pixdesc.h"
#include "include/ffmpeg/libavutil/frame.h"
#include "include/ffmpeg/libavutil/time.h"
}


#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include "BugMediaSLESAudioRenderer.h"
#include "BugMediaFFmpegAudioDecoder.h"
#include "BugMediaFFmpegVideoDecoder.h"
#include "BugMediaGLESVideoRenderer.h"

using namespace std;


class BugMediaPlayer {
    int maxBufferSize;
    const char *url = nullptr;
    AVFormatContext *formatContext = nullptr;
    pthread_t initThread{};
    bool isRelease = false;
    bool isAudioEnd = false;
    bool isVideoEnd = false;
    vector<BugMediaFFmpegAudioDecoder *> audioDecoders{};
    vector<BugMediaFFmpegVideoDecoder *> videoDecoders{};
    int audioTrackCount{};
    int videoTrackCount{};
    BugMediaFFmpegAudioDecoder *currentAudioDecoder{};
    BugMediaFFmpegVideoDecoder *currentVideoDecoder{};
    int64_t audioPts{};
    BugMediaSLESAudioRenderer *audioRenderer{};
    BugMediaGLESVideoRenderer *videoRenderer{};
    int duration{};


    static void *initThreadFunc(void *pVoid);

    // 定义该函数是目的是为了在线程中直接使用类内的方法和变量，
    // 而不需要用传过去的本类的指针去访问，简化写法而已。
    void init();

    static BugMediaAudioFrame *getAudioFrameData(void *ctx);

    void release();

public:


    BugMediaPlayer(const char *url, int bufferSize = 50);

    ~BugMediaPlayer();

    BugMediaAudioFrame *getAudioFrame();

    BugMediaVideoFrame *getVideoFrame();


    int64_t getAudioPts() const;

    int getAudioTrackCount() const;

    int getVideoTrackCount() const;

    void switchAudioChannel(int ch);

    void switchVideoChannel(int ch);

    void setBufferSize(int size);

    void load();

    void play();

    void pause();

    void stop();

    void destroy();

    void setWindowSurface(JNIEnv *env, jobject surface);

    void setPBufferSurface(EGLint width, EGLint height);

    void resizeView(GLint x, GLint y, GLsizei width, GLsizei height);

};


#endif //SLOWER_BUGMEDIAPLAYER_H
