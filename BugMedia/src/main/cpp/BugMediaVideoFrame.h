//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAVIDEOFRAME_H
#define SLOWER_BUGMEDIAVIDEOFRAME_H

extern "C"{
#include "include/ffmpeg/libavutil/frame.h"
};


struct BugMediaVideoFrame {
    bool isKeyframe{};
    bool isInterlaced{};
    int width{};
    int height{};
    uint8_t **data{};

    // 以毫秒为单位的时间戳
    int64_t pts{};
    bool isEnd= false;
    int format{};
    float_t  position{};

};


#endif //SLOWER_BUGMEDIAVIDEOFRAME_H
