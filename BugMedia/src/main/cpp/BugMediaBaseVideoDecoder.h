//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIABASEVIDEODECODER_H
#define SLOWER_BUGMEDIABASEVIDEODECODER_H


#include "BugMediaAudioFrameQueue.h"
#include "BugMediaVideoFrameQueue.h"
#include <pthread.h>


class BugMediaBaseVideoDecoder {
    int maxBufferSize;
    int minBufferSize;
    BugMediaAudioFrameQueue *audioFrameQueue;
    BugMediaVideoFrameQueue *videoFrameQueue;
    pthread_t decodeThread;
    const char *filePath;

public:

    BugMediaBaseVideoDecoder(const char *filePath);
    ~BugMediaBaseVideoDecoder();
    BugMediaAudioFrame * getNextAudioFrame();
    BugMediaVideoFrame * getNextVideoFrame();

};


#endif //SLOWER_BUGMEDIABASEVIDEODECODER_H
