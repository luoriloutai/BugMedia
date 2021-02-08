//
// Created by Gshine on 2021/1/31.
//

#include "BugMediaVideoRenderer.h"

void BugMediaVideoRenderer::setShaderSource() {


}

void BugMediaVideoRenderer::startDraw() {
    prepare();
    //videoLoader->getVideoFrame();
    //videoLoader->getAudioFrame();

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

void BugMediaVideoRenderer::render() {
    BugMediaVideoFrame *frame = videoLoader->getVideoFrame();


//    if (getFrameData != nullptr) {
//
//       int ret = getFrameData(data, &width,&height,this);
//       if(ret!=0){
//           LOGE("获取视频帧数据错误");
//       }
//    }
}



