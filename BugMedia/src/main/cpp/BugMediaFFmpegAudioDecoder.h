//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAFFMPEGAUDIODECODER_H
#define SLOWER_BUGMEDIAFFMPEGAUDIODECODER_H

#include "BugMediaFFmpegBaseDecoder.h"
#include "BugMediaAudioFrame.h"
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include "interfaces/BugMediaDecoder.h"

using namespace std;


class BugMediaFFmpegAudioDecoder : public BugMediaFFmpegBaseDecoder, public BugMediaDecoder {
    //queue<BugMediaAudioFrame *> frameQueue{};
    queue<BugMediaAVFrame *> frameQueue{};
    int bufferSize{};
    pthread_t decodeThread{};
    sem_t canFillData{};
    sem_t canTakeData{};

    // 解码并入队
    void decode();

    // 解码线程运行的函数，由于是静态的，需要将本类对象传入以访问类对象的成员
    // 由于在该方法内访问对象成员需要用本对象的指针，写起来麻烦，于是又定义了
    // decode()方法，这样只需要在这个函数里将ctx转成本类对象指针后调用一次decode()
    static void *decodeRoutine(void *ctx);



public:
    //BugMediaAudioFrame *getFrame();

    BugMediaAVFrame *getFrame();

    // 获取输入音频的声道格式
    uint64_t getInChannelLayout();

    // 获取输入音频的采样率
    int getInSampleRate();

    // 获取输入音频的采样格式
    AVSampleFormat getInSampleFormat();

    BugMediaFFmpegAudioDecoder(AVFormatContext *formatContext, int trackIdx, int bufferSize = 100);

    ~BugMediaFFmpegAudioDecoder();

};


#endif //SLOWER_BUGMEDIAFFMPEGAUDIODECODER_H
