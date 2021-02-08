//
// Created by Gshine on 2021/1/31.
//

#include "BugMediaVideoRenderer.h"

void BugMediaVideoRenderer::setShaderSource() {


}

void BugMediaVideoRenderer::onRender() {
    prepare();

    //
    // 不断读取数据进行渲染
    while (true) {
        if (currentState == PLAYING) {
            if (startTimeMs == -1) {
                startTimeMs = getCurMsTime();
            }

            if (renderOnce()){
                break;
            }

        } else if (currentState == PAUSE) {
            startTimeMs = startTimeMs + videoPst;
            //startTimeMs = getCurMsTime()-videoPst;

        } else if (currentState == STOP) {
            break;
        }
    }

    //
    // 释放资源，EGL资源不能跨线程释放，所以在这里释放
    //
    release();
}

BugMediaVideoRenderer::BugMediaVideoRenderer(BugMediaVideoLoader *loader) {
    currentState = STOP;
    videoLoader = loader;
}

BugMediaVideoRenderer::~BugMediaVideoRenderer() {


}

void BugMediaVideoRenderer::play() {
    currentState = PLAYING;
}

void BugMediaVideoRenderer::pause() {
    currentState = PAUSE;

}

void BugMediaVideoRenderer::stop() {
    currentState = STOP;
}

void BugMediaVideoRenderer::prepare() {

}

bool BugMediaVideoRenderer::renderOnce() {
    BugMediaVideoFrame *frame = videoLoader->getVideoFrame();
    audioPts = videoLoader->getAudioPts();
    if (frame->isEnd) {
        currentState = STOP;
        return true;
    }



    return false;
}



