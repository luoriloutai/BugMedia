//
// Created by Gshine on 2021/2/7.
//

#ifndef SLOWER_BUGMEDIASLESAUDIORENDERER_H
#define SLOWER_BUGMEDIASLESAUDIORENDERER_H

#define DEBUGAPP

extern "C" {
#include "include/ffmpeg/libavutil/opt.h"
#include "include/ffmpeg/libavutil/mem.h"
#include "include/ffmpeg/libswresample/swresample.h"
#include "include/ffmpeg/libavutil/time.h"
#include "include/ffmpeg/libswresample/swresample.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
}


#include <pthread.h>
#include <semaphore.h>
#include "BugMediaCommon.h"
#include "BugMediaAudioFrame.h"
#include <queue>

using namespace std;


class BugMediaSLESAudioRenderer {

    State currentState = UNSTART;


    // 开始执行操作时的时间（毫秒）
    int64_t startTimeMs = -1;

    int64_t delay{};

    pthread_t renderThread{};

    int64_t prePts{};

    sem_t playSem{};

    bool quit = false;

    int queueSize{};

    static void *renderRoutine(void *ctx);

    const SLuint32 QUEUE_BUFFER_COUNT = 2;

    // 引擎
    SLObjectItf engineObj = NULL;
    SLEngineItf engine = NULL;

    //混音器
    SLObjectItf mixObj = NULL;
    SLEnvironmentalReverbItf envReverb = NULL;
    // SL_I3DL2_ENVIRONMENT_PRESET_ROOM
    // SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT
    const SLEnvironmentalReverbSettings envReverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //播放器
    SLObjectItf playerObj = NULL;
    SLPlayItf player = NULL;
    SLVolumeItf playerVolume = NULL;

    // 这个接口用于向播放器缓冲区输入数据
    SLAndroidSimpleBufferQueueItf simpleBufferQueue{};


    void *callbackContext{};

    void doRender();

    bool createEngine();

    bool createOutputMixer();

    bool createPlayer();

    bool resultErr(SLresult r, const char *errMsg);

    static void bufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *ctx);

    // 给缓冲区填充数据
    void doBufferQueue();


public:

    void render();

    void play();

    void pause();

    void stop();

    typedef BugMediaAudioFrame *(*GetAudioFrameCallback)(void *ctx);

    BugMediaSLESAudioRenderer(GetAudioFrameCallback getAudioFrameCallback, void *ctx, int bufferSize = 10);

    ~BugMediaSLESAudioRenderer();

private:

    GetAudioFrameCallback getAudioFrame{};

};


#endif //SLOWER_BUGMEDIASLESAUDIORENDERER_H
