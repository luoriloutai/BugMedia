//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaFFmpegBaseDecoder.h"

// #define DEBUGAPP

BugMediaFFmpegBaseDecoder::~BugMediaFFmpegBaseDecoder() {

    if (avPacket != nullptr) {
        av_packet_free(&avPacket);
    }
    if (avFrame != nullptr) {
        av_frame_free(&avFrame);
    }
    if (avCodecContext != nullptr) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
    }
}


int32_t BugMediaFFmpegBaseDecoder::getStreamDuration() const {
    return streamDuration;
}

BugMediaFFmpegBaseDecoder::BugMediaFFmpegBaseDecoder(AVFormatContext *formatContext, int trackIdx) {
    avFormatContext = formatContext;
    trackIndex = trackIdx;
    openDecoder();

}

void BugMediaFFmpegBaseDecoder::openDecoder() {

    AVStream *stream = avFormatContext->streams[trackIndex];

    // 流长与总时长不一样
    streamDuration = stream->duration * av_q2d(stream->time_base);

#ifdef DEBUGAPP
    LOGD("流长度：%lld,换算后长度：%d",stream->duration,streamDuration);
#endif

    codecType = stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO ? "audio" : "video";
    avCodec = avcodec_find_decoder(stream->codecpar->codec_id);
    avCodecContext = avcodec_alloc_context3(avCodec);
    // 向avCondecContext填充参数
    if (avcodec_parameters_to_context(avCodecContext, stream->codecpar) != 0) {
        LOGE("获取参数失败");
        return;
    }
    if (avcodec_open2(avCodecContext, avCodec, nullptr) != 0) {
        LOGE("打开%s解码器失败", codecType);
        return;
    }

#ifdef DEBUGAPP

    LOGD("sample_rate: %d,trackIndex: %d,codecType:%s,sample_format:%d",
            avCodecContext->sample_rate,trackIndex,codecType,avCodecContext->sample_fmt);
#endif

    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();

}



