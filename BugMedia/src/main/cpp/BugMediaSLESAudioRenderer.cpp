//
// Created by Gshine on 2021/2/7.
//

#include "BugMediaSLESAudioRenderer.h"

#define DEBUGAPP

void BugMediaSLESAudioRenderer::play() {
    currentState = PLAYING;
    resumePlay();
}

void BugMediaSLESAudioRenderer::pause() {
    currentState = PAUSE;
}

void BugMediaSLESAudioRenderer::stop() {
    currentState = STOP;
    resumePlay();
}

BugMediaSLESAudioRenderer::BugMediaSLESAudioRenderer(GetAudioFrameCallback callback,
                                                     void *ctx, int bufferSize) {

    queueSize = bufferSize;
    callbackContext = ctx;
    getAudioFrame = callback;

    sem_init(&canFillQueue, 0, queueSize);
    sem_init(&canTakeData, 0, 0);


}


BugMediaSLESAudioRenderer::~BugMediaSLESAudioRenderer() {
    // 使线程退出等待状态
    // 在获取到信号后判断状态退出，使线程不会一直等待
    currentState = STOP;
    sem_post(&canFillQueue);
    sem_post(&canTakeData);

    quit = true;
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&condPlay);
    pthread_mutex_unlock(&mutex);

    pthread_cond_destroy(&condPlay);
    pthread_mutex_destroy(&mutex);


    //停止播放器
    if (player) {
        (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED);
        player = nullptr;
    }


    //销毁播放器对象
    if (playerObj) {
        (*playerObj)->Destroy(playerObj);
        playerObj = nullptr;
        simpleBufferQueue = nullptr;
    }
    //销毁混音器对象
    if (mixObj) {
        (*mixObj)->Destroy(mixObj);
        mixObj = nullptr;
    }
    //销毁引擎和引擎对象
    if (engineObj) {
        (*engineObj)->Destroy(engineObj);
        engineObj = nullptr;
        engine = nullptr;
    }
    //释放队列数据
    for (int i = 0; i < playQueue.size(); ++i) {
        //PcmData *pcm = playQueue.front();
        BugMediaAudioFrame *frame = playQueue.front();
        delete frame->data;
        delete frame;
        playQueue.pop();
    }

    sem_destroy(&canFillQueue);
    sem_destroy(&canTakeData);
    pthread_join(renderThread, nullptr);

}

void BugMediaSLESAudioRenderer::render() {
    pthread_create(&renderThread, nullptr, renderRoutine, this);

}

// 如果有错返回true，跳出循环
bool BugMediaSLESAudioRenderer::renderOnce() {
#ifdef DEBUGAPP
    LOGD("开始渲染一帧");
#endif

    sem_wait(&canFillQueue);
    // 用于正常退出等待状态，需要在析构函数里再释放一个信号，同时把currentState设置为STOP
    if (currentState == STOP) {
#ifdef DEBUGAPP
        LOGD("Stop 退出");
#endif

        return true;
    }
    //
    // 获取帧，通过回调函数。因为不知道其他线程在什么时候初始化完毕
    // 所以用回调函数，其他线程执行初始化完成后再初始化本对象
    //
    if (getAudioFrame == nullptr) {
#ifdef DEBUGAPP
        LOGD("回调函数");
#endif

        return true;
    }

    auto *frame = getAudioFrame(callbackContext);

    if (frame == nullptr) {

#ifdef DEBUGAPP
        LOGD("未取到帧");
#endif

        currentState = STOP;
        return true;
    }
    if (frame->isEnd) {
#ifdef DEBUGAPP
        LOGD("最后一帧");
#endif

        currentState = STOP;
        return true;
    }

#ifdef DEBUGAPP
    LOGD("计算延时");
#endif

    int64_t pass = getCurMsTime() - startTimeMs;
    delay = frame->pts - pass;
    // pts比当前时间大，说明要等待时间到
    if (delay > 0) {
        av_usleep(delay);
    }

    playQueue.push(frame);
    sem_post(&canTakeData);


#ifdef DEBUGAPP
    LOGD("渲染一帧结束");
#endif

    return false;
}


void *BugMediaSLESAudioRenderer::renderRoutine(void *ctx) {
    auto render = (BugMediaSLESAudioRenderer *) ctx;
    render->doRender();
    return nullptr;
}

void BugMediaSLESAudioRenderer::doRender() {
    if (!createEngine()) {
        LOGE("createEngine 失败");
        return;
    }
    if (!createOutputMixer()) {
        LOGE("createOutputMixer 失败");
        return;
    }
    if (!createPlayer()) {
        LOGE("createPlayer 失败");
        return;
    }

#ifdef DEBUGAPP
    LOGD("引擎、混音器、播放器创建完毕");
#endif

    while (true) {

#ifdef DEBUGAPP
        LOGD("进入循环");
#endif

        waitPlay();

#ifdef DEBUGAPP
        LOGD("等待完成");
#endif

        if (currentState == PLAYING) {

#ifdef DEBUGAPP
            LOGD("进入[播放状态]");
#endif

            if (startTimeMs == -1) {
                // 初次执行，记录时间点，后面的播放都依赖于这个起始时间点。
                // pts是基于这个时间点的消逝时间，即偏移
                startTimeMs = getCurMsTime();
            }

#ifdef DEBUGAPP
            LOGD("起始时间:%lld", startTimeMs);
#endif

            if (renderOnce()) {
                break;
            }
#ifdef DEBUGAPP
            LOGD("一帧结束时间:%lld", getCurMsTime());
#endif

            resumePlay();

        } else if (currentState == PAUSE) {
            // 将播放起始时间点向前移即可
            startTimeMs = startTimeMs + pst;
            //startTimeMs = getCurMsTime()-pst;

            waitPlay();

            // 暂停后少了一个信号，下一次循环开头的waitPlay()将阻塞，
            // 不能播放,这里给它一个信号，让它继续执行
            resumePlay();

            if (quit) {
                return;
            }

        } else if (currentState == STOP) {
            break;
        }
    }

#ifdef DEBUGAPP
    LOGD("渲染结束，没有更多的数据要处理");
#endif


}

bool BugMediaSLESAudioRenderer::createEngine() {
    if (slCreateEngine(&engineObj, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS) {
        LOGE("slCreateEngine");
        return false;
    }

    if ((*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) {
        LOGE("realize");
        return false;
    }

    if ((*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine) != SL_RESULT_SUCCESS) {
        LOGE("GetInterface");
        return false;
    }
    return true;
}

bool BugMediaSLESAudioRenderer::createOutputMixer() {
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    SLresult result = (*engine)->CreateOutputMix(engine, &mixObj, 1, ids, req);
    if (resultErr(result, "CreateOutputMix")) return false;

    result = (*mixObj)->Realize(mixObj, SL_BOOLEAN_FALSE);
    if (resultErr(result, "(*mixObj)->Realize")) return false;

//    // 混响设置不了，程序崩溃
//
//    result = (*mixObj)->GetInterface(mixObj, SL_IID_ENVIRONMENTALREVERB, &envReverb);
//    if (resultErr(result, "mixObj")) return false;
//
//#ifdef DEBUGAPP
//    LOGD("混响器接口是否为空：%s",envReverb== nullptr?"是":"否");
//#endif
//
//    result = (*envReverb)->SetEnvironmentalReverbProperties(envReverb, &envReverbSettings);
//    return !resultErr(result, "SetEnvironmentalReverbProperties");

    return true;
}

bool BugMediaSLESAudioRenderer::createPlayer() {
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            QUEUE_BUFFER_COUNT};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) 2,//声道数
            SL_SAMPLINGRATE_44_1,//采样率44100hz
            SL_PCMSAMPLEFORMAT_FIXED_16,//采样格式，位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//与采样位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,// 声道（源）位置，设置为左前和右前
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mixObj};
    SLDataSink slDataSink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result = (*engine)->CreateAudioPlayer(engine, &playerObj, &slDataSource, &slDataSink, 3, ids, req);
    if (resultErr(result, "(*engine)->CreateAudioPlayer")) return false;

    result = (*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE);
    if (resultErr(result, "(*playerObj)->Realize")) return false;

    result = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);
    if (resultErr(result, "GetInterface player")) return false;

    result = (*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &simpleBufferQueue);
    if (resultErr(result, "GetInterface bufferQueue")) return false;

    // 注册向缓冲中写入数据的回调函数
    // opensl es 在缓冲消耗完后回调用该方法再次获取数据
    result = (*simpleBufferQueue)->RegisterCallback(simpleBufferQueue, bufferQueueCallback, this);
    if (resultErr(result, "(*bufferQueue)->RegisterCallback")) return false;

    //获取音量接口
    result = (*playerObj)->GetInterface(playerObj, SL_IID_VOLUME, &playerVolume);
    return !resultErr(result, "GetInterface playerVolume");


}

bool BugMediaSLESAudioRenderer::resultErr(SLresult r, const char *errMsg) {
    if (r != SL_RESULT_SUCCESS) {
        LOGE("%s,%s", errMsg, av_err2str(r));
        return true;
    }
    return false;
}

void BugMediaSLESAudioRenderer::bufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *ctx) {
    auto self = (BugMediaSLESAudioRenderer *) ctx;
    self->doBufferQueue();
}

// 向播放器缓冲填充数据，使用自己定义的队列里的数据
void BugMediaSLESAudioRenderer::doBufferQueue() {
    if (player == nullptr) return;

    sem_wait(&canTakeData);

    // 用于退出线程等待，但需要在析构函数里再释放
    // 一个信号，同时把currentState设置为STOP，
    // 使得如果当前线程卡在上面一步时能顺利退出
    if (currentState == STOP) {
        return;
    }

    // 如果数据被复制到缓冲区则不需要这个步骤
//    while (!playQueue.empty()) {
//        PcmData *pcm = playQueue.front();
//        if (pcm->used) {
//            playQueue.pop();
//            delete pcm;
//        } else {
//            break;
//        }
//    }


    BugMediaAudioFrame *frame = playQueue.front();
    if (nullptr != frame && player) {
        SLresult result = (*simpleBufferQueue)->Enqueue(simpleBufferQueue, frame->data, (SLuint32) frame->resampleSize);
        if (result == SL_RESULT_SUCCESS) {
            // 成功填入缓冲则移除数据，否则下次填充的还是这个数据
            delete frame->data;
            delete frame;
            playQueue.pop();

            sem_post(&canFillQueue);
        }
    }
}

void BugMediaSLESAudioRenderer::waitPlay() {

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&condPlay, &mutex);
    pthread_mutex_unlock(&mutex);
}

void BugMediaSLESAudioRenderer::resumePlay() {

    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&condPlay);
    pthread_mutex_unlock(&mutex);
}
