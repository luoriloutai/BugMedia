//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIABASEDECODER_H
#define SLOWER_BUGMEDIABASEDECODER_H

extern "C" {
#include "include/ffmpeg/libavcodec/avcodec.h"
#include "include/ffmpeg/libavformat/avformat.h"
#include "include/ffmpeg/libswscale/swscale.h"
#include "include/ffmpeg/libswresample/swresample.h"
#include "include/ffmpeg/libavutil/pixdesc.h"
#include "include/ffmpeg/libavutil/frame.h"
#include "include/ffmpeg/libavutil/time.h"
}

#include <vector>
#include "BugMediaCommon.h"
#include "BugMediaVideoFrame.h"
#include "BugMediaAudioFrame.h"



class BugMediaBaseDecoder {

protected:
    std::vector<int32_t> trackIndices;
    int32_t durationSecond = 0;
    AVCodec *avCodec = nullptr;
    AVCodecParameters *avCodecPar = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    AVPacket *avPacket = nullptr;
    AVFrame *avFrame = nullptr;
    AVFormatContext * avFormatContext{};
public:

    int getFirstTrackIndex();

    long getDuration() const;

    int getTrackCount();

    void addTrackIndex(int32_t i);

    std::vector<int32_t> getTrackIndices() const;

    bool init(AVFormatContext *formatContext,AVCodecParameters *parameters, int32_t duration);

    virtual ~ BugMediaBaseDecoder();

    BugMediaBaseDecoder();




};


#endif //SLOWER_BUGMEDIABASEDECODER_H
