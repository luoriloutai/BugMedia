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
#include <pthread.h>
#include "BugMediaVideoDecoder.h"
#include "BugMediaAudioDecoder.h"
#include <semaphore.h>
#include "BugMediaStateChangedCallback.h"


class BugMediaVideoLoader {
    int maxBufferSize;
    BugMediaAudioFrameQueue *audioFrameQueue = nullptr;
    BugMediaVideoFrameQueue *videoFrameQueue = nullptr;
    BugMediaVideoDecoder *videoDecoder{};
    BugMediaAudioDecoder *audioDecoder{};
    const char *url = nullptr;
    AVFormatContext *formatContext = nullptr;
    sem_t bufferReadable{};
    sem_t bufferWritable{};
    pthread_t worker{};
    bool isRelease = false;
    bool isEnd = false;


    void initDecoder();

    static void *threadWorking(void *pVoid);

    // 定义该函数是目的是为了在线程中直接使用类内的方法和变量，
    // 而不需要用传过去的本类的指针去访问，简化写法而已。
    void doWork();


public:
    BugMediaStateChangedCallback *stateChangedCallback{};

    void release();

    BugMediaVideoLoader(const char *url,int bufferSize);

    ~BugMediaVideoLoader();

    BugMediaAudioFrame *getNextAudioFrame();



};


#endif //SLOWER_BUGMEDIAVIDEOLOADER_H
