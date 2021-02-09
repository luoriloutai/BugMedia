//
// Created by Gshine on 2021/2/4.
//


#include "BugMediaVideoLoader.h"
#include "BugMediaCommon.h"

BugMediaVideoLoader::BugMediaVideoLoader(const char *url, int bufferSize) {
    maxBufferSize = bufferSize;
    this->url = url;
    //videoDecoders = vector<BugMediaFFmpegVideoDecoder *>();
    //audioDecoders = vector<BugMediaFFmpegAudioDecoder *>();

}

BugMediaVideoLoader::~BugMediaVideoLoader() {
    if (!isRelease) {
        release();
    }

}

void BugMediaVideoLoader::release() {
    if (!isRelease) {
        isRelease = true;
        if (formatContext != nullptr) {
            avformat_close_input(&formatContext);
            avformat_free_context(formatContext);
        }

        for (auto &audioDecoder : audioDecoders) {
            delete audioDecoder;
        }
        for (auto &videoDecoder : videoDecoders) {
            delete videoDecoder;
        }

        pthread_join(initThread, nullptr);

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
                auto *audioDecoder = new BugMediaFFmpegAudioDecoder(formatContext, i, maxBufferSize);
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
                auto *videoDecoder = new BugMediaFFmpegVideoDecoder(formatContext, i, maxBufferSize);

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


void BugMediaVideoLoader::setBufferSize(int size) {
    maxBufferSize = size;
}

void BugMediaVideoLoader::load() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&initThread, &attr, initThreadFunc, this);
}

void BugMediaVideoLoader::switchAudioChannel(int ch) {
    currentAudioDecoder = audioDecoders[ch];
}

void BugMediaVideoLoader::switchVideoChannel(int ch) {
    currentVideoDecoder = videoDecoders[ch];
}


int BugMediaVideoLoader::getAudioTrackCount() const {
    return audioTrackCount;
}

int BugMediaVideoLoader::getVideoTrackCount() const {
    return videoTrackCount;
}

int64_t BugMediaVideoLoader::getAudioPts() const {
    return audioPts;
}

uint64_t BugMediaVideoLoader::getInAudioChannelLayout() {
    return currentAudioDecoder->getInChannelLayout();
}

int BugMediaVideoLoader::getInAudioSampleRate() {
    return currentAudioDecoder->getInSampleRate();
}

AVSampleFormat BugMediaVideoLoader::getInAudioSampleFormat() {
    return currentAudioDecoder->getInSampleFormat();
}

BugMediaDecoder::BugMediaAVFrame *BugMediaVideoLoader::getAudioFrame() {
    auto frame = currentAudioDecoder->getFrame();
    if (frame== nullptr){
        isAudioEnd=true;
        return nullptr;
    }
    audioPts = frame->audioFrame->pts;
    if (!frame->audioFrame->isEnd) {
        isAudioEnd = true;
    }

    return frame;

}

BugMediaDecoder::BugMediaAVFrame *BugMediaVideoLoader::getVideoFrame() {
    auto frame = currentVideoDecoder->getFrame();
    if (frame== nullptr){
        isVideoEnd=true;
        return nullptr;
    }
    if (!frame->videoFrame->isEnd) {
        isVideoEnd = true;
    }
    return frame;
}

//BugMediaAudioFrame *BugMediaVideoLoader::getAudioFrame() {
//
//    auto frame = currentAudioDecoder->getFrame();
//    audioPts = frame->pts;
//    if (!frame->isEnd) {
//        isAudioEnd = true;
//    }
//
//    return frame;
//}
//
//BugMediaVideoFrame *BugMediaVideoLoader::getVideoFrame() {
//
//    auto frame = currentVideoDecoder->getFrame();
//    if (!frame->isEnd) {
//        isVideoEnd = true;
//    }
//    return frame;
//}







