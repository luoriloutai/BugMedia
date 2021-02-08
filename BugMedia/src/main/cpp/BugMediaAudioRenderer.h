//
// Created by Gshine on 2021/2/7.
//

#ifndef SLOWER_BUGMEDIAAUDIORENDERER_H
#define SLOWER_BUGMEDIAAUDIORENDERER_H

extern "C" {
#include "include/ffmpeg/libavutil/opt.h"
}

#include "BugMediaVideoLoader.h"

class BugMediaAudioRenderer {
    State currentState = UNSTART;
    BugMediaVideoLoader *videoLoader{};

    // 格式转换上下文
    SwrContext *swrContext{};
    // 输出缓冲
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

    AVSampleFormat getSampleFmt() {
        return AV_SAMPLE_FMT_S16;
    }


    int getSampleRate(int spr) {
        return SAMPLE_RATE;
    }
public:
    void play();

    void pause();

    void stop();

    BugMediaAudioRenderer(BugMediaVideoLoader *loader);
    ~BugMediaAudioRenderer();
};


#endif //SLOWER_BUGMEDIAAUDIORENDERER_H
