#include <alloca.h>
//
// Created by Gshine on 2021/2/4.
//

#define DEBUGAPP

#include "BugMediaFFmpegAudioDecoder.h"

extern "C" {
#include "include/ffmpeg/libavutil/opt.h"
}


BugMediaFFmpegAudioDecoder::BugMediaFFmpegAudioDecoder(AVFormatContext *formatContext, int trackIdx, int bufferSize) :
        BugMediaFFmpegBaseDecoder(formatContext, trackIdx) {
    this->bufferSize = bufferSize;

#ifdef DEBUGAPP
    LOGD("音频采样率：%d", avCodecContext->sample_rate);
    LOGD("音频采样格式：%d", avCodecContext->sample_fmt);
#endif

    initSwrContext();
    initOutputBuffer();
    sem_init(&this->canFillData, 0, this->bufferSize);
    sem_init(&this->canTakeData, 0, 0);

}

void BugMediaFFmpegAudioDecoder::decode() {

#ifdef DEBUGAPP
    LOGD("开始解码音频");
#endif

    int ret;

    while (true) {

        sem_wait(&this->canFillData);
        if (quit) {
            return;
        }

        ret = av_read_frame(avFormatContext, avPacket);
        if (ret != 0) {

            // 流结束或者出错
            auto aFrame = new BugMediaAudioFrame();
            aFrame->isEnd = true;
            frameQueue.push(aFrame);
            sem_post(&this->canTakeData);

            return;
        }

        // 读取的包视频和音频混合在一起，需要判断选取
        if (avPacket->stream_index != trackIndex) {
            sem_post(&canFillData);
            continue;
        }

        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            if (AVERROR_EOF == ret) {
                // 正常结束
                //
                // 结束的时候也往队列放一个帧，将其结束标志设置上，
                // 让帧来作为判断结束的依据

#ifdef DEBUGAPP
                LOGD("音频解码结束");
#endif

                auto aFrame = new BugMediaAudioFrame();
                aFrame->isEnd = true;
                frameQueue.push(aFrame);
                sem_post(&this->canTakeData);
                return;
            }
        }

        // 该方法执行不返回0需要再次从头开始执行读取帧，直至成功返回0表示取到了一帧
        ret = avcodec_receive_frame(avCodecContext, avFrame);
#ifdef DEBUGAPP
        LOGD("avcodec_receive_frame执行结果：%d ,%s", ret, av_err2str(ret));
#endif
        if (ret == AVERROR_EOF) {
            auto aFrame = new BugMediaAudioFrame();
            aFrame->isEnd = true;
            frameQueue.push(aFrame);
            sem_post(&this->canTakeData);

            return;

        }

        if (ret == 0) {
            auto *aFrame = new BugMediaAudioFrame();
            aFrame->channels = avFrame->channels;
            aFrame->format = avFrame->format;
            aFrame->position = avFrame->pkt_pos;
            aFrame->sampleRate = avFrame->sample_rate;
            aFrame->sampleCount = avFrame->nb_samples;



            // avFrame->pts是以stream.time_base为单位的时间戳，单位为秒
            // time_base不是一个数，是一个AVRational结构，可用av_q2d()转换成double,
            // 这个结构本质上是一个分子和一个分母表示的分数，av_q2d()就是用分子除以
            // 分母得出的数。
            // pts*av_q2d(time_base)就是这个值的最终表示，单位为秒
            aFrame->pts = avFrame->pts * av_q2d(avFormatContext->streams[trackIndex]->time_base) * 1000;

            aFrame->resampleSize = resampleSize;

            // 音频重采样转换
            int sampleCount = swr_convert(swrContext, outputBuffer, resampleSize,
                                   (const uint8_t **) (avFrame->data), avFrame->nb_samples);
#ifdef DEBUGAPP

            LOGD("音频帧转换，每通道样本数：%d,%s", sampleCount, av_err2str(sampleCount));


#endif

            if (sampleCount > 0) {

                // 将缓冲中的数据复制到新开辟的空间，并入队
                auto *data = (uint8_t *) malloc(resampleSize);
                memcpy(data, outputBuffer[0], resampleSize);
                aFrame->data = data;
                frameQueue.push(aFrame);
                sem_post(&this->canTakeData);

#ifdef DEBUGAPP
static int couter=0;
                LOGD("第%d帧解码完毕",++couter);
                LOGD("帧大小：%d",sizeof(data));
                LOGD("队列大小：%d", frameQueue.size());

#endif


            }
        }else{
            // 这一次做的事情没有成功，返还信号
            sem_post(&canFillData);
        }


        // 解除对包的引用，以便下次再用
        av_packet_unref(avPacket);

    }  // 最外层while


}


void *BugMediaFFmpegAudioDecoder::decodeRoutine(void *ctx) {
    auto self = (BugMediaFFmpegAudioDecoder *) ctx;
    self->decode();
    return nullptr;
}

BugMediaFFmpegAudioDecoder::~BugMediaFFmpegAudioDecoder() {
    quit = true;
    sem_post(&canTakeData);
    sem_post(&canFillData);

    if (outputBuffer[0] != nullptr) {
        free(outputBuffer[0]);
        outputBuffer[0] = nullptr;
    }

    if (swrContext != nullptr) {
        swr_free(&swrContext);
    }

    while (!frameQueue.empty()) {
        BugMediaAudioFrame *frame = frameQueue.front();
        delete frame;
        frameQueue.pop();
    }

    void *retval;
    sem_destroy(&this->canFillData);
    sem_destroy(&this->canTakeData);
    pthread_join(decodeThread, &retval);
}


BugMediaAudioFrame *BugMediaFFmpegAudioDecoder::getFrame() {

    sem_wait(&this->canTakeData);
    if (quit) {
        return nullptr;
    }
    BugMediaAudioFrame *frame = frameQueue.front();
    frameQueue.pop();
    if (!frame->isEnd) {
        sem_post(&this->canFillData);
    }


    return frame;
}

void BugMediaFFmpegAudioDecoder::startDecode() {

    this->decodeThread = pthread_create(&decodeThread, nullptr, decodeRoutine, this);
}

void BugMediaFFmpegAudioDecoder::initSwrContext() {

    swrContext = swr_alloc();
    av_opt_set_int(swrContext, "in_channel_layout", avCodecContext->channel_layout, 0);
    av_opt_set_int(swrContext, "out_channel_layout", CHANNEL_LAYOUT, 0);
    av_opt_set_int(swrContext, "in_sample_rate", avCodecContext->sample_rate, 0);
    av_opt_set_int(swrContext, "out_sample_rate", getSampleRate(avCodecContext->sample_rate), 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", avCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", getSampleFmt(), 0);
    swr_init(swrContext);
}

void BugMediaFFmpegAudioDecoder::initOutputBuffer() {

    // 根据输入的采样数和采样率计算出重采样的个数
    // 目标采样个数 = 原采样个数 *（目标采样率 / 原采样率）
    resampleCount = (int) av_rescale_rnd(SAMPLES_COUNT, getSampleRate(avCodecContext->sample_rate),
                                         avCodecContext->sample_rate, AV_ROUND_UP);
    // 重采样后一帧数据的大小
    resampleSize = (size_t) av_samples_get_buffer_size(
            nullptr, CHANNEL_COUNTS,
            resampleCount, getSampleFmt(), 1);

    outputBuffer[0] = (uint8_t *) malloc(resampleSize);
//    outputBuffer[0] = (uint8_t *) malloc(resampleSize / 2);
//    outputBuffer[1] = (uint8_t *) malloc(resampleSize / 2);

#ifdef DEBUGAPP
    LOGD("重采样数：%d，重采样大小：%d", resampleCount, resampleSize);
#endif

}

int BugMediaFFmpegAudioDecoder::getSampleRate(int inSampleRate) {
    return SAMPLE_RATE;
}

AVSampleFormat BugMediaFFmpegAudioDecoder::getSampleFmt() {
    return AV_SAMPLE_FMT_S16;
}
