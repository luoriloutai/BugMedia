//
// Created by Gshine on 2021/2/4.
//


#include "BugMediaVideoLoader.h"
#include "BugMediaCommon.h"

BugMediaVideoLoader::BugMediaVideoLoader(const char *url) {
    maxBufferSize = 500;
    minBufferSize = 50;
    videoFrameQueue = new BugMediaVideoFrameQueue();
    audioFrameQueue = new BugMediaAudioFrameQueue();
    //decodeThread= nullptr;
    this->url = url;
    this->audioInfo = new BugMediaAudioInfo();
    this->videoInfo = new BugMediaVideoInfo();
}

BugMediaAudioFrame *BugMediaVideoLoader::getNextAudioFrame() {
    return audioFrameQueue->dequeueFrame();
}

BugMediaVideoFrame *BugMediaVideoLoader::getNextVideoFrame() {
    return videoFrameQueue->dequeueFrame();
}

BugMediaVideoLoader::~BugMediaVideoLoader() {
    delete videoFrameQueue;
    delete audioFrameQueue;
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
    auto duration =formatContext->duration/AV_TIME_BASE;

    for (int i = 0; i < formatContext->nb_streams; ++i) {
        AVCodecParameters *codecParameters = formatContext->streams[i]->codecpar;
        if (codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            this->audioInfo->addTrackIndex(i);
            if (!this->audioInfo->init(codecParameters,duration)){
                LOGE("初始化音频解码器失败");
                return;
            }

        } else if (codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            this->videoInfo->addTrackIndex(i);
            if (!this->videoInfo->init(codecParameters,duration)){
                LOGE("初始化图像解码器失败");
                return;
            }
        }
    }

    if (audioInfo->getTrackCount() == 0 && videoInfo->getTrackCount() == 0) {
        LOGE("未获取到音视频数据");
        release();
        return;
    }




}

void BugMediaVideoLoader::init() {
    std::thread doWork([this] {
        initDecoder();
    });
}

void BugMediaVideoLoader::release() {
    if (packet != nullptr) {
        av_packet_free(&packet);
    }
    if (frame != nullptr) {
        av_frame_free(&frame);
    }
    if (codecContext != nullptr) {
        avcodec_close(codecContext);
        avcodec_free_context(&codecContext);
    }
    if (formatContext != nullptr) {
        avformat_close_input(&formatContext);
        avformat_free_context(formatContext);
    }


    delete audioInfo;
    delete videoInfo;

}
