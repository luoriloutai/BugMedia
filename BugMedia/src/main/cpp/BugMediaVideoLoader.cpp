//
// Created by Gshine on 2021/2/4.
//


#include "BugMediaVideoLoader.h"
#include "BugMediaCommon.h"
#include "BugMediaFFmpegAudioDecoder.h"
#include "BugMediaFFmpegVideoDecoder.h"

#define DEBUGAPP

BugMediaVideoLoader::BugMediaVideoLoader(const char *url, BugMediaSLESAudioRenderer *audioRenderer,
                                         BugMediaGLESVideoRenderer *videoRenderer, int bufferSize) {
    decoderBufferSize = bufferSize;
    this->url = url;
    this->audioRenderer = audioRenderer;
    this->videoRenderer = videoRenderer;
}

BugMediaVideoLoader::~BugMediaVideoLoader() {
    if (!isRelease) {
        release();
    }

}

void BugMediaVideoLoader::release() {
    if (!isRelease) {
        isRelease = true;
        if (formatContext != nullptr) {
            avformat_close_input(&formatContext);
            avformat_free_context(formatContext);
        }

        for (auto &audioDecoder : audioDecoders) {
            delete audioDecoder;
        }
        for (auto &videoDecoder : videoDecoders) {
            delete videoDecoder;
        }


        pthread_join(initThread, nullptr);

    }

}

// 线程执行函数
void *BugMediaVideoLoader::initThreadFunc(void *pVoid) {
    auto *loader = (BugMediaVideoLoader *) pVoid;
    // 使用另一个方法，简化代码写法，不然每个变量访问
    // 都得类似于:loader->xxxx
    loader->init();
    return nullptr;
}

// 正经执行初始化操作的函数
void BugMediaVideoLoader::init() {

    formatContext = avformat_alloc_context();

    int ret = avformat_open_input(&formatContext, url, nullptr, nullptr);
    if (ret != 0) {
        LOGE("打开URL [%s] 失败:%s", url, av_err2str(ret));
        release();
        return;
    }

    ret = avformat_find_stream_info(formatContext, nullptr);
    if (ret != 0) {
        LOGE("查找流信息失败:%s", av_err2str(ret));
        release();
        return;
    }

    duration = formatContext->duration * av_q2d(AV_TIME_BASE_Q);


#ifdef DEBUGAPP
    LOGD("总时长：%lld，转换后：%ds,比特率：%lld", formatContext->duration, duration, formatContext->bit_rate);
#endif

    // 初始化解码器组，音频的视频的，不同的流各一个
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        AVCodecParameters *codecParameters = formatContext->streams[i]->codecpar;
        if (codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            try {
                auto *audioDecoder = new BugMediaFFmpegAudioDecoder(formatContext, i, decoderBufferSize);
                audioDecoders.push_back(audioDecoder);
                audioTrackCount++;
            } catch (char *e) {
                LOGE("%s", e);
                LOGE("初始化音频解码器失败");
                release();
                return;
            }


        } else if (codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            try {
                auto *videoDecoder = new BugMediaFFmpegVideoDecoder(formatContext, i, decoderBufferSize);

                videoDecoders.push_back(videoDecoder);
                videoTrackCount++;
            } catch (char *e) {
                LOGE("%s", e);
                LOGE("初始化图像解码器失败");
                release();
                return;
            }
        }
    }

    if (!videoDecoders.empty()) {
        currentVideoDecoder = videoDecoders[0];
    }

    if (!audioDecoders.empty()) {
        currentAudioDecoder = audioDecoders[0];
    }

    if (currentVideoDecoder == nullptr && currentAudioDecoder == nullptr) {
        LOGE("没有找到媒体信息");
        return;
    }

    // 解码器准备好之后进行解码，渲染器此时也可以工作了

    if (currentAudioDecoder != nullptr) {
#ifdef DEBUGAPP
        LOGD("音频解码器数量:%d,", audioDecoders.size());
        currentAudioDecoder->startDecode();
        audioRenderer->render();
#else

        currentAudioDecoder->startDecode();
        audioRenderer->render();
#endif
    }

    if (currentVideoDecoder != nullptr) {
#ifdef DEBUGAPP
        LOGD("视频解码器数量:%d", videoDecoders.size());
        currentVideoDecoder->startDecode();
        videoRenderer->render();
#else
        currentVideoDecoder->startDecode();
        videoRenderer->render();
#endif
    }


}


void BugMediaVideoLoader::setBufferSize(int size) {
    decoderBufferSize = size;
}

void BugMediaVideoLoader::load() {
    // 文件探测可能会花很长时间，
    // 所以启动一个线程执行

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&initThread, &attr, initThreadFunc, this);
    isLoaded = true;
}

void BugMediaVideoLoader::switchAudioChannel(int ch) {
    currentAudioDecoder = audioDecoders[ch];

}

void BugMediaVideoLoader::switchVideoChannel(int ch) {
    currentVideoDecoder = videoDecoders[ch];
}


int BugMediaVideoLoader::getAudioTrackCount() const {
    return audioTrackCount;
}

int BugMediaVideoLoader::getVideoTrackCount() const {
    return videoTrackCount;
}

int64_t BugMediaVideoLoader::getAudioPts() const {
    return audioPts;
}


BugMediaAudioFrame *BugMediaVideoLoader::getAudioFrame() {

    auto frame = currentAudioDecoder->getFrame();
    audioPts = frame->pts;
    if (!frame->isEnd) {
        isAudioEnd = true;
    }

    return frame;
}

BugMediaVideoFrame *BugMediaVideoLoader::getVideoFrame() {

    auto frame = currentVideoDecoder->getFrame();
    if (!frame->isEnd) {
        isVideoEnd = true;
    }
    return frame;
}

void BugMediaVideoLoader::play() {
    if (isLoaded){
        audioRenderer->play();
        //videoRenderer->play();
    }

}

void BugMediaVideoLoader::pause() {
    if (isLoaded){
        audioRenderer->pause();
        //videoRenderer->pause();
    }

}

void BugMediaVideoLoader::stop() {
    if (isLoaded){
        audioRenderer->stop();
        //videoRenderer->stop();
    }

}

void BugMediaVideoLoader::destroy() {
    release();
}

void BugMediaVideoLoader::setWindowSurface(JNIEnv *env, jobject surface) {
    videoRenderer->setWindowSurface(env, surface);
}

void BugMediaVideoLoader::setPBufferSurface(EGLint width, EGLint height) {
    videoRenderer->setPBufferSurface(width, height);
}

void BugMediaVideoLoader::resizeView(GLint x, GLint y, GLsizei width, GLsizei height) {
    videoRenderer->resizeView(x, y, width, height);
}







