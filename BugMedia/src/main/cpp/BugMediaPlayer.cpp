//
// Created by Gshine on 2021/2/4.
//


#include "BugMediaPlayer.h"
#include "BugMediaCommon.h"
#include "BugMediaFFmpegAudioDecoder.h"
#include "BugMediaFFmpegVideoDecoder.h"

#define DEBUGAPP

BugMediaPlayer::BugMediaPlayer(const char *url, int bufferSize) {
    maxBufferSize = bufferSize;
    this->url = url;

    //videoDecoders = vector<BugMediaFFmpegVideoDecoder *>();
    //audioDecoders = vector<BugMediaFFmpegAudioDecoder *>();

}

BugMediaPlayer::~BugMediaPlayer() {
    if (!isRelease) {
        release();
    }

}

void BugMediaPlayer::release() {
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

        delete audioRenderer;
        delete videoRenderer;

        pthread_join(initThread, nullptr);

    }

}

// 线程执行函数
void *BugMediaPlayer::initThreadFunc(void *pVoid) {
    auto *loader = (BugMediaPlayer *) pVoid;
    // 使用另一个方法，简化代码写法，不然每个变量访问
    // 都得类似于:loader->xxxx
    loader->init();
    return nullptr;
}

// 正经执行初始化操作的函数
void BugMediaPlayer::init() {

    formatContext = avformat_alloc_context();

#ifdef DEBUGAPP
    LOGD("formatContext是否为空：%s", formatContext == nullptr ? "是" : "否");
#endif

    int ret = avformat_open_input(&formatContext, url, nullptr, nullptr);
    if (ret != 0) {
        LOGE("打开URL [%s] 失败:%s", url, av_err2str(ret));
        release();
        return;
    }

#ifdef DEBUGAPP
    LOGD("av_format_open_input end");
#endif

    ret = avformat_find_stream_info(formatContext, NULL);
    if (ret != 0) {
        LOGE("查找流信息失败:%s", av_err2str(ret));
        release();
        return;
    }

#ifdef DEBUGAPP
    LOGD("avformat_find_stream_info end");
#endif

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
                return;
            }
        }
    }

#ifdef DEBUGAPP
    LOGD("音轨数:%d,视轨数:%d", audioTrackCount, videoTrackCount);

#endif

    currentVideoDecoder = videoDecoders[0];
    currentAudioDecoder = audioDecoders[0];

    currentAudioDecoder->openDecoder();
    currentVideoDecoder->openDecoder();

    audioRenderer = new BugMediaSLESAudioRenderer(currentAudioDecoder->getInSampleRate(),
                                                  currentAudioDecoder->getInChannelLayout(),
                                                  currentAudioDecoder->getInSampleFormat(),
                                                  getAudioFrameData, this);
    audioRenderer->getAudioFrame = getAudioFrameData;
    audioRenderer->render();

    videoRenderer = new BugMediaGLESVideoRenderer();
    //videoRenderer->render();


}


void BugMediaPlayer::setBufferSize(int size) {
    maxBufferSize = size;
}

void BugMediaPlayer::load() {
    // 文件探测可能会花很长时间，
    // 所以启动一个线程执行

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&initThread, &attr, initThreadFunc, this);
}

void BugMediaPlayer::switchAudioChannel(int ch) {
    currentAudioDecoder = audioDecoders[ch];
    currentAudioDecoder->openDecoder();
}

void BugMediaPlayer::switchVideoChannel(int ch) {
    currentVideoDecoder = videoDecoders[ch];
    currentVideoDecoder->openDecoder();
}


int BugMediaPlayer::getAudioTrackCount() const {
    return audioTrackCount;
}

int BugMediaPlayer::getVideoTrackCount() const {
    return videoTrackCount;
}

int64_t BugMediaPlayer::getAudioPts() const {
    return audioPts;
}


BugMediaAudioFrame *BugMediaPlayer::getAudioFrame() {

    auto frame = currentAudioDecoder->getFrame();
    audioPts = frame->pts;
    if (!frame->isEnd) {
        isAudioEnd = true;
    }

    return frame;
}

BugMediaVideoFrame *BugMediaPlayer::getVideoFrame() {

    auto frame = currentVideoDecoder->getFrame();
    if (!frame->isEnd) {
        isVideoEnd = true;
    }
    return frame;
}

BugMediaAudioFrame *BugMediaPlayer::getAudioFrameData(void *ctx) {
    auto loader = (BugMediaPlayer *) ctx;
    return loader->getAudioFrame();
}

void BugMediaPlayer::play() {
    audioRenderer->play();
    //videoRenderer->play();
}

void BugMediaPlayer::pause() {
    audioRenderer->pause();
    //videoRenderer->pause();
}

void BugMediaPlayer::stop() {
    audioRenderer->stop();
    //videoRenderer->stop();
}

void BugMediaPlayer::destroy() {
    release();
}

void BugMediaPlayer::setWindowSurface(JNIEnv *env, jobject surface) {
    videoRenderer->setWindowSurface(env, surface);
}

void BugMediaPlayer::setPBufferSurface(EGLint width, EGLint height) {
    videoRenderer->setPBufferSurface(width, height);
}

void BugMediaPlayer::resizeView(GLint x, GLint y, GLsizei width, GLsizei height) {
    videoRenderer->resizeView(x, y, width, height);
}







