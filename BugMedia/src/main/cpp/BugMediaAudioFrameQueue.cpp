//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaAudioFrameQueue.h"

void BugMediaAudioFrameQueue::enqueueFrame(BugMediaAudioFrame *frame) {
    if (frame == nullptr) {
        return;
    }

    if (head == nullptr) {
        head = frame;
        tail = frame;
    } else {
        tail->next = frame;
        tail = frame;
    }
}

BugMediaAudioFrame *BugMediaAudioFrameQueue::dequeueFrame() {
    if (head == nullptr) {
        return nullptr;
    }
    BugMediaAudioFrame *frame = head;
    head = head->next;
    return frame;
}

BugMediaAudioFrameQueue::BugMediaAudioFrameQueue() {
    head = nullptr;
    tail = nullptr;
    count = 0;
}

BugMediaAudioFrameQueue::~BugMediaAudioFrameQueue() {
    BugMediaAudioFrame *next;
    while (head) {
        next = head->next;
        delete head;
        head = next;
    }
}

int BugMediaAudioFrameQueue::size() const {
    return count;
}
