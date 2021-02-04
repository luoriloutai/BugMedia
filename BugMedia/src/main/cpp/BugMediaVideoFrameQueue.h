//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAVIDEOFRAMEQUEUE_H
#define SLOWER_BUGMEDIAVIDEOFRAMEQUEUE_H

#include "BugMediaVideoFrame.h"

class BugMediaVideoFrameQueue {
    BugMediaVideoFrame *head;
    BugMediaVideoFrame *tail;
    int count;
public:
    int size();

    void enqueueFrame(BugMediaVideoFrame *frame);

    BugMediaVideoFrame *dequeueFrame();

    BugMediaVideoFrameQueue();

    ~BugMediaVideoFrameQueue();
};


#endif //SLOWER_BUGMEDIAVIDEOFRAMEQUEUE_H
