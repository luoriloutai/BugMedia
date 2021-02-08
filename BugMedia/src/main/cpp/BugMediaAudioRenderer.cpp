//
// Created by Gshine on 2021/2/7.
//

#include "BugMediaAudioRenderer.h"

void BugMediaAudioRenderer::play() {
    currentState = PLAYING;
}

void BugMediaAudioRenderer::pause() {
    currentState = PAUSE;
}

void BugMediaAudioRenderer::stop() {
    currentState = STOP;
}

BugMediaAudioRenderer::BugMediaAudioRenderer(BugMediaVideoLoader *loader) {
    this->videoLoader = loader;

    swrContext = swr_alloc();
    av_opt_set_int(swrContext, "in_channel_layout",videoLoader->getInAudioChannelLayout() , 0);
    av_opt_set_int(swrContext, "out_channel_layout", CHANNEL_LAYOUT, 0);
    av_opt_set_int(swrContext, "in_sample_rate", videoLoader->getInAudioSampleRate(), 0);
    av_opt_set_int(swrContext, "out_sample_rate", getSampleRate(videoLoader->getInAudioSampleRate()), 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", videoLoader->getInAudioSampleFormat() , 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", getSampleFmt(), 0);
    swr_init(swrContext);

    // 根据输入的采样数和采样率计算出重采样的个数
    // 目标采样个数 = 原采样个数 *（目标采样率 / 原采样率）
    resampleCount = (int)av_rescale_rnd(SAMPLES_COUNT, getSampleRate(videoLoader->getInAudioSampleRate()),
                                        videoLoader->getInAudioSampleRate(), AV_ROUND_UP);
    // 重采样后一帧数据的大小
    resampleSize = (size_t)av_samples_get_buffer_size(
            nullptr, CHANNEL_COUNTS,
            resampleCount, getSampleFmt(), 1);


    outputBuffer[0] = (uint8_t *) malloc(resampleSize / 2);
    outputBuffer[1] = (uint8_t *) malloc(resampleSize / 2);

}

BugMediaAudioRenderer::~BugMediaAudioRenderer() {

}
