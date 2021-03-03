//
// Created by Gshine on 2021/1/31.
//


#include "BugMediaGLESVideoRenderer.h"
#include "openGL/BugMediaRendererCommon.h"



#define DEBUGAPP


void BugMediaGLESVideoRenderer::setShaderSource() {


    const char *vertexShaderSource = "attribute vec2 position; \n"
                                     "attribute vec2 texcoord; \n"
                                     "uniform mat4 view;\n"
                                     "varying vec2 v_texcoord; \n"
                                     "void main(void) \n"
                                     "{ \n"
                                     " gl_Position = view * vec4(position,0,1); \n"
                                     " v_texcoord = texcoord; \n"
                                     "} \n";

    const char *fragmentShaderSource = "precision highp float; \n"
                                       "varying highp vec2 v_texcoord; \n"
                                       "uniform sampler2D texSampler; \n"
                                       "void main() { \n"
                                       " gl_FragColor = texture2D(texSampler, v_texcoord); \n"
                                       "} \n";

    setShaderSources(vertexShaderSource, fragmentShaderSource);
}

void BugMediaGLESVideoRenderer::onRender() {
    prepare();

    //
    // 不断读取数据进行渲染
    while (true) {
        sem_wait(&playSem);
        if (currentState == PLAYING) {
            if (startTimeMs == -1) {
                startTimeMs = getCurMsTime();
            }

            if (renderOnce()) {

#ifdef DEBUGAPP
                LOGD("视频播放跳出");
#endif

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
    unbind2DTexture0(&texId);
    release();
}

BugMediaGLESVideoRenderer::BugMediaGLESVideoRenderer(GetVideoFrameCallback getVideoFrameCallback,
                                                     GetAudioPtsCallback getAudioPtsCallback, void *callbackContext,
                                                     JNIEnv *env, jobject surface, EGLint width, EGLint height,
                                                     bool createPBufferSurface) {
    currentState = UNSTART;
    this->getVideoFrame = getVideoFrameCallback;
    this->getAudioPts = getAudioPtsCallback;
    this->callbackContext = callbackContext;
    this->env = env;
    this->surface = surface;
    this->width = width;
    this->height = height;
    this->createPBufferSurface = createPBufferSurface;
    sem_init(&playSem, 0, 0);

    setWindowSurface(env,surface);

}


BugMediaGLESVideoRenderer::~BugMediaGLESVideoRenderer() {
    if (currentState == PAUSE) {
        currentState = STOP;
        sem_post(&playSem);
    }
    sem_destroy(&playSem);

}

void BugMediaGLESVideoRenderer::play() {
    if (rendering&&currentState!=PLAYING) {
        currentState = PLAYING;
        sem_post(&playSem);
    }

}

void BugMediaGLESVideoRenderer::pause() {
    if (rendering&&currentState!=PAUSE) {
        currentState = PAUSE;
    }


}

void BugMediaGLESVideoRenderer::stop() {
    if (rendering) {
        currentState = STOP;
    }

}

// 相对固定的东西放在这里
void BugMediaGLESVideoRenderer::prepare() {

    // ==============
    // 顶点与纹理坐标
    // ==============

    // 顶点坐标，以物体中心为原点
    GLfloat vertices[] = {-1.0f, -1.0f,
                          1.0f, -1.0f,
                          -1.0f, 1.0f,
                          1.0f, 1.0f};

    // 顶点是以物体中心为原点的，这里取屏幕中心作为原点，
    // 取了四个角的点坐标放在数组中。顶点是用来绘图形的，
    // 简单的图形直接绘制，但复杂的效果不好直接绘制，比如
    // 绘制一张图片，做起来就相当费劲了。
    //
    // 如果把一张图片直接贴在图形上，不就大大简化了绘制的
    // 过程了吗。这种贴在图形上面的图像就叫做纹理。
    //
    // 要把图片即纹理贴到图形上，但是纹理坐标不是取屏幕中心为
    // 原点，如果直接把纹理贴到图形上有可能会出现纹理上下颠倒
    // 等各种奇怪现象。
    //
    // 这就需要转换，把顶点的各个坐标“翻译”成对应的纹理坐标。
    // 比如顶点坐标的屏幕左上角是(-1,1),那么它对应的纹理坐
    // 标就是(0,1)；如果转成屏幕坐标就是(0,0)。这个过程或者
    // 反过来说是将纹理坐标对应到顶点坐标，这样纹理贴到图形
    // 上时就不会出现上下颠倒等情况。


    // 纹理坐标，以物体左下角为原点，将顶点转成如下坐标
    GLfloat texCoords[] = {0.0f, 0.0f,
                           1.0f, 0.0f,
                           0.0f, 1.0f,
                           1.0f, 1.0f};

    // 屏幕坐标，以屏幕或物体左上角为原点，将顶点转为如下坐标
    GLfloat screenCoords[] = {0.0f, 1.0f,
                              1.0f, 1.0f,
                              0.0f, 0.0f,
                              1.0f, 0.0f};

    GLsizeiptr vertexSize = sizeof(vertices);
    GLsizeiptr eleSize = sizeof(GLfloat);
    // 顶点维度、顶点坐标数组大小（字节）、坐标数组每个元素的大小（字节）
    configVertex(2, vertexSize, eleSize);

    // 属性在着色器中是4维的，但是这里用数组并指定成二维也可以转成着色器需要的数据,
    // 猜测会根据给定的维度大小去取数据，这里只有二维，后二维估计使用默认值，与现在着色器效果一样
    setVertexAttribArray("position", 2, GL_FLOAT, GL_FALSE, 0, vertices);
    // 要显示在屏幕上，所以用屏幕坐标
    setVertexAttribArray("texcoord", 2, GL_FLOAT, GL_FALSE, 0, screenCoords);

#ifdef DEBUGAPP
    LOGD("顶点属性设置完毕");
#endif


    // ==========
    // 纹理操作
    // ==========

    // 用图像数据创建2D纹理
    create2DTexture0(&texId);

    //
    //设置纹理参数
    //
    // 过滤器设置。缩小设置
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // 过滤器设置。放大设置
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 对边界的约束。S表示横向，T表示纵向，纹理坐标以左下角为原点
    // GL_CLAMP_TO_EDGE表示将值控制在一定范围内，大于1设置为1，小于0设为0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


}

bool BugMediaGLESVideoRenderer::renderOnce() {
    //
    // 获取帧
    //
    if (getVideoFrame == nullptr) {
        return true;
    }
    auto *frame = getVideoFrame(callbackContext);
    if (frame == nullptr) {
        return true;
    }

    if (frame->isEnd) {

#ifdef DEBUGAPP
        LOGD("最后一帧视频");
#endif

        currentState = STOP;
        return true;
    }

    videoPts = frame->pts;

    if (getAudioPts != nullptr) {
        audioPts = getAudioPts(callbackContext);
    }

    //更新一个unform之前你必须先使用程序（调用glUseProgram)
    // 每次更新设置后都应调用
    useProgram();

//    if(frame->width % 16 != 0){
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    }
    set2DTexture0ToShader("texSampler", texId, frame->data, frame->width, frame->height);


    //
    // 缩放尺寸使图像不变形且居中
    //
    EGLint viewWidth = getViewWidth();
    EGLint viewHeight = getViewHeight();
    scaleCenter(viewWidth, viewHeight, frame->width, frame->height);

    //
    // 变换，未实现
    //

    // 预留一个单位矩阵，作为着色器中量的初始化，待添加对图像的空间变换等操作
    glm::mat4 viewMat(1.0f); // 注：初始化一个单位矩阵,需要传一个参数

    // 2D图像，创建正交投影即可。目前没用，先预留
    //glm::mat4 projection;
    GLuint viewLoc = glGetUniformLocation(getProgram(), "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

    //
    // 控制渲染时机
    //
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


    //
    // 绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
    swapBuffers();

    sem_post(&playSem);

    // 绘制完毕后释放帧资源
    delete frame;

    return false;
}

void BugMediaGLESVideoRenderer::render() {
    BugMediaGraphics::render();
    rendering = true;
}







