//
// Created by Gshine on 2021/2/6.
//

#ifndef SLOWER_BUGMEDIAFRAMEQUEUE_H
#define SLOWER_BUGMEDIAFRAMEQUEUE_H


template<typename T>
class BugMediaFrameQueue {
private:
    T *head = nullptr;
    T *tail = nullptr;
public:
    void enqueue(T *obj);

    T *dequeue();

    void clear();

    ~BugMediaFrameQueue();
};

template<typename T>
void BugMediaFrameQueue<T>::enqueue(T *obj) {
    if (obj == nullptr) {
        return;
    }

    if (head == nullptr) {
        head = obj;
        tail = obj;
    } else {
        tail->next = obj;
        tail = obj;
    }
}

template<typename T>
T *BugMediaFrameQueue<T>::dequeue() {
    if (head == nullptr) {
        return nullptr;
    }
    T *frame = head;
    head = head->next;
    return frame;
}

template<typename T>
void BugMediaFrameQueue<T>::clear() {
    T *next;
    while (head) {
        next = head->next;
        delete head;
        head = next;
    }
    head = nullptr;
    tail = nullptr;
}

template<typename T>
BugMediaFrameQueue<T>::~BugMediaFrameQueue() {
    clear();
}


#endif //SLOWER_BUGMEDIAFRAMEQUEUE_H
