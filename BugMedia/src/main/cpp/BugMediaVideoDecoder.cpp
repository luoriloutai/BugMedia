//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaVideoDecoder.h"

BugMediaVideoFrame *BugMediaVideoDecoder::getFrame() {

    int readRet = av_read_frame(avFormatContext,avPacket);
    if (readRet!=0){
        LOGE("av_read_frame 失败");
        return nullptr;
    }
    int packetRet =-1;

    while (readRet==0){
        packetRet = avcodec_send_packet(avCodecContext, avPacket);
        if (packetRet!=0){
            if (AVERROR_EOF == packetRet){
                isEnd = true;
                return nullptr;
            }else{
                LOGE("avcodec_send_packet 失败");
                return nullptr;
            }
        }

        if (avcodec_receive_frame(avCodecContext, avFrame) == 0) {
            auto *frame = new BugMediaVideoFrame;
            frame->isKeyframe = avFrame->key_frame == 1;
            frame->pts = avFrame->pts;
            frame->data = avFrame->data;
            frame->isInterlaced = avFrame->interlaced_frame == 1;
            av_packet_unref(avPacket);
            av_frame_unref(avFrame);
            return frame;
        }else{
            // 据说取帧返回错误码要继续读取下一个包，不然会丢帧
            // 所以这里用了个循环。我推测这是因为B帧的问题，
            // 取到它们之后不能立马解出来，需要后面的帧先解出来
            av_packet_unref(avPacket);
            readRet = av_read_frame(avFormatContext,avPacket);
        }
    }

    return nullptr;
}

BugMediaVideoDecoder::BugMediaVideoDecoder() {

}

BugMediaVideoDecoder::~BugMediaVideoDecoder() {

}

bool BugMediaVideoDecoder::end() const {
    return isEnd;
}





