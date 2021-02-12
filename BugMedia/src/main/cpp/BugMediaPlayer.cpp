//
// Created by Gshine on 2021/2/12.
//

#include "BugMediaPlayer.h"

BugMediaPlayer::~BugMediaPlayer() {
    delete audioRenderer;
    audioRenderer = nullptr;
    delete videoRenderer;
    videoRenderer = nullptr;
    delete loader;
    loader = nullptr;
}

BugMediaPlayer::BugMediaPlayer(const char *url, JNIEnv *env, jobject surface, EGLint width, EGLint height) {

    // 创建渲染器
    audioRenderer = new BugMediaSLESAudioRenderer(getAudioFrameCallback, this);
    videoRenderer = new BugMediaGLESVideoRenderer(nullptr, env, surface, width, height);

    // 播放、暂停等由VideoLoader来控制，渲染器无法得知何时执行操作
    loader = new BugMediaVideoLoader(url, audioRenderer, videoRenderer);

}

BugMediaAudioFrame *BugMediaPlayer::getAudioFrameCallback(void *ctx) {
    auto self = (BugMediaPlayer *) ctx;
    return self->getAudioFrame();
}

BugMediaAudioFrame *BugMediaPlayer::getAudioFrame() {
    return loader->getAudioFrame();
}

BugMediaVideoFrame *BugMediaPlayer::getVideoFrameCallback(void *ctx) {
    auto self = (BugMediaPlayer *)ctx;
    return self->getVideoFrame();
}

BugMediaVideoFrame *BugMediaPlayer::getVideoFrame() {
    return loader->getVideoFrame();
}

void BugMediaPlayer::play() {
    loader->play();
}

void BugMediaPlayer::pause() {
    loader->pause();
}

void BugMediaPlayer::stop() {
    loader->stop();
}

void BugMediaPlayer::load() {
    // 加载文件，解码，解码有数据时渲染开始
    loader->load();
}


