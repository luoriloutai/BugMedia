//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaVideoFrameQueue.h"

void BugMediaVideoFrameQueue::enqueueFrame(BugMediaVideoFrame *frame) {
    if (frame== nullptr){
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

BugMediaVideoFrame *BugMediaVideoFrameQueue::dequeueFrame() {
    if (head == nullptr) {
        return nullptr;
    }
    BugMediaVideoFrame *frame = head;
    head = head->next;
    return frame;
}

BugMediaVideoFrameQueue::BugMediaVideoFrameQueue() {
    head = nullptr;
    tail = nullptr;
}

BugMediaVideoFrameQueue::~BugMediaVideoFrameQueue() {
    BugMediaVideoFrame *next;
    while (head) {
        next = head->next;
        delete head;
        head=next;
    }
}

