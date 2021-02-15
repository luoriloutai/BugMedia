//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaFFmpegDecoder.h"


#define DEBUGAPP

BugMediaFFmpegDecoder::~BugMediaFFmpegDecoder() {

    if (avPacket != nullptr) {
        av_packet_free(&avPacket);
    }
    if (avFrame != nullptr) {
        av_frame_free(&avFrame);
    }
    if (avCodecContext != nullptr) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
    }

    if (swsContext != nullptr) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }

    if (swrContext != nullptr) {
        swr_free(&swrContext);
    }

    if (avFormatContext != nullptr) {
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
    }

    quit = true;
    sem_post(&canTakeData);
    sem_post(&canFillData);

    if (mediaType == AVMEDIA_TYPE_AUDIO) {
        while (!frameQueue.empty()) {
            auto *frame = (BugMediaAudioFrame *) frameQueue.front();
            delete frame;
            frameQueue.pop();
        }
    }

    if (mediaType == AVMEDIA_TYPE_VIDEO) {
        while (!frameQueue.empty()) {
            auto *frame = (BugMediaVideoFrame *) frameQueue.front();
            delete frame;
            frameQueue.pop();
        }
    }


    if (audioOutputBuffer[0] != nullptr) {
        free(audioOutputBuffer[0]);
        audioOutputBuffer[0] = nullptr;
    }

    if (videoOutputBuffer != nullptr) {
        free(videoOutputBuffer);
        videoOutputBuffer = nullptr;
    }

    if (bufferFrame != nullptr) {
        av_frame_free(&bufferFrame);
        bufferFrame = nullptr;
    }

    void *retval;
    pthread_join(startThread, &retval);
    sem_destroy(&this->canFillData);
    sem_destroy(&this->canTakeData);

}


int64_t BugMediaFFmpegDecoder::getStreamDuration() const {
    return streamDuration;
}


void BugMediaFFmpegDecoder::openDecoder() {

    AVStream *stream = avFormatContext->streams[currentStreamIndex];

    // 流长与总时长不一样
    streamDuration = stream->duration * av_q2d(stream->time_base);

#ifdef DEBUGAPP
    LOGD("流长度：%lld,换算后长度：%d", stream->duration, streamDuration);
#endif

    codecType = stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO ? "audio" : "video";
    avCodec = avcodec_find_decoder(stream->codecpar->codec_id);
    avCodecContext = avcodec_alloc_context3(avCodec);
    // 向avCondecContext填充参数，不调用的话解码器上下文中获取不到参数
    if (avcodec_parameters_to_context(avCodecContext, stream->codecpar) != 0) {
        LOGE("获取参数失败");
        return;
    }
    if (avcodec_open2(avCodecContext, avCodec, nullptr) != 0) {
        LOGE("打开%s解码器失败", codecType);
        return;
    }


    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();

}

BugMediaFFmpegDecoder::BugMediaFFmpegDecoder(const char *url, int bufferSize, AVMediaType mediaType) {
    this->mediaType = mediaType;
    this->bufferSize = bufferSize;
    this->url = url;

    sem_init(&this->canFillData, 0, this->bufferSize);
    sem_init(&this->canTakeData, 0, 0);


}

void BugMediaFFmpegDecoder::findIndices() {
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == mediaType) {
            streamIndices.push_back(i);
        }
    }

    currentStreamIndex = streamIndices[0];
}

void *BugMediaFFmpegDecoder::startRoutine(void *pVoid) {
    auto self = (BugMediaFFmpegDecoder *) pVoid;
    self->start();
    return nullptr;
}

void BugMediaFFmpegDecoder::start() {
    avFormatContext = avformat_alloc_context();

    int ret = avformat_open_input(&avFormatContext, url, nullptr, nullptr);
    if (ret != 0) {
        LOGE("打开URL [%s] 失败:%s", url, av_err2str(ret));

        return;
    }

    ret = avformat_find_stream_info(avFormatContext, nullptr);
    if (ret != 0) {
        LOGE("查找流信息失败:%s", av_err2str(ret));

        return;
    }

#ifdef DEBUGAPP
    LOGD("类型：%s", mediaType == AVMEDIA_TYPE_AUDIO ? "音频" : "视频");
#endif


    duration = avFormatContext->duration * av_q2d(AV_TIME_BASE_Q);

    findIndices();
    openDecoder();

    // 初始化音频转换需要的组件
    if (mediaType == AVMEDIA_TYPE_AUDIO) {
        initSwrContext();
    }

    // 初始化视频转换需要的组件
    if (mediaType == AVMEDIA_TYPE_VIDEO) {
        initSwsContext();
        initVideoOutputBuffer();
    }


    // 解码。读取一定数量的帧放入队列作为缓冲
    decode();
}

void BugMediaFFmpegDecoder::startDecode() {
    pthread_create(&startThread, nullptr, startRoutine, this);
}


//void BugMediaFFmpegDecoder::initAudioOutputBuffer() {
//    // 根据输入的采样数和采样率计算出重采样的个数
//    // 目标采样个数 = 原采样个数 *（目标采样率 / 原采样率）
//    resampleCount = (int) av_rescale_rnd(SAMPLES_COUNT, getSampleRate(avCodecContext->sample_rate),
//                                         avCodecContext->sample_rate, AV_ROUND_UP);
//    // 重采样后一帧数据的大小
////    resampleSize = (size_t) av_samples_get_buffer_size(
////            nullptr, CHANNEL_COUNTS,
////            resampleCount, getSampleFmt(), 1);
//
//    resampleSize = (size_t) av_samples_get_buffer_size(
//            nullptr, avCodecContext->channels,
//            resampleCount, AV_SAMPLE_FMT_S16, 1);
//
//    // 不行,使用输入的采样格式
////    resampleSize = (size_t) av_samples_get_buffer_size(
////            nullptr, CHANNEL_COUNTS,
////            resampleCount, avCodecContext->sample_fmt, 1);
//
//    audioOutputBuffer[0] = (uint8_t *) malloc(resampleSize);
//
//
////    audioOutputBuffer[0] = (uint8_t *) malloc(resampleSize / 2);
////    audioOutputBuffer[1] = (uint8_t *) malloc(resampleSize / 2);
//
//}

void BugMediaFFmpegDecoder::initSwrContext() {

    swrContext = swr_alloc();

    av_opt_set_int(swrContext, "in_channel_layout", avCodecContext->channel_layout, 0);
    av_opt_set_int(swrContext, "out_channel_layout", avCodecContext->channel_layout, 0);
    av_opt_set_int(swrContext, "in_sample_rate", avCodecContext->sample_rate, 0);
    av_opt_set_int(swrContext, "out_sample_rate", avCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", avCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", OUT_SAMPLE_FORMAT, 0);

    swr_init(swrContext);

}

void BugMediaFFmpegDecoder::decode() {
    int ret;

    while (true) {

        sem_wait(&this->canFillData);
        if (quit) {
            return;
        }

        ret = av_read_frame(avFormatContext, avPacket);
        if (ret != 0) {

            //
            // 流结束或者出错
            //
            // 结束的时候也往队列放一个帧，将其结束标志设置上，
            // 让帧来作为判断结束的依据

            sendEndFrame();
            return;
        }

        // 读取的包视频和音频混合在一起，需要判断选取
        if (avPacket->stream_index != currentStreamIndex) {
            sem_post(&canFillData);
            continue;
        }

        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            if (AVERROR_EOF == ret) {
                //
                // 正常结束
                //

                sendEndFrame();
                return;
            }
        }

        // 该方法执行不返回0需要再次执行读取帧，直至成功返回0表示取到了一帧
        ret = avcodec_receive_frame(avCodecContext, avFrame);

        if (ret == AVERROR_EOF) {
            //
            // 流结束
            //

            sendEndFrame();
            return;

        }

        // 取到一帧
        if (ret == 0) {
            convertFrame();

        } else {
            // 这一次做的事情没有成功，返还信号
            sem_post(&canFillData);
        }

        // 解除对包的引用，以便下次再用
        av_packet_unref(avPacket);

    }  // while
}

void BugMediaFFmpegDecoder::sendAudioEndFrame() {
    auto aFrame = new BugMediaAudioFrame();
    aFrame->isEnd = true;
    frameQueue.push(aFrame);
    sem_post(&this->canTakeData);
}

void BugMediaFFmpegDecoder::sendVideoEndFrame() {
    auto *vFrame = new BugMediaVideoFrame();
    vFrame->isEnd = true;
    frameQueue.push(vFrame);
    sem_post(&this->canTakeData);
}

void BugMediaFFmpegDecoder::sendEndFrame() {
    if (mediaType == AVMEDIA_TYPE_VIDEO) {
        sendVideoEndFrame();
    } else if (mediaType == AVMEDIA_TYPE_AUDIO) {
        sendAudioEndFrame();
    }
}

void BugMediaFFmpegDecoder::convertAudioFrame() {

    // 获取输出缓冲大小
    // 输出采格格式要与swr设置的一样
    int dataSize = av_samples_get_buffer_size(
            avFrame->linesize, avCodecContext->channels,
            avFrame->nb_samples, OUT_SAMPLE_FORMAT, 1);

    // 输出缓冲
    uint8_t *outData = new uint8_t[dataSize]();

    // 音频重采样转换
    int sampleCount = swr_convert(swrContext, &outData, dataSize,
                                  (const uint8_t **) (avFrame->data), avFrame->nb_samples);

    if (sampleCount > 0) {
        auto *aFrame = new BugMediaAudioFrame();
        aFrame->channels = avFrame->channels;
        aFrame->format = avFrame->format;
        aFrame->position = avFrame->pkt_pos;
        aFrame->sampleRate = avFrame->sample_rate;
        aFrame->sampleCount = avFrame->nb_samples;
        aFrame->resampleSize = dataSize;

        // avFrame->pts是以stream.time_base为单位的时间戳，单位为秒
        // time_base不是一个数，是一个AVRational结构，可用av_q2d()转换成double,
        // 这个结构本质上是一个分子和一个分母表示的分数，av_q2d()就是用分子除以
        // 分母得出的数。
        // pts*av_q2d(time_base)就是这个值的最终表示，单位为秒
        aFrame->pts = avFrame->pts * av_q2d(avFormatContext->streams[currentStreamIndex]->time_base) * 1000;

        aFrame->data = outData;
        frameQueue.push(aFrame);
        sem_post(&this->canTakeData);

#ifdef DEBUGAPP
        static int couter = 0;
        LOGD("第%d帧音频解码完毕", ++couter);
        LOGD("队列大小：%d", frameQueue.size());

#endif


    } else {

        sem_post(&canFillData);
    }
}

void BugMediaFFmpegDecoder::convertVideoFrame() {
    // 转换YUV为RGBA，存储到缓冲帧
    int imgHeiht = sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avFrame->height, bufferFrame->data,
                             bufferFrame->linesize);


    if (imgHeiht > 0) {
        auto *vFrame = new BugMediaVideoFrame();
        vFrame->isKeyframe = avFrame->key_frame == 1;

        // avFrame->pts是以stream.time_base为单位的时间戳，单位为秒
        // time_base不是一个数，是一个AVRational结构，可用av_q2d()转换成double,
        // 这个结构本质上是一个分子和一个分母表示的分数，av_q2d()就是用分子除以
        // 分母得出的数。
        // pts*av_q2d(time_base)就是这个值的最终表示，单位为秒
        vFrame->pts = avFrame->pts * av_q2d(avFormatContext->streams[currentStreamIndex]->time_base) * 1000;


        // 以下两字段用缓冲里的数据
        int bufSize = av_image_get_buffer_size(VIDEO_OUT_FORMAT, avFrame->width, avFrame->height, 1);
        uint8_t *rgb = new uint8_t[bufSize];
        memcpy(rgb, bufferFrame->data[0], bufSize);
        vFrame->data = rgb;
        vFrame->lineSize = bufferFrame->linesize[0];

        vFrame->isInterlaced = avFrame->interlaced_frame == 1;
        vFrame->position = avFrame->pkt_pos;
        vFrame->format = avFrame->format;
        vFrame->width = avFrame->width;
        vFrame->height = avFrame->height;

        frameQueue.push(vFrame);
        delete[] bufferFrame->data[0];
        av_frame_unref(bufferFrame);
        sem_post(&this->canTakeData);

#ifdef DEBUGAPP
        static int couter = 0;
        LOGD("第%d帧视频解码完毕", ++couter);
        LOGD("视频帧队列大小：%d", frameQueue.size());


#endif

    } else {
        // 未写入缓冲，退还信号
        sem_post(&canFillData);
    }
}

void BugMediaFFmpegDecoder::convertFrame() {
    if (mediaType == AVMEDIA_TYPE_AUDIO) {
        convertAudioFrame();
    } else if (mediaType == AVMEDIA_TYPE_VIDEO) {
        convertVideoFrame();
    }
}

void BugMediaFFmpegDecoder::initSwsContext() {
    // 视频输出转换，输入与输出宽高相同

    swsContext = sws_getContext(avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
                                avCodecContext->width,
                                avCodecContext->height, VIDEO_OUT_FORMAT, SWS_FAST_BILINEAR, nullptr,
                                nullptr, nullptr);
}

void BugMediaFFmpegDecoder::initVideoOutputBuffer() {
    // 音频输出缓冲

    int nums = av_image_get_buffer_size(VIDEO_OUT_FORMAT, avCodecContext->width, avCodecContext->height, 1);
    videoOutputBuffer = (uint8_t *) av_malloc(nums * sizeof(uint8_t));
    bufferFrame = av_frame_alloc();
    av_image_fill_arrays(bufferFrame->data, bufferFrame->linesize, videoOutputBuffer, VIDEO_OUT_FORMAT,
                         avCodecContext->width,
                         avCodecContext->height, 1);
}

int64_t BugMediaFFmpegDecoder::getDuration() const {
    return duration;
}


BugMediaAudioFrame *BugMediaFFmpegDecoder::getAudioFrame() {
    if (mediaType == AVMEDIA_TYPE_VIDEO) {
        return nullptr;
    }

    sem_wait(&this->canTakeData);
    if (quit) {
        // 用于退出等待，当没有数据时处于等待状态，
        // 析构函数里再次发送一个信号就会执行到这里，
        // 等待状态就退出了
        return nullptr;
    }
    auto *frame = (BugMediaAudioFrame *) frameQueue.front();
    frameQueue.pop();
    if (!frame->isEnd) {
        sem_post(&this->canFillData);
    }

    return frame;
}

BugMediaVideoFrame *BugMediaFFmpegDecoder::getVideoFrame() {
    if (mediaType == AVMEDIA_TYPE_AUDIO) {
        return nullptr;
    }

    sem_wait(&this->canTakeData);
    if (quit) {
        // 用于退出等待，当没有数据时处于等待状态，
        // 析构函数里再次发送一个信号就会执行到这里，
        // 等待状态就退出了
        return nullptr;
    }
    auto *frame = (BugMediaVideoFrame *) frameQueue.front();
    frameQueue.pop();
    if (!frame->isEnd) {
        sem_post(&this->canFillData);
    }

    return frame;
}




