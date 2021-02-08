//
// Created by Gshine on 2021/2/7.
//

#include "BugMediaAudioRenderer.h"

void BugMediaAudioRenderer::play() {
    currentState = PLAYING;
}

void BugMediaAudioRenderer::pause() {
    currentState = PAUSE;
}

void BugMediaAudioRenderer::stop() {
    currentState = STOP;
}

BugMediaAudioRenderer::BugMediaAudioRenderer(BugMediaVideoLoader *loader) {
    this->videoLoader = loader;
}
