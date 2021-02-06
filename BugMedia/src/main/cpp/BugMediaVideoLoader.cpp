//
// Created by Gshine on 2021/2/4.
//


#include "BugMediaVideoLoader.h"
#include "BugMediaCommon.h"

BugMediaVideoLoader::BugMediaVideoLoader(const char *url,int bufferSize=100) {
    maxBufferSize = bufferSize;
    videoFrameQueue = new BugMediaVideoFrameQueue();
    audioFrameQueue = new BugMediaAudioFrameQueue();
    this->url = url;
    this->audioDecoder = new BugMediaAudioDecoder();
    this->videoDecoder = new BugMediaVideoDecoder();
    sem_init(&bufferReadable, 0, 0);
    sem_init(&bufferWritable, 0, maxBufferSize);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&worker, &attr, threadWorking, this);
}

BugMediaAudioFrame *BugMediaVideoLoader::getNextAudioFrame() {
    sem_wait(&bufferReadable);
    auto frame = audioFrameQueue->dequeueFrame();
    if (frame == nullptr) {
        sem_destroy(&bufferReadable);
        return nullptr;
    }
    if (!isEnd) {
        sem_post(&bufferWritable);
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

    // formatContext->duration以微秒为单位，AV_TIME_BASE恰好是1秒换算成微秒的数
    auto duration = formatContext->duration / AV_TIME_BASE;

    for (int i = 0; i < formatContext->nb_streams; ++i) {
        AVCodecParameters *codecParameters = formatContext->streams[i]->codecpar;
        if (codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            this->audioDecoder->addTrackIndex(i);
            if (!this->audioDecoder->init(formatContext, codecParameters, duration)) {
                LOGE("初始化音频解码器失败");
                return;
            }

        } else if (codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            this->videoDecoder->addTrackIndex(i);
            if (!this->videoDecoder->init(formatContext, codecParameters, duration)) {
                LOGE("初始化图像解码器失败");
                return;
            }
        }
    }

    if (audioDecoder->getTrackCount() == 0 && videoDecoder->getTrackCount() == 0) {
        LOGE("未获取到音视频数据");
        release();
        return;
    }


}

void BugMediaVideoLoader::release() {
    if (!isRelease) {
        if (formatContext != nullptr) {
            avformat_close_input(&formatContext);
            avformat_free_context(formatContext);
        }

        delete videoFrameQueue;
        delete audioFrameQueue;
        delete audioDecoder;
        delete videoDecoder;
        sem_destroy(&bufferWritable);
        sem_destroy(&bufferReadable);

        isRelease = true;
    }

}

// 线程执行函数,一个壳子，里面装着正经干活的函数
void *BugMediaVideoLoader::threadWorking(void *pVoid) {
    auto *loader = (BugMediaVideoLoader *) pVoid;
    // 使用另一个方法，简化代码写法，不然每个变量访问
    // 都得类似于:loader->xxxx
    loader->doWork();
    return nullptr;
}

// 正经执行操作的函数
void BugMediaVideoLoader::doWork() {
    initDecoder();

    // 一个生产线程，一个消费线程的模式，不需要互斥，
    // 生产者与消费者之间不需要互斥，
    // 多生产者之间，多消费者之间才需要，因为它们执
    // 行相同的操作，会碰撞在一起
    while (true) {
        sem_wait(&bufferWritable);

        if (audioDecoder->end()) {
            break;
        }
        BugMediaAudioFrame *aFrame = audioDecoder->getFrame();
        audioFrameQueue->enqueueFrame(aFrame);

        if (videoDecoder->end()) {
            break;
        }
        BugMediaVideoFrame *vFrame = videoDecoder->getFrame();
        videoFrameQueue->enqueueFrame(vFrame);

        sem_post(&bufferReadable);

    }
    isEnd = true;
    sem_destroy(&bufferWritable);


}


