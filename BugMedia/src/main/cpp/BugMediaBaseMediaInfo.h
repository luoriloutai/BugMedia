//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIABASEMEDIAINFO_H
#define SLOWER_BUGMEDIABASEMEDIAINFO_H

#include <vector>
#include "BugMediaCommon.h"

extern "C" {
#include "include/ffmpeg/libavcodec/avcodec.h"
#include "include/ffmpeg/libavformat/avformat.h"
}

class BugMediaBaseMediaInfo {
    std::vector<int32_t> trackIndices;
    int32_t durationSecond = 0;

public:
    AVCodec *codec = nullptr;

    AVCodecParameters *codecPar = nullptr;

    AVCodecContext *codecContext = nullptr;

    int getFirstTrackIndex();

    long getDuration() const;

    int getTrackCount();

    void addTrackIndex(int32_t i);

    std::vector<int32_t> getTrackIndices() const;

    bool init(AVCodecParameters *parameters,int32_t duration);

    virtual ~ BugMediaBaseMediaInfo();

};


#endif //SLOWER_BUGMEDIABASEMEDIAINFO_H
