//
// Created by Gshine on 2021/2/4.
//


#include "BugMediaPlayer.h"
#include "BugMediaCommon.h"
#include "BugMediaFFmpegAudioDecoder.h"
#include "BugMediaFFmpegVideoDecoder.h"

#define DEBUGAPP

BugMediaPlayer::BugMediaPlayer(const char *url, int decoderBufferSize, JNIEnv *env,
                               jobject surface, EGLint width, EGLint height, bool createPBufferSurface) {

    // 由于JNIEnv是与线程关联的，不同线程之间不能互相访问
    // 而程序内创建渲染器是在另一个线程中进行的
    // 所以用到JNIEnv作为参数时不能直接用
    // 可以获取JavaVM，它是线程共享的
    // 用它可以为线程分配一个JNIEnv
    env->GetJavaVM(&javaVm);

    // 全局引用一个java对象，在其他线程也可以用
    this->surface = env->NewGlobalRef(surface);

    maxBufferSize = decoderBufferSize;
    this->url = url;
    this->width = width;
    this->height = height;
    this->createPBufferSurface = createPBufferSurface;

}

BugMediaPlayer::BugMediaPlayer(const char *url, int bufferSize, JNIEnv *env, jobject surface)
        : BugMediaPlayer(url, bufferSize, env, surface, 0, 0, false) {

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

        if (avPacket != nullptr) {
            av_packet_free(&avPacket);
        }
        if (avFrame != nullptr) {
            av_frame_free(&avFrame);
        }
        if (avCodecContext != nullptr) {
            avcodec_close(avCodecContext);
            avcodec_free_context(&avCodecContext);
        }

        for (auto &audioDecoder : audioDecoders) {
            delete audioDecoder;
        }
        for (auto &videoDecoder : videoDecoders) {
            delete videoDecoder;
        }

        delete audioRenderer;
        audioRenderer = nullptr;
        delete videoRenderer;
        videoRenderer = nullptr;

        pthread_join(loadThread, nullptr);

        env->DeleteGlobalRef(surface);
    }

}

// 线程执行函数
void *BugMediaPlayer::loadThreadFunc(void *pVoid) {
    auto *loader = (BugMediaPlayer *) pVoid;
    // 使用另一个方法，简化代码写法，不然每个变量访问
    // 都得类似于:loader->xxxx
    loader->loadStart();
    return nullptr;
}

// 正经执行操作的函数
void BugMediaPlayer::loadStart() {

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


    duration = formatContext->duration * av_q2d(AV_TIME_BASE_Q);


#ifdef DEBUGAPP
    LOGD("总时长：%lld，转换后：%ds,比特率：%lld", formatContext->duration, duration, formatContext->bit_rate);
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

    if (!videoDecoders.empty()) {
        currentVideoDecoder = videoDecoders[0];
    }

    if (!audioDecoders.empty()) {
        currentAudioDecoder = audioDecoders[0];
    }

    if (currentVideoDecoder == nullptr && currentAudioDecoder == nullptr) {
        LOGE("没有找到媒体信息");
        return;
    }


    if (currentAudioDecoder != nullptr) {
#ifdef DEBUGAPP
        LOGD("音频解码器数量:%d,", audioDecoders.size());
//        currentAudioDecoder->startDecode();
//        audioRenderer = new BugMediaSLESAudioRenderer(getAudioFrameData, this);
//        audioRenderer->render();
#else

        currentAudioDecoder->startDecode();
        audioRenderer = new BugMediaSLESAudioRenderer(getAudioFrameData, this);
        audioRenderer->render();
#endif
    }

    if (currentVideoDecoder != nullptr) {
#ifdef DEBUGAPP
        LOGD("视频解码器数量:%d", videoDecoders.size());
        currentVideoDecoder->startDecode();

        javaVm->AttachCurrentThread(&env, nullptr);

        videoRenderer = new BugMediaGLESVideoRenderer(getVideoFrameData, getAudioPtsData, this,
                                                      env, surface, width, height, createPBufferSurface);
        videoRenderer->render();
#else
        currentVideoDecoder->startDecode();
        videoRenderer = new BugMediaGLESVideoRenderer(getVideoFrameData, getAudioPtsData, this,
                                                      env, surface, width, height, createPBufferSurface);
        videoRenderer->render();
#endif
    }

    loaded = true;

}


void BugMediaPlayer::setBufferSize(int size) {
    maxBufferSize = size;
}

void BugMediaPlayer::load() {

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&loadThread, &attr, loadThreadFunc, this);
}

void BugMediaPlayer::switchAudioChannel(int ch) {
    currentAudioDecoder = audioDecoders[ch];

}

void BugMediaPlayer::switchVideoChannel(int ch) {
    currentVideoDecoder = videoDecoders[ch];

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

BugMediaVideoFrame *BugMediaPlayer::getVideoFrameData(void *ctx) {
    auto loader = (BugMediaPlayer *) ctx;
    return loader->getVideoFrame();
}

int64_t BugMediaPlayer::getAudioPtsData(void *ctx) {
    auto loader = (BugMediaPlayer *) ctx;
    return loader->getAudioPts();
}

void BugMediaPlayer::play() {
    if (loaded) {
        audioRenderer->play();
        videoRenderer->play();
    }

}

void BugMediaPlayer::pause() {
    if (loaded) {
        audioRenderer->pause();
        videoRenderer->pause();
    }


}

void BugMediaPlayer::stop() {
    if (loaded) {
        audioRenderer->pause();
        videoRenderer->pause();
    }

}

void BugMediaPlayer::destroy() {
    release();
}


void BugMediaPlayer::resizeView(GLint x, GLint y, GLsizei width, GLsizei height) {
    if (loaded) {
        videoRenderer->resizeView(x, y, width, height);
    }

}

void *BugMediaPlayer::sortThreadRoutine(void *pVoid) {
    auto self = (BugMediaPlayer *) pVoid;
    self->sortFrame();
    return nullptr;
}

void BugMediaPlayer::sortFrame() {
//    while (true) {
//        if (streamEnd){
//            break;
//        }
//        sem_wait(&canTake);
//        BugMediaCacheFrame cacheFrame = cacheFrameQueue.front();
//        if (cacheFrame.type==AVMEDIA_TYPE_VIDEO){
//
//        } else if (cacheFrame.type==AVMEDIA_TYPE_AUDIO){
//
//        }
//    }
}













