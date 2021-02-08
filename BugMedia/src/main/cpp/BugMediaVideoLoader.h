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
#include <vector>

using namespace std;


class BugMediaVideoLoader {
    int maxBufferSize;
    const char *url = nullptr;
    AVFormatContext *formatContext = nullptr;
    pthread_t initThread{};
    bool isRelease = false;
    bool isAudioEnd = false;
    bool isVideoEnd = false;
    vector<BugMediaAudioDecoder *> audioDecoders{};
    vector<BugMediaVideoDecoder *> videoDecoders{};
    int audioTrackCount{};
    int videoTrackCount{};
    BugMediaAudioDecoder *currentAudioDecoder{};
    BugMediaVideoDecoder *currentVideoDecoder{};
    int64_t audioPts{};

    static void *initThreadFunc(void *pVoid);

    // 定义该函数是目的是为了在线程中直接使用类内的方法和变量，
    // 而不需要用传过去的本类的指针去访问，简化写法而已。
    void init();

public:

    void release();

    BugMediaVideoLoader(const char *url, int bufferSize = 100);

    ~BugMediaVideoLoader();

    BugMediaAudioFrame *getAudioFrame();

    BugMediaVideoFrame *getVideoFrame();

    int64_t getAudioPts() const;

    int getAudioTrackCount() const;

    int getVideoTrackCount() const;

    void switchAudioChannel(int ch);

    void switchVideoChannel(int ch);

    void setBufferSize(int size);

    void load();

    // 获取输入音频的声道格式
    uint64_t getInAudioChannelLayout();

    // 获取输入音频的采样率
    int getInAudioSampleRate();

    // 获取输入音频的采样格式
    AVSampleFormat getInAudioSampleFormat();

};


#endif //SLOWER_BUGMEDIAVIDEOLOADER_H
