//
// Created by Gshine on 2021/2/7.
//

#include "BugMediaPlayer.h"

BugMediaPlayer::BugMediaPlayer(const char *url) {
    this->videoLoader = new BugMediaVideoLoader(url);
    videoLoader->load();

    this->videoRenderer = new BugMediaVideoRenderer(videoLoader);
    videoRenderer->render();

    this->audioRenderer = new BugMediaAudioRenderer(videoLoader);
}

BugMediaPlayer::~BugMediaPlayer() {
    delete audioRenderer;
    delete videoRenderer;
    delete videoLoader;
}

void BugMediaPlayer::play() {
    audioRenderer->play();
    videoRenderer->play();
}

void BugMediaPlayer::pause() {
    audioRenderer->pause();
    videoRenderer->pause();
}

void BugMediaPlayer::stop() {
    audioRenderer->stop();
    videoRenderer->stop();
}