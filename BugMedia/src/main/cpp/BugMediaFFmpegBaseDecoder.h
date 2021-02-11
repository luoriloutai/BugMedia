//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAFFMPEGBASEDECODER_H
#define SLOWER_BUGMEDIAFFMPEGBASEDECODER_H


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

class BugMediaFFmpegBaseDecoder {

protected:
    int32_t streamDuration = 0;
    AVCodec *avCodec = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    AVPacket *avPacket = nullptr;
    AVFrame *avFrame = nullptr;
    AVFormatContext *avFormatContext{};
    int trackIndex = -1;
    const char *codecType{};

public:
    int32_t getStreamDuration() const;

    virtual ~ BugMediaFFmpegBaseDecoder();

    BugMediaFFmpegBaseDecoder(AVFormatContext *formatContext, int trackIdx);

private:
    void openDecoder();
};


#endif //SLOWER_BUGMEDIAFFMPEGBASEDECODER_H
