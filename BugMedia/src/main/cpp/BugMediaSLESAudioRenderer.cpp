//
// Created by Gshine on 2021/2/7.
//

#include "BugMediaSLESAudioRenderer.h"

void BugMediaSLESAudioRenderer::play() {
    currentState = PLAYING;
}

void BugMediaSLESAudioRenderer::pause() {
    currentState = PAUSE;
}

void BugMediaSLESAudioRenderer::stop() {
    currentState = STOP;
}

BugMediaSLESAudioRenderer::BugMediaSLESAudioRenderer(BugMediaVideoLoader *loader) {
    this->videoLoader = loader;

    initSwrContext();
    initOutputBuffer();

}

void BugMediaSLESAudioRenderer::initOutputBuffer() {// 根据输入的采样数和采样率计算出重采样的个数
// 目标采样个数 = 原采样个数 *（目标采样率 / 原采样率）
    resampleCount = (int) av_rescale_rnd(SAMPLES_COUNT, getSampleRate(videoLoader->getInAudioSampleRate()),
                                         videoLoader->getInAudioSampleRate(), AV_ROUND_UP);
    // 重采样后一帧数据的大小
    resampleSize = (size_t) av_samples_get_buffer_size(
            nullptr, CHANNEL_COUNTS,
            resampleCount, getSampleFmt(), 1);

    outputBuffer[0] = (uint8_t *) malloc(resampleSize);
//    outputBuffer[0] = (uint8_t *) malloc(resampleSize / 2);
//    outputBuffer[1] = (uint8_t *) malloc(resampleSize / 2);

}

void BugMediaSLESAudioRenderer::initSwrContext() {
    swrContext = swr_alloc();
    av_opt_set_int(swrContext, "in_channel_layout", videoLoader->getInAudioChannelLayout(), 0);
    av_opt_set_int(swrContext, "out_channel_layout", CHANNEL_LAYOUT, 0);
    av_opt_set_int(swrContext, "in_sample_rate", videoLoader->getInAudioSampleRate(), 0);
    av_opt_set_int(swrContext, "out_sample_rate", getSampleRate(videoLoader->getInAudioSampleRate()), 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", videoLoader->getInAudioSampleFormat(), 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", getSampleFmt(), 0);
    swr_init(swrContext);
}

BugMediaSLESAudioRenderer::~BugMediaSLESAudioRenderer() {
    if (outputBuffer[0] != nullptr) {
        free(outputBuffer[0]);
        outputBuffer[0] = nullptr;
    }
    pthread_join(renderThread, nullptr);

}

void BugMediaSLESAudioRenderer::render() {
    pthread_create(&renderThread, nullptr, renderRoutine, this);

}

bool BugMediaSLESAudioRenderer::renderOnce() {
    return false;
}

AVSampleFormat BugMediaSLESAudioRenderer::getSampleFmt() {
    return AV_SAMPLE_FMT_S16;
}

int BugMediaSLESAudioRenderer::getSampleRate(int inSampleRate) {
    return SAMPLE_RATE;
}

void *BugMediaSLESAudioRenderer::renderRoutine(void *ctx) {
    auto render = (BugMediaSLESAudioRenderer *) ctx;
    render->doRender();
    return nullptr;
}

void BugMediaSLESAudioRenderer::doRender() {
    while (true) {

        if (currentState == PLAYING) {
            if (startTimeMs == -1) {
                // 初次执行，记录时间点，后面的播放都依赖于这个起始时间点。
                // pts是基于这个时间点的消逝时间，即偏移
                startTimeMs = getCurMsTime();
            }

            if (renderOnce()) {
                break;
            }

        } else if (currentState == PAUSE) {
            // 将播放起始时间点向前移即可
            startTimeMs = startTimeMs + pst;
            //startTimeMs = getCurMsTime()-pst;

        } else if (currentState == STOP) {
            break;
        }
    }
}
