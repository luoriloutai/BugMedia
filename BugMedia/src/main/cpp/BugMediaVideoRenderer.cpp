//
// Created by Gshine on 2021/1/31.
//

#include "BugMediaVideoRenderer.h"

void BugMediaVideoRenderer::setShaderSource() {


}

void BugMediaVideoRenderer::startDraw() {
    prepare();
    //videoLoader->getVideoFrame();

    //
    // 不断读取数据进行渲染
    while (true) {
        if (currentState == PLAYING) {
            render();
        } else if (currentState == PAUSE) {

        } else {
            break;
        }
    }

    //
    // 释放资源，EGL资源不能跨线程释放，所以在这里释放
    //
    release();
}

BugMediaVideoRenderer::BugMediaVideoRenderer(const char * url) {
    currentState = STOP;
    videoLoader = new BugMediaVideoLoader(url);
    //videoLoader->setBufferSize(500);
    videoLoader->load();

    callback = new BugMediaStateChangedCallback(this);
    //loader->stateChangedCallback = callback;


}

BugMediaVideoRenderer::~BugMediaVideoRenderer() {
    delete videoLoader;
    delete callback;

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

void BugMediaVideoRenderer::render() {

}



