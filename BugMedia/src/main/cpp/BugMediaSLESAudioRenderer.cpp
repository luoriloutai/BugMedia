//
// Created by Gshine on 2021/2/7.
//

#include "BugMediaSLESAudioRenderer.h"
#include "BugMediaPlayer.h"

#define DEBUGAPP

void BugMediaSLESAudioRenderer::play() {
    if (currentState != PLAYING) {
        currentState = PLAYING;
        sem_post(&playSem);

    }

}

void BugMediaSLESAudioRenderer::pause() {
    if (currentState == PLAYING) {
        currentState = PAUSE;
    }

}

void BugMediaSLESAudioRenderer::stop() {
    currentState = STOP;
    //sem_post(&playSem);

}

BugMediaSLESAudioRenderer::BugMediaSLESAudioRenderer(GetAudioFrameCallback callback,
                                                     void *ctx, int bufferSize) {

    queueSize = bufferSize;
    callbackContext = ctx;
    getAudioFrame = callback;

    sem_init(&playSem, 0, 0);

}


BugMediaSLESAudioRenderer::~BugMediaSLESAudioRenderer() {
    // 使线程退出等待状态
    // 在获取到信号后判断状态退出，使线程不会一直等待
    currentState = STOP;
    sem_post(&playSem);


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

    sem_destroy(&playSem);

    pthread_join(renderThread, nullptr);

}

void BugMediaSLESAudioRenderer::render() {
    pthread_create(&renderThread, nullptr, renderRoutine, this);

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

    sem_wait(&playSem);

    (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
    // 执行播放的是这个回调函数，控制流程也在这里面
    bufferQueueCallback(simpleBufferQueue, this);


//    while (true) {
//
//        sem_wait(&playSem);
//
//
//#ifdef DEBUGAPP
//        LOGD("是否播放：%s",currentState==PLAYING?"是":"否");
//        LOGD("是否暂停：%s",currentState==PAUSE?"是":"否");
//        LOGD("是否停止：%s",currentState==STOP?"是":"否");
//#endif
//
//        if (currentState == STOP) {
//            return;
//        }
//
//        if (currentState == PLAYING) {
//
//            (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
//            bufferQueueCallback(simpleBufferQueue, this);
//
//        }
//        else if (currentState == PAUSE) {
//
//
//            // 将播放起始时间点向前移即可
//            //startTimeMs = startTimeMs + pst;
//            //startTimeMs = getCurMsTime()-pst;
//
//            //sem_wait(&playSem);
//
//
//            if (quit) {
//                return;
//            }
//
//        } else if (currentState == STOP) {
//            break;
//        }
//    }

#ifdef DEBUGAPP
    LOGD("渲染结束");
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

    auto renderer = (BugMediaSLESAudioRenderer *) ctx;
    renderer->doBufferQueue();
}

// 向播放器缓冲填充数据。该方法SLES会自动回调，不需要控制延时
void BugMediaSLESAudioRenderer::doBufferQueue() {
    if (player == nullptr) {
        LOGE("播放器未初始化");
        return;
    }

    if (currentState == STOP) {
        (*simpleBufferQueue)->Clear(simpleBufferQueue);

        return;
    }


    if (startTimeMs == -1) {
        // 初次执行，记录时间点，后面的播放都依赖于这个起始时间点。
        // pts是基于这个时间点的消逝时间，即偏移
        startTimeMs = getCurMsTime();
    }

    auto *frame = getAudioFrame(callbackContext);

    if (frame == nullptr) {
        currentState = STOP;
        (*simpleBufferQueue)->Clear(simpleBufferQueue);

        return;
    }
    if (frame->isEnd) {
#ifdef DEBUGAPP
        LOGD("最后一帧");
#endif
        currentState = STOP;
        (*simpleBufferQueue)->Clear(simpleBufferQueue);

        return;
    }

    if (currentState == PAUSE) {
        //startTimeMs = getCurMsTime()-frame->pts;
        //startTimeMs = getCurMsTime();
        sem_wait(&playSem);
    }


//    int64_t pass = getCurMsTime() - startTimeMs;
//    delay = frame->pts - pass;
//    //delay = getCurMsTime()-frame->pts-startTimeMs-passtimeMs;
//
//
//
//    // pts比当前时间大，说明要等待时间到
//    if (delay > 0) {
//#ifdef DEBUGAPP
//        LOGD("延时时长：%lld", delay);
//#endif
//        av_usleep(delay * 1000);
//    }


    SLresult result = (*simpleBufferQueue)->Enqueue(simpleBufferQueue, frame->data, (SLuint32) frame->resampleSize);
    if (result == SL_RESULT_SUCCESS) {
        //sem_post(&playSem);
        delete frame->data;
        delete frame;

#ifdef DEBUGAPP
        LOGD("推入缓冲成功");

#endif

    } else {
        LOGE("缓冲数据时出错：%s", av_err2str(result));
    }


}


