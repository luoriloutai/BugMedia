//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIAAUDIODECODER_H
#define SLOWER_BUGMEDIAAUDIODECODER_H

#include "BugMediaBaseDecoder.h"
#include "BugMediaAudioFrame.h"
#include <pthread.h>
#include <semaphore.h>
#include <queue>
using namespace std;


class BugMediaAudioDecoder : public BugMediaBaseDecoder {
    queue<BugMediaAudioFrame*> frameQueue{};
    int bufferSize{};
    pthread_t decodeThread{};
    sem_t canFillData{};
    sem_t canTakeData{};

    // 解码并入队
    void decode();
    // 解码线程运行的函数，由于是静态的，需要将本类对象传入以访问类对象的成员
    // 由于在该方法内访问对象成员需要用本对象的指针，写起来麻烦，于是又定义了
    // decode()方法，这样只需要在这个函数里将ctx转成本类对象指针后调用一次decode()
    static void *decodeRoutine(void *ctx);
public:
    BugMediaAudioFrame *getFrame();

    BugMediaAudioDecoder(AVFormatContext * formatContext,int trackIdx,int bufferSize=100);

    ~BugMediaAudioDecoder();

};


#endif //SLOWER_BUGMEDIAAUDIODECODER_H
