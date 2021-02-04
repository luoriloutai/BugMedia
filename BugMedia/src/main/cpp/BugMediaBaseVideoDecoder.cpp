//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaBaseVideoDecoder.h"

BugMediaBaseVideoDecoder::BugMediaBaseVideoDecoder(const char *filePath) {
    maxBufferSize = 500;
    minBufferSize = 50;
    videoFrameQueue = new BugMediaVideoFrameQueue();
    audioFrameQueue = new BugMediaAudioFrameQueue();
    decodeThread = 0;
    this->filePath=filePath;
}

BugMediaAudioFrame *BugMediaBaseVideoDecoder::getNextAudioFrame() {
    return audioFrameQueue->dequeueFrame();
}

BugMediaVideoFrame *BugMediaBaseVideoDecoder::getNextVideoFrame() {
    return videoFrameQueue->dequeueFrame();
}

BugMediaBaseVideoDecoder::~BugMediaBaseVideoDecoder() {
    delete videoFrameQueue;
    delete audioFrameQueue;
}
