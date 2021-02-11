#include <alloca.h>
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

extern "C"{
#include <SLES/OpenSLES.h>
};



using namespace std;


class BugMediaFFmpegAudioDecoder : virtual public BugMediaFFmpegBaseDecoder {
    queue<BugMediaAudioFrame *> frameQueue{};
    int bufferSize{};
    pthread_t decodeThread{};
    sem_t canFillData{};
    sem_t canTakeData{};
    bool quit= false;

    // 解码并入队
    void decode();

    // 解码线程运行的函数，由于是静态的，需要将本类对象传入以访问类对象的成员
    // 由于在该方法内访问对象成员需要用本对象的指针，写起来麻烦，于是又定义了
    // decode()方法，这样只需要在这个函数里将ctx转成本类对象指针后调用一次decode()
    static void *decodeRoutine(void *ctx);

    // 音频重采样上下文
    SwrContext *swrContext{};
    // 格式转换输出缓冲
    uint8_t *outputBuffer[2] = {};

    // 重采样个数
    // acc默认为1024，重采样后可能会变化
    int resampleCount = 1024;

    // 重采样以后，一帧数据的大小
    size_t resampleSize = 0;

    // 编码格式：浮点型数据（32位）
    static const AVSampleFormat ENCODE_FORMAT = AV_SAMPLE_FMT_FLTP;
    // 采样率
    static const int SAMPLE_RATE = 44100;
    // 声道数
    static const int CHANNEL_COUNTS = 2;
    // 声道格式:立体声（双声道）
    static const uint64_t CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
    // 比特率
    static const int BIT_RATE = 64000;
    // ACC音频一帧采样数
    static const int SAMPLES_COUNT = 1024;

    int getSampleRate(int inSampleRate);

    AVSampleFormat getSampleFmt();

    void initOutputBuffer();

public:
    BugMediaAudioFrame *getFrame();

    BugMediaFFmpegAudioDecoder(AVFormatContext *formatContext, int trackIdx, int bufferSize = 50);

    ~BugMediaFFmpegAudioDecoder();

    void startDecode();

    void initSwrContext();

};


#endif //SLOWER_BUGMEDIAFFMPEGAUDIODECODER_H
