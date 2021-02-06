//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAVIDEOFRAME_H
#define SLOWER_BUGMEDIAVIDEOFRAME_H


#include "include/ffmpeg/libavutil/frame.h"

struct BugMediaVideoFrame {
    BugMediaVideoFrame *next;
    bool isKeyframe{};
    bool isInterlaced{};
    int width{};
    int height{};
    uint8_t **data;
    int64_t pts{};

};


#endif //SLOWER_BUGMEDIAVIDEOFRAME_H
