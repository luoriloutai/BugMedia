//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaBaseDecoder.h"

int BugMediaBaseDecoder::getTrackCount() {
    return trackIndices.size();
}

void BugMediaBaseDecoder::addTrackIndex(int32_t i) {
    trackIndices.push_back(i);
}

std::vector<int32_t> BugMediaBaseDecoder::getTrackIndices() const {
    return trackIndices;
}

int BugMediaBaseDecoder::getFirstTrackIndex() {
    if (trackIndices.empty()) {
        return -1;
    }
    return trackIndices[0];
}

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


    delete avCodecPar;
    delete avCodec;

}


bool BugMediaBaseDecoder::init(AVFormatContext *formatContext,AVCodecParameters *parameters, int32_t duration) {
    if (avFormatContext== nullptr){
        avFormatContext=formatContext;
    }
    if (this->durationSecond == 0) {
        this->durationSecond = duration;
    }
    if (avCodecPar == nullptr) {
        avCodecPar = parameters;
    }
    if (avCodec == nullptr) {
        avCodec = avcodec_find_decoder(avCodecPar->codec_id);
    }
    if (avCodecContext == nullptr) {
        avCodecContext = avcodec_alloc_context3(avCodec);
        if (avcodec_open2(avCodecContext, avCodec, nullptr) != 0) {
            LOGE("打开解码器失败");
            return false;
        }
    }


    return true;
}

long BugMediaBaseDecoder::getDuration() const {
    return durationSecond;
}

BugMediaBaseDecoder::BugMediaBaseDecoder() {
    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();
}



