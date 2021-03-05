//
// Created by Gshine on 2021/2/4.
//


#include "BugMediaPlayer.h"
#include "BugMediaCommon.h"

#define DEBUGAPP

BugMediaPlayer::BugMediaPlayer(const char *url, int decoderBufferSize, JNIEnv *env,
                               jobject surface, EGLint width, EGLint height) {

    // 由于JNIEnv是与线程关联的，不同线程之间不能互相访问
    // 而程序内创建渲染器是在另一个线程中进行的
    // 所以用到JNIEnv作为参数时不能直接用
    // 可以获取JavaVM，它是线程共享的
    // 用它可以为线程分配一个JNIEnv。
    // 当跨线程时使用。
    env->GetJavaVM(&javaVm); // 暂时没用，直接创建并使用了ANativeWindow

    // 全局引用一个java对象，在其他线程也可以用
    this->surface = env->NewGlobalRef(surface); // 暂时没用，直接创建并使用了ANativeWindow

    maxBufferSize = decoderBufferSize;
    this->url = url;
    this->width = width;
    this->height = height;


    audioDecoder = new BugMediaFFmpegDecoder(url, maxBufferSize, AVMEDIA_TYPE_AUDIO);
    videoDecoder = new BugMediaFFmpegDecoder(url, maxBufferSize, AVMEDIA_TYPE_VIDEO);

    audioRenderer = new BugMediaSLESAudioRenderer(getAudioFrameCallback, this);

    nativeWindow = ANativeWindow_fromSurface(env, surface);
    videoRenderer = new BugMediaVideoRenderer(nativeWindow, getVideoFrameCallback, getAudioPtsCallback, this);


}

BugMediaPlayer::BugMediaPlayer(const char *url, int bufferSize, JNIEnv *env, jobject surface)
        : BugMediaPlayer(url, bufferSize, env, surface, 0, 0) {

}

BugMediaPlayer::~BugMediaPlayer() {
    if (!isRelease) {
        release();
    }

}

void BugMediaPlayer::release() {
    if (!isRelease) {
        isRelease = true;

        delete audioDecoder;
        audioRenderer = nullptr;
        delete videoDecoder;
        videoRenderer = nullptr;

        delete audioRenderer;
        audioRenderer = nullptr;
        delete videoRenderer;
        videoRenderer = nullptr;

        ANativeWindow_release(nativeWindow);
        JNIEnv *jniEnv;
        javaVm->AttachCurrentThread(&jniEnv, nullptr);
        jniEnv->DeleteGlobalRef(surface);

        // 下面两句不可调用，会崩溃
        //javaVm->DetachCurrentThread();
        //javaVm->DestroyJavaVM();
    }

}

void BugMediaPlayer::setBufferSize(int size) {
    maxBufferSize = size;
}

void BugMediaPlayer::load() {

    audioDecoder->startDecode();
    videoDecoder->startDecode();

    loaded = true;

    audioRenderer->render();
    videoRenderer->render();

}

void BugMediaPlayer::switchAudioChannel(int ch) {
    //currentAudioDecoder = audioDecoders[ch];

}

void BugMediaPlayer::switchVideoChannel(int ch) {
    //currentVideoDecoder = videoDecoders[ch];

}


int64_t BugMediaPlayer::getAudioPts() const {
    return audioPts;
}


BugMediaAudioFrame *BugMediaPlayer::getAudioFrame() {

    auto frame = audioDecoder->getAudioFrame();
    audioPts = frame->pts;
    if (!frame->isEnd) {
        isAudioEnd = true;
    }

    return frame;

}

BugMediaVideoFrame *BugMediaPlayer::getVideoFrame() {

    auto frame = videoDecoder->getVideoFrame();
    if (!frame->isEnd) {
        isVideoEnd = true;
    }
    return frame;

}

BugMediaAudioFrame *BugMediaPlayer::getAudioFrameCallback(void *ctx) {

    auto loader = (BugMediaPlayer *) ctx;
    return loader->getAudioFrame();
}

BugMediaVideoFrame *BugMediaPlayer::getVideoFrameCallback(void *ctx) {
    auto loader = (BugMediaPlayer *) ctx;
    return loader->getVideoFrame();
}

int64_t BugMediaPlayer::getAudioPtsCallback(void *ctx) {
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
        audioRenderer->stop();
        videoRenderer->stop();
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

















