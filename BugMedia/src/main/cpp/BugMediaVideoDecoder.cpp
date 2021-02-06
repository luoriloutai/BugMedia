//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaVideoDecoder.h"

BugMediaVideoFrame *BugMediaVideoDecoder::getFrame() {
    auto *frame = new BugMediaVideoFrame;
    int readRet = av_read_frame(avFormatContext,avPacket);
    if (readRet!=0){
        frame->isEnd= true;
        LOGE("av_read_frame 失败");
        return frame;
    }
    int packetRet =-1;

    while (readRet==0){
        packetRet = avcodec_send_packet(avCodecContext, avPacket);
        if (packetRet!=0){
            if (AVERROR_EOF == packetRet){
                frame->isEnd= true;
                return frame;
            }else{
                LOGE("avcodec_send_packet 失败");
            }
        }

        if (avcodec_receive_frame(avCodecContext, avFrame) == 0) {

            frame->isKeyframe = avFrame->key_frame == 1;
            frame->pts = avFrame->pts;
            frame->data = avFrame->data;
            frame->isInterlaced = avFrame->interlaced_frame == 1;
            frame->position=avFrame->pkt_pos;
            frame->format=avFrame->format;
            frame->width=avFrame->width;
            frame->height=avFrame->height;

            av_packet_unref(avPacket);
            av_frame_unref(avFrame);
            return frame;
        }else{
            // 据说返回错误要继续读取下一个包，不然会丢帧
            // 所以这里用了个循环。我推测这是因为B帧的问题，
            // 取到它们之后不能立马解出来，需要后面的帧先解出来
            av_packet_unref(avPacket);
            av_frame_unref(avFrame);
            readRet = av_read_frame(avFormatContext,avPacket);
        }
    }

    return frame;
}

BugMediaVideoDecoder::BugMediaVideoDecoder(AVFormatContext *formatContext,int trackIdx):BugMediaBaseDecoder(formatContext,trackIdx) {

}

BugMediaVideoDecoder::~BugMediaVideoDecoder() {

}







