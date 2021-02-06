//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaAudioDecoder.h"

BugMediaAudioFrame *BugMediaAudioDecoder::getFrame() {

    auto *frame = new BugMediaAudioFrame;
    int readRet = av_read_frame(avFormatContext, avPacket);
    if (readRet != 0) {
        frame->isEnd = true;
        LOGE("av_read_frame 失败");
        return frame;
    }
    int packetRet = -1;

    while (readRet == 0) {
        packetRet = avcodec_send_packet(avCodecContext, avPacket);
        if (packetRet != 0) {
            if (AVERROR_EOF == packetRet) {
                frame->isEnd = true;
                return frame;
            } else {
                LOGE("avcodec_send_packet 失败");
            }
        }

        if (avcodec_receive_frame(avCodecContext, avFrame) == 0) {

            frame->channels = avFrame->channels;
            frame->format = avFrame->format;
            frame->position = avFrame->pkt_pos;
            frame->sampleRate = avFrame->sample_rate;
            frame->pts = avFrame->pts;
            frame->data = avFrame->data;

            av_packet_unref(avPacket);
            av_frame_unref(avFrame);
            return frame;
        } else {
            av_packet_unref(avPacket);
            av_frame_unref(avFrame);
            readRet = av_read_frame(avFormatContext, avPacket);
        }
    }

    return frame;
}


BugMediaAudioDecoder::BugMediaAudioDecoder(AVFormatContext *formatContext, int trackIdx) : BugMediaBaseDecoder(
        formatContext, trackIdx) {

}
