//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaFFmpegBaseDecoder.h"


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


int32_t BugMediaFFmpegBaseDecoder::getDuration() const {
    return durationSecond;
}

BugMediaFFmpegBaseDecoder::BugMediaFFmpegBaseDecoder(AVFormatContext *formatContext, int trackIdx) {
    avFormatContext = formatContext;
    trackIndex = trackIdx;


}

void BugMediaFFmpegBaseDecoder::openDecoder() {
    AVStream *stream = avFormatContext->streams[trackIndex];

    // AV_TIME_BASE=1/1000000秒，即1微秒，stream.duration以AV_TIME_BASE为单位
    // 除以这个单位换算成秒，stream->duration / AV_TIME_BASE，下面的转换也可以
    durationSecond = stream->duration * av_q2d(stream->time_base);
    codecType = stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO ? "audio" : "video";
    avCodec = avcodec_find_decoder(stream->codecpar->codec_id);
    avCodecContext = avcodec_alloc_context3(avCodec);
    if (avcodec_open2(avCodecContext, avCodec, nullptr) != 0) {
        LOGE("打开%s解码器失败", codecType);
        return;
    }

    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();
}



