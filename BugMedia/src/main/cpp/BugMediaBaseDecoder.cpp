//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaBaseDecoder.h"


BugMediaBaseDecoder::~BugMediaBaseDecoder() {

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


int32_t BugMediaBaseDecoder::getDuration() const {
    return durationSecond;
}

BugMediaBaseDecoder::BugMediaBaseDecoder(AVFormatContext *formatContext, int trackIdx) {
    avFormatContext = formatContext;
    trackIndex = trackIdx;

    // AV_TIME_BASE=1/1000000秒，即1微秒
    AVStream * stream = formatContext->streams[trackIdx];
    durationSecond = stream->duration/AV_TIME_BASE;
    avCodec = avcodec_find_decoder(stream->codecpar->codec_id);
    avCodecContext = avcodec_alloc_context3(avCodec);
    if (avcodec_open2(avCodecContext, avCodec, nullptr) != 0) {
        throw "打开解码器失败";
    }

    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();
}



