//
// Created by Gshine on 2021/3/3.
//

#include "BugMediaVideoRenderer.h"

BugMediaVideoRenderer::BugMediaVideoRenderer(ANativeWindow *window) : BugMediaGLES(window) {

}

BugMediaVideoRenderer::~BugMediaVideoRenderer() {
    if (currentState == PAUSE) {
        currentState = STOP;
        sem_post(&playSem);
    }
    sem_destroy(&playSem);
}

const GLchar *BugMediaVideoRenderer::getVertexShaderSource() {
    const GLchar *vertexShaderSource = "attribute vec4 position; \n"
                                       "attribute vec2 texcoord; \n"
                                       "varying vec2 v_texcoord; \n"
                                       "void main(void) \n"
                                       "{ \n"
                                       " gl_Position =  position; \n"
                                       " v_texcoord = texcoord; \n"
                                       "} \n";
    return vertexShaderSource;
}

const GLchar *BugMediaVideoRenderer::getFragmentShaderSource() {
    const GLchar *fragmentShaderSource = "precision highp float; \n"
                                         "varying highp vec2 v_texcoord; \n"
                                         "uniform sampler2D texSampler; \n"
                                         "void main() { \n"
                                         " gl_FragColor = texture2D(texSampler, v_texcoord); \n"
                                         "} \n";
    return fragmentShaderSource;
}

void BugMediaVideoRenderer::onRender() {
    prepare();

    while (true) {
        sem_wait(&playSem);
        if (currentState == PLAYING) {
            if (startTimeMs == -1) {
                startTimeMs = getCurMsTime();
            }

            if (!renderOnce()) {
                break;
            }

        } else if (currentState == PAUSE) {
            //startTimeMs = startTimeMs + videoPts;
            startTimeMs = getCurMsTime() - videoPts;
            sem_wait(&playSem);

        } else if (currentState == STOP) {
            break;
        }
    }

    //
    // 释放资源，EGL资源不能跨线程释放，所以析构函数里不能调用释放方法
    //

    release();
}

void BugMediaVideoRenderer::release() {

}

void BugMediaVideoRenderer::prepare() {
    // 顶点坐标，以物体中心为原点
    GLfloat vertices[] = {-1.0f, -1.0f,
                          1.0f, -1.0f,
                          -1.0f, 1.0f,
                          1.0f, 1.0f};

    // 纹理与屏幕坐标都以顶点坐标为基准进行转换
    // 把相对应的顶点坐标转换成纹理或屏幕坐标
    // 就产生下面两个坐标

    // 纹理坐标，以物体左下角为原点，将顶点转成如下坐标
    // 不直接在屏幕上绘制时使用，例如离屏渲染
    GLfloat texCoords[] = {0.0f, 0.0f,
                           1.0f, 0.0f,
                           0.0f, 1.0f,
                           1.0f, 1.0f};

    // 屏幕坐标，以屏幕或物体左上角为原点，将顶点转为如下坐标
    GLfloat screenCoords[] = {0.0f, 1.0f,
                              1.0f, 1.0f,
                              0.0f, 0.0f,
                              1.0f, 0.0f};


}

GLboolean BugMediaVideoRenderer::renderOnce() {

    // 获取帧

    if (getVideoFrame == nullptr) {
        return GL_FALSE;
    }
    auto *frame = getVideoFrame(callbackContext);
    if (frame == nullptr) {
        return GL_FALSE;
    }

    if (frame->isEnd) {
        currentState = STOP;
        return GL_FALSE; // 结束了不需要进行下一轮渲染
    }

    videoPts = frame->pts;

    if (getAudioPts != nullptr) {
        audioPts = getAudioPts(callbackContext);
    }


    // 控制渲染时机

    int64_t pass = getCurMsTime() - startTimeMs;
    delay = frame->pts - pass;
    //delay = getCurMsTime()-frame->pts-startTimeMs-passtimeMs;

    // pts比当前时间大，说明要等待时间到
    if (delay > 0) {
#ifdef DEBUGAPP
        LOGD("延时时长：%lld", delay);
#endif
        av_usleep(delay * 1000);  // 微秒,delay是毫秒
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame->width, frame->height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, frame->data);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    swapDefaultBuffers();

    sem_post(&playSem);

    // 绘制完毕后释放帧资源
    delete frame;

    return GL_TRUE;
}

void BugMediaVideoRenderer::play() {
    if (currentState != PLAYING) {
        currentState = PLAYING;
        sem_post(&playSem);
    }
}

void BugMediaVideoRenderer::pause() {
    if (currentState != PAUSE) {
        currentState = PAUSE;
    }
}

void BugMediaVideoRenderer::stop() {
    currentState = STOP;
}


