//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAVIDEODECODER_H
#define SLOWER_BUGMEDIAVIDEODECODER_H

#include "BugMediaBaseDecoder.h"
#include "BugMediaAudioFrame.h"
#include "BugMediaVideoFrame.h"
#include "BugMediaVideoFrameQueue.h"

class BugMediaVideoDecoder : public BugMediaBaseDecoder {
private:

public:

    BugMediaVideoDecoder(AVFormatContext *formatContext,int trackIdx);

    ~BugMediaVideoDecoder();

    BugMediaVideoFrame *getFrame();

};


#endif //SLOWER_BUGMEDIAVIDEODECODER_H
