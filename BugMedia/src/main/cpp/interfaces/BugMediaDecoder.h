//
// Created by Gshine on 2021/2/9.
//

#ifndef SLOWER_BUGMEDIADECODER_H
#define SLOWER_BUGMEDIADECODER_H

#include "../BugMediaVideoFrame.h"
#include "../BugMediaAudioFrame.h"

class BugMediaDecoder {
public:
    union BugMediaAVFrame{
        BugMediaAudioFrame *audioFrame;
        BugMediaVideoFrame *videoFrame;
    };


    virtual BugMediaAVFrame *getFrame()=0;
};

#endif //SLOWER_BUGMEDIADECODER_H
