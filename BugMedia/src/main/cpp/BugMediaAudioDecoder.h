//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAAUDIODECODER_H
#define SLOWER_BUGMEDIAAUDIODECODER_H

#include "BugMediaBaseDecoder.h"

class BugMediaAudioDecoder : public BugMediaBaseDecoder {
    bool isEnd = false;
public:
    BugMediaAudioFrame *getFrame();

    bool end() const;

};


#endif //SLOWER_BUGMEDIAAUDIODECODER_H
