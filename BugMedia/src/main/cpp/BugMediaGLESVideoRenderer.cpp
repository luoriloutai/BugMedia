//
// Created by Gshine on 2021/1/31.
//


#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "BugMediaGLESVideoRenderer.h"
#include "openGL/BugMediaRendererCommon.h"



#define DEBUGAPP


void BugMediaGLESVideoRenderer::setShaderSource() {


//    const char *vertexShaderSource = "attribute vec2 position; \n"
//                                     "attribute vec2 texcoord; \n"
//                                     //"uniform mat4 view;\n"
//                                     "varying vec2 v_texcoord; \n"
//                                     "void main(void) \n"
//                                     "{ \n"
//                                     //" gl_Position = view * vec4(position,0,1); \n"
//                                     " gl_Position = vec4(position,0,1); \n"
//                                     " v_texcoord = texcoord; \n"
//                                     "} \n";
//
//    const char *fragmentShaderSource = "precision highp float; \n"
//                                       "varying highp vec2 v_texcoord; \n"
//                                       "uniform sampler2D texSampler; \n"
//                                       "void main() { \n"
//                                       " gl_FragColor = texture2D(texSampler, v_texcoord); \n"
//                                       "} \n";
//
//    setShaderSources(vertexShaderSource, fragmentShaderSource);
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
                                                     JNIEnv *env, jobject jSurface, EGLint width, EGLint height,
                                                     bool createPBufferSurface) {
    env->GetJavaVM(&javaVm);
    surface = env->NewGlobalRef(jSurface);
    currentState = UNSTART;
    this->getVideoFrame = getVideoFrameCallback;
    this->getAudioPts = getAudioPtsCallback;
    this->callbackContext = callbackContext;
    this->width = width;
    this->height = height;
    this->createPBufferSurface = createPBufferSurface;
    sem_init(&playSem, 0, 0);

    setWindowSurface(javaVm,surface);

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
EGLDisplay display;
EGLSurface windowSurface;
// 相对固定的东西放在这里
void BugMediaGLESVideoRenderer::prepare() {
    JNIEnv *env;
    javaVm->AttachCurrentThread(&env, nullptr);
/* ========
    * EGL 相关
    * ========*/
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("create display failed:%d", eglGetError());
        return ;
    }
    if (!eglInitialize(display, nullptr, nullptr)) {
        LOGE("initialize display failed:%d", eglGetError());
        return ;
    }
    const EGLint configAttribs[] = {EGL_RED_SIZE, 8,
                                    EGL_GREEN_SIZE, 8,
                                    EGL_BLUE_SIZE, 8,
                                    EGL_ALPHA_SIZE, 8,
                                    EGL_BUFFER_SIZE, 32,
                                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                    EGL_NONE}; // 必须以EGL_NONE结尾

    EGLint numConfigs = 0;
    EGLConfig config;

    // 选择符合配置选项的config
    // config_size:选择符合条件的多少个config输出到传入的参数Config中
    // numConfigs:符合条件的配置有多少个，输出到numConfigs
    // 第三个参数可以不传，这时numConfigs的值就是找到的符合条件的eglconfig的个数，
    // 然后，再将numConfigs传到第四个参数，同时传一个接收eglconfig的数组到第三个参数，可以获取所有符合条件的eglconfig
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig() error:%d", eglGetError());
        return ;
    }
    EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};
    EGLContext context;
    if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT,
                                     contextAttribs))) {
        LOGE("eglCreateContext() error:%d", eglGetError());
        return ;
    }

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    EGLint format;
    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
        return ;
    }
    ANativeWindow_setBuffersGeometry(nativeWindow, 0, 0, format);
    //EGLSurface windowSurface;
    if (!(windowSurface = eglCreateWindowSurface(display, config, nativeWindow, nullptr))) {
        LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
    }

    // 绑定到当前线程，opengl才可以操作设备
    if (!eglMakeCurrent(display, windowSurface, windowSurface, context)) {
        LOGE("eglMakeCurrent() error:%d\n", eglGetError());
        return ;
    }

    /* ===============
     * OpenGL ES 相关
     * ===============*/

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

    const GLchar *vertexShaderSource = "attribute vec4 position; \n"
                                       "attribute vec2 texcoord; \n"
                                       "varying vec2 v_texcoord; \n"
                                       "void main(void) \n"
                                       "{ \n"
                                       " gl_Position =  position; \n"
                                       " v_texcoord = texcoord; \n"
                                       "} \n";
    const GLchar *fragmentShaderSource = "precision highp float; \n"
                                         "varying highp vec2 v_texcoord; \n"
                                         "uniform sampler2D texSampler; \n"
                                         "void main() { \n"
                                         " gl_FragColor = texture2D(texSampler, v_texcoord); \n"
                                         "} \n";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    GLint compiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == 0) {
        glDeleteShader(vertexShader);
        LOGE("vertex shader compile failed");
        return ;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    compiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == 0) {
        glDeleteShader(fragmentShader);
        LOGE("fragment shader compile failed.");
        return ;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    int error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        LOGE("attach vertex shader error:%d", error);
        return ;
    }
    error = GL_NO_ERROR;
    glAttachShader(program, fragmentShader);
    if ((error = glGetError()) != GL_NO_ERROR) {
        LOGE("attach fragment shader error:%d", error);
        return ;
    }
    glLinkProgram(program);
    GLint linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        GLchar logInfo[len];
        glGetProgramInfoLog(program, len, nullptr, logInfo);
        LOGE("Could not link program: %s", logInfo);

        glDeleteProgram(program);
        program = 0;
        LOGE("link program falied");
        return ;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(program);
    GLuint positionLoc = glGetAttribLocation(program, "position");
    GLuint texcoordLoc = glGetAttribLocation(program, "texcoord");
    GLuint texSamplerLoc = glGetUniformLocation(program, "texSampler");

    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, GL_FALSE, 0, screenCoords);
    glEnableVertexAttribArray(texcoordLoc);

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0); // 纹理单元 texture0 默认是激活的,这句代码可以不要
    glBindTexture(GL_TEXTURE_2D, texture); // 绑定之前需要先激活纹理单元
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 缩小参数设置
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大参数设置
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 纹理S方向的参数，拉伸、重复等
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // 纹理T方向的参数
    glUniform1i(texSamplerLoc, 0); // 指明采样器连接到哪个纹理单元，从该纹理单元采样


//    // ==============
//    // 顶点与纹理坐标
//    // ==============
//
//    // 顶点坐标，以物体中心为原点
//    GLfloat vertices[] = {-1.0f, -1.0f,
//                          1.0f, -1.0f,
//                          -1.0f, 1.0f,
//                          1.0f, 1.0f};
//
//    // 顶点是以物体中心为原点的，这里取屏幕中心作为原点，
//    // 取了四个角的点坐标放在数组中。顶点是用来绘图形的，
//    // 简单的图形直接绘制，但复杂的效果不好直接绘制，比如
//    // 绘制一张图片，做起来就相当费劲了。
//    //
//    // 如果把一张图片直接贴在图形上，不就大大简化了绘制的
//    // 过程了吗。这种贴在图形上面的图像就叫做纹理。
//    //
//    // 要把图片即纹理贴到图形上，但是纹理坐标不是取屏幕中心为
//    // 原点，如果直接把纹理贴到图形上有可能会出现纹理上下颠倒
//    // 等各种奇怪现象。
//    //
//    // 这就需要转换，把顶点的各个坐标“翻译”成对应的纹理坐标。
//    // 比如顶点坐标的屏幕左上角是(-1,1),那么它对应的纹理坐
//    // 标就是(0,1)；如果转成屏幕坐标就是(0,0)。这个过程或者
//    // 反过来说是将纹理坐标对应到顶点坐标，这样纹理贴到图形
//    // 上时就不会出现上下颠倒等情况。
//
//
//    // 纹理坐标，以物体左下角为原点，将顶点转成如下坐标
//    GLfloat texCoords[] = {0.0f, 0.0f,
//                           1.0f, 0.0f,
//                           0.0f, 1.0f,
//                           1.0f, 1.0f};
//
//    // 屏幕坐标，以屏幕或物体左上角为原点，将顶点转为如下坐标
//    GLfloat screenCoords[] = {0.0f, 1.0f,
//                              1.0f, 1.0f,
//                              0.0f, 0.0f,
//                              1.0f, 0.0f};
//
//    GLsizeiptr vertexSize = sizeof(vertices);
//    GLsizeiptr eleSize = sizeof(GLfloat);
//    // 顶点维度、顶点坐标数组大小（字节）、坐标数组每个元素的大小（字节）
//    configVertex(2, vertexSize, eleSize);
//
//    GLuint positionLoc = glGetAttribLocation(getProgram(), "position");
//    GLuint texcoordLoc = glGetAttribLocation(getProgram(), "texcoord");
//    GLuint texSamplerLoc = glGetUniformLocation(getProgram(), "texSampler");
//
//    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
//    glEnableVertexAttribArray(positionLoc);
//    glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, GL_FALSE, 0, screenCoords);
//    glEnableVertexAttribArray(texcoordLoc);
//
//
//    glGenTextures(1, &texId);
//    glActiveTexture(GL_TEXTURE0); // 纹理单元 texture0 默认是激活的,这句代码可以不要
//    glBindTexture(GL_TEXTURE_2D, texId); // 绑定之前需要先激活纹理单元
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 缩小参数设置
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大参数设置
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 纹理S方向的参数，拉伸、重复等
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // 纹理T方向的参数
//    glUniform1i(texSamplerLoc, 0); // 指明采样器连接到哪个纹理单元，从该纹理单元采样
//


    // ==========
    // 纹理操作
    // ==========

    // 用图像数据创建2D纹理
    //create2DTexture0(&texId);

//    glGenTextures(1, &texId);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texId);
//
//    //
//    //设置纹理参数
//    //
//    // 过滤器设置。缩小设置
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    // 过滤器设置。放大设置
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    // 对边界的约束。S表示横向，T表示纵向，纹理坐标以左下角为原点
//    // GL_CLAMP_TO_EDGE表示将值控制在一定范围内，大于1设置为1，小于0设为0
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//    GLuint samplerId = glGetUniformLocation(getProgram(), "texSampler");
//    glUniform1i(samplerId, 0);

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
    //useProgram();

//    if(frame->width % 16 != 0){
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    }
    //set2DTexture0ToShader("texSampler", texId, frame->data, frame->width, frame->height);


//    //
//    // 缩放尺寸使图像不变形且居中
//    //
//    EGLint viewWidth = getViewWidth();
//    EGLint viewHeight = getViewHeight();
//    scaleCenter(viewWidth, viewHeight, frame->width, frame->height);

//    //
//    // 变换，未实现
//    //
//
//    // 预留一个单位矩阵，作为着色器中量的初始化，待添加对图像的空间变换等操作
//    glm::mat4 viewMat(1.0f); // 注：初始化一个单位矩阵,需要传一个参数
//
//    // 2D图像，创建正交投影即可。目前没用，先预留
//    //glm::mat4 projection;
//    GLuint viewLoc = glGetUniformLocation(getProgram(), "view");
//    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame->width, frame->height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, frame->data);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    eglSwapBuffers(display,windowSurface);

//    //
//    // 绘制
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
//    swapBuffers();

    sem_post(&playSem);

    // 绘制完毕后释放帧资源
    delete frame;

    return false;
}

void BugMediaGLESVideoRenderer::render() {
    BugMediaGraphics::render();
    rendering = true;
}







