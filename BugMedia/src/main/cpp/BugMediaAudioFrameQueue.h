//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAAUDIOFRAMEQUEUE_H
#define SLOWER_BUGMEDIAAUDIOFRAMEQUEUE_H
#include "BugMediaAudioFrame.h"



class BugMediaAudioFrameQueue {

    BugMediaAudioFrame * head;
    BugMediaAudioFrame * tail;
    int count;

public:

    int size() const;
    void enqueueFrame(BugMediaAudioFrame * frame);
    BugMediaAudioFrame * dequeueFrame();
    void clear();
    BugMediaAudioFrameQueue();
    ~BugMediaAudioFrameQueue();

};


#endif //SLOWER_BUGMEDIAAUDIOFRAMEQUEUE_H
