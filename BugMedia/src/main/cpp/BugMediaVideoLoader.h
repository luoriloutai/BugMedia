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

#include "BugMediaAudioFrameQueue.h"
#include "BugMediaVideoFrameQueue.h"
#include <thread>
#include <pthread.h>
#include "BugMediaVideoInfo.h"
#include "BugMediaAudioInfo.h"


class BugMediaVideoLoader {
    int maxBufferSize;
    int minBufferSize;
    BugMediaAudioFrameQueue *audioFrameQueue = nullptr;
    BugMediaVideoFrameQueue *videoFrameQueue = nullptr;
    BugMediaVideoInfo *videoInfo{};
    BugMediaAudioInfo *audioInfo{};

    //std::thread *decodeThread;
    const char *url = nullptr;
    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;
    pthread_mutex_t mutex{};
    pthread_cond_t cond{};

    void initDecoder();

    void init();

protected:
    virtual void release();

public:

    BugMediaVideoLoader(const char *url);

    ~BugMediaVideoLoader();

    BugMediaAudioFrame *getNextAudioFrame();

    BugMediaVideoFrame *getNextVideoFrame();

};


#endif //SLOWER_BUGMEDIAVIDEOLOADER_H
