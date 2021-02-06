//
// Created by Gshine on 2021/1/31.
//

#include "BugMediaVideoRenderer.h"

void BugMediaVideoRenderer::setShaderSource() {


}

void BugMediaVideoRenderer::startDraw() {
    prepare();

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
    // 释放资源，EGL资源不能跨线程释放所在在这里释放
    //
    release();
}

BugMediaVideoRenderer::BugMediaVideoRenderer(BugMediaVideoLoader *loader) {
    videoLoader = loader;
    currentState = STOP;
    callback = new BugMediaStateChangedCallback(this);
    loader->stateChangedCallback = callback;
    //
    // 这里用了另外一个类为回调函数赋值，因为这里涉及到循环引用，所以
    // 引入了第三个类

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



