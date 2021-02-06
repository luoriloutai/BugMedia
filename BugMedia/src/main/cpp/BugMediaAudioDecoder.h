//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAAUDIODECODER_H
#define SLOWER_BUGMEDIAAUDIODECODER_H

#include "BugMediaBaseDecoder.h"

class BugMediaAudioDecoder : public BugMediaBaseDecoder {
public:
    BugMediaAudioFrame *getFrame();

    BugMediaAudioDecoder(AVFormatContext * formatContext,int trackIdx);

};


#endif //SLOWER_BUGMEDIAAUDIODECODER_H
