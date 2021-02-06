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
    int32_t durationSecond = 0;
    AVCodec *avCodec = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    AVPacket *avPacket = nullptr;
    AVFrame *avFrame = nullptr;
    AVFormatContext *avFormatContext{};
    int trackIndex = -1;
public:
    int32_t getDuration() const;
    virtual ~ BugMediaBaseDecoder();
    BugMediaBaseDecoder(AVFormatContext *formatContext, int trackIdx);

};


#endif //SLOWER_BUGMEDIABASEDECODER_H
