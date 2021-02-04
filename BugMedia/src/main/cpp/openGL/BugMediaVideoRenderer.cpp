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
        if (currentState==PLAYING){
            render();
        } else if (currentState==PAUSE){

        }else{
            break;
        }
    }

    //
    // 释放资源，EGL资源不能跨线程释放所在在这里释放
    //
}

BugMediaVideoRenderer::BugMediaVideoRenderer(BugMediaVideoLoader *decoder) {
    videoDecoder = decoder;
    currentState = STOP;
}

BugMediaVideoRenderer::~BugMediaVideoRenderer() {
    delete videoDecoder;

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
