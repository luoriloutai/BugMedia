//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAVIDEOLOADER_H
#define SLOWER_BUGMEDIAVIDEOLOADER_H

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


class BugMediaVideoLoader {
    int decoderBufferSize;
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
    // 是否执行过加载操作，控制播放等操作
    bool isLoaded = false;

    static const int DEFAULT_BUFFER_SIZE = 10;

    static void *initThreadFunc(void *pVoid);

    // 定义该函数是目的是为了在线程中直接使用类内的方法和变量，
    // 而不需要用传过去的本类的指针去访问，简化写法而已。
    void init();

    void release();

public:


    BugMediaVideoLoader(const char *url, BugMediaSLESAudioRenderer *audioRenderer,
                        BugMediaGLESVideoRenderer *videoRenderer, int bufferSize = DEFAULT_BUFFER_SIZE);

    ~BugMediaVideoLoader();

    BugMediaAudioFrame *getAudioFrame();

    BugMediaVideoFrame *getVideoFrame();


    int64_t getAudioPts() const;

    int getAudioTrackCount() const;

    int getVideoTrackCount() const;

    void switchAudioChannel(int ch);

    void switchVideoChannel(int ch);

    void setBufferSize(int size);

    // 执行加载操作单独成一个方法是为了灵活放置调用位置（时机）
    // 该方法建立新线程运行
    void load();

    void play();

    void pause();

    void stop();

    void destroy();

    void setWindowSurface(JNIEnv *env, jobject surface);

    void setPBufferSurface(EGLint width, EGLint height);

    void resizeView(GLint x, GLint y, GLsizei width, GLsizei height);

};


#endif //SLOWER_BUGMEDIAVIDEOLOADER_H
