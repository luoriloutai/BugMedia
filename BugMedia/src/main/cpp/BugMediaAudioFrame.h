//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAAUDIOFRAME_H
#define SLOWER_BUGMEDIAAUDIOFRAME_H

extern "C"{
#include "include/ffmpeg/libavutil/frame.h"
}


struct BugMediaAudioFrame {
    uint8_t *data{};

    // 以毫秒为单位的时间戳
    int64_t pts{};
    bool isEnd = false;
    int format{};
    float_t position{};
    int sampleRate{};
    int channels{};
    int sampleCount{};
    size_t resampleSize{};
};


#endif //SLOWER_BUGMEDIAAUDIOFRAME_H
