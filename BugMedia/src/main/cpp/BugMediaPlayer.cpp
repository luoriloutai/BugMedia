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
        audioRenderer= nullptr;
        delete videoRenderer;
        videoRenderer = nullptr;

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

    int ret = avformat_open_input(&formatContext, url, nullptr, nullptr);
    if (ret != 0) {
        LOGE("打开URL [%s] 失败:%s", url, av_err2str(ret));
        release();
        return;
    }

    ret = avformat_find_stream_info(formatContext, nullptr);
    if (ret != 0) {
        LOGE("查找流信息失败:%s", av_err2str(ret));
        release();
        return;
    }

    duration = formatContext->duration*av_q2d(AV_TIME_BASE_Q);


#ifdef DEBUGAPP
    LOGD("总时长：%lld，转换后：%ds,比特率：%lld",formatContext->duration,duration,formatContext->bit_rate);
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

    currentVideoDecoder = videoDecoders[0];
    currentAudioDecoder = audioDecoders[0];

    if (currentAudioDecoder != nullptr) {
#ifdef DEBUGAPP
LOGD("音频解码器数量:%d,",audioDecoders.size());
        currentAudioDecoder->startDecode();
#else

        currentAudioDecoder->startDecode();
        audioRenderer = new BugMediaSLESAudioRenderer(getAudioFrameData, this);
        audioRenderer->render();
#endif
    }

    if (currentVideoDecoder != nullptr) {
#ifdef DEBUGAPP
        LOGD("视频解码器数量:%d",videoDecoders.size());
#else
        currentVideoDecoder->startDecode();
        videoRenderer = new BugMediaGLESVideoRenderer();
        videoRenderer->render();
#endif
    }


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
    //currentAudioDecoder->openDecoder();
}

void BugMediaPlayer::switchVideoChannel(int ch) {
    currentVideoDecoder = videoDecoders[ch];
    //currentVideoDecoder->openDecoder();
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







