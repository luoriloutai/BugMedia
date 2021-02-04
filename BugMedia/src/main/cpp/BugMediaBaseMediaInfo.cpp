//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaBaseMediaInfo.h"

int BugMediaBaseMediaInfo::getTrackCount() {
    return trackIndices.size();
}

void BugMediaBaseMediaInfo::addTrackIndex(int32_t i) {
    trackIndices.push_back(i);
}

std::vector<int32_t> BugMediaBaseMediaInfo::getTrackIndices() const {
    return trackIndices;
}

int BugMediaBaseMediaInfo::getFirstTrackIndex() {
    return trackIndices[0];
}

BugMediaBaseMediaInfo::~BugMediaBaseMediaInfo() {
    delete codecPar;
    delete codec;
    delete codecContext;
}


bool BugMediaBaseMediaInfo::init(AVCodecParameters *parameters,int32_t duration) {
    if (this->durationSecond==0){
        this->durationSecond=duration;
    }
    if (codecPar == nullptr) {
        codecPar = parameters;
    }
    if (codec == nullptr) {
        codec = avcodec_find_decoder(codecPar->codec_id);
    }
    if (codecContext == nullptr) {
        codecContext = avcodec_alloc_context3(codec);
        if (avcodec_open2(codecContext, codec, nullptr) != 0) {
            LOGE("打开解码器失败");
            return false;
        }
    }



    return true;
}

long BugMediaBaseMediaInfo::getDuration() const {
    return durationSecond;
}


