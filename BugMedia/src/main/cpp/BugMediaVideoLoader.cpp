//
// Created by Gshine on 2021/2/4.
//


#include "BugMediaVideoLoader.h"
#include "BugMediaCommon.h"

BugMediaVideoLoader::BugMediaVideoLoader(const char *url, int bufferSize = 100) {
    maxBufferSize = bufferSize;
//    videoFrameQueue = new BugMediaVideoFrameQueue();
//    audioFrameQueue = new BugMediaAudioFrameQueue();
    this->url = url;
    videoDecoders = vector<BugMediaVideoDecoder *>();
    audioDecoders = vector<BugMediaAudioDecoder *>();
    audioFrameQueue = new BugMediaFrameQueue<BugMediaAudioFrame>();
    videoFrameQueue = new BugMediaFrameQueue<BugMediaVideoFrame>();

    sem_init(&canTakeVideoFrame, 0, 0);
    sem_init(&canTakeAudioFrame, 0, 0);
    sem_init(&canFillVideoFrame, 0, maxBufferSize);
    sem_init(&canFillAudioFrame, 0, maxBufferSize);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&initThread, &attr, initThreadFunc, this);
}

BugMediaAudioFrame *BugMediaVideoLoader::getAudioFrame() {
    sem_wait(&canTakeAudioFrame);
    auto frame = audioFrameQueue->dequeue();
    if(!frame->isEnd){
        sem_post(&canFillAudioFrame);
    }

    return frame;
}


BugMediaVideoLoader::~BugMediaVideoLoader() {
    if (!isRelease) {
        release();
    }

}

void BugMediaVideoLoader::initDecoder() {
    formatContext = avformat_alloc_context();

    if (avformat_open_input(&formatContext, url, nullptr, nullptr) != 0) {
        LOGE("打开URL失败:%s", url);
        release();
        return;
    }
    if (avformat_find_stream_info(formatContext, nullptr) != 0) {
        LOGE("查找流信息失败");
        release();
        return;
    }


    for (int i = 0; i < formatContext->nb_streams; ++i) {
        AVCodecParameters *codecParameters = formatContext->streams[i]->codecpar;
        if (codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            try {
                auto *audioDecoder = new BugMediaAudioDecoder(formatContext, i);
                audioDecoders.push_back(audioDecoder);
                audioTrackCount++;
            } catch (char *e) {
                LOGE("%s", e);
                LOGE("初始化音频解码器失败");
                release();
                return;
            }


        } else if (codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            try {
                auto *videoDecoder = new BugMediaVideoDecoder(formatContext, i);
                videoDecoders.push_back(videoDecoder);
                videoTrackCount++;
            } catch (char *e) {
                LOGE("%s", e);
                LOGE("初始化图像解码器失败");
                release();
            }
        }
    }

    currentVideoDecoder = videoDecoders[0];
    currentAudioDecoder = audioDecoders[0];

}

void BugMediaVideoLoader::release() {
    if (!isRelease) {
        if (formatContext != nullptr) {
            avformat_close_input(&formatContext);
            avformat_free_context(formatContext);
        }

        delete videoFrameQueue;
        delete audioFrameQueue;

        sem_destroy(&canFillAudioFrame);
        sem_destroy(&canFillVideoFrame);
        sem_destroy(&canTakeAudioFrame);
        sem_destroy(&canTakeVideoFrame);

        for (auto &audioDecoder : audioDecoders) {
            delete audioDecoder;
        }
        for (auto &videoDecoder : videoDecoders) {
            delete videoDecoder;
        }
        pthread_join(audioDecodeThread, nullptr);
        pthread_join(videoDecodeThread, nullptr);

        isRelease = true;
    }

}

// 线程执行函数,一个壳子，里面装着正经干活的函数
void *BugMediaVideoLoader::initThreadFunc(void *pVoid) {
    auto *loader = (BugMediaVideoLoader *) pVoid;
    // 使用另一个方法，简化代码写法，不然每个变量访问
    // 都得类似于:loader->xxxx
    loader->init();
    return nullptr;
}

// 正经执行初始化操作的函数
void BugMediaVideoLoader::init() {
    initDecoder();
    pthread_create(&audioDecodeThread, nullptr, audioDecodeFunc, this);
    pthread_create(&videoDecodeThread, nullptr, videoDecodeFunc, this);
}

// 音频解码线程函数
void *BugMediaVideoLoader::audioDecodeFunc(void *pVoid) {
    auto *loader = (BugMediaVideoLoader *) pVoid;
    loader->doAudioDecode();
    return nullptr;
}

// 视频解码线程函数
void *BugMediaVideoLoader::videoDecodeFunc(void *pVoid) {
    auto *loader = (BugMediaVideoLoader *) pVoid;
    loader->doVideoDecode();
    return nullptr;
}

// 线程内音频解码工作
void BugMediaVideoLoader::doAudioDecode() {
    while (true) {
        sem_wait(&canFillAudioFrame);

        BugMediaAudioFrame *aFrame = currentAudioDecoder->getFrame();
        audioFrameQueue->enqueue(aFrame);
        if (aFrame->isEnd){
            isAudioEnd= true;
            break;
        }

        sem_post(&canTakeAudioFrame);

    }
}

// 线程内视频解码工作
void BugMediaVideoLoader::doVideoDecode() {
    while (true) {
        sem_wait(&canFillVideoFrame);

        BugMediaVideoFrame *vFrame = currentVideoDecoder->getFrame();
        videoFrameQueue->enqueue(vFrame);
        if(vFrame->isEnd){
            isVideoEnd= true;
            break;
        }


        sem_post(&canTakeVideoFrame);

    }

}

BugMediaVideoFrame *BugMediaVideoLoader::getVideoFrame() {
    sem_wait(&canTakeVideoFrame);
    BugMediaVideoFrame * frame = videoFrameQueue->dequeue();
    if (!frame->isEnd){
        sem_post(&canFillVideoFrame);
    }
    return frame;
}


