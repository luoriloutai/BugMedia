//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaFFmpegVideoDecoder.h"

extern "C" {
#include "include/ffmpeg/libavutil/imgutils.h"
}

#define DEBUGAPP

BugMediaFFmpegVideoDecoder::BugMediaFFmpegVideoDecoder(AVFormatContext *formatContext, int trackIdx, int bufferSize)
        : BugMediaFFmpegBaseDecoder(formatContext, trackIdx) {
    this->bufferSize = bufferSize;
    sem_init(&this->canFillData, 0, this->bufferSize);
    sem_init(&this->canTakeData, 0, 0);

    // 输出转换，输入与输出宽高相同
    swsContext = sws_getContext(avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
                                avCodecContext->width,
                                avCodecContext->height, OUT_FORMAT, SWS_FAST_BILINEAR, nullptr,
                                nullptr, nullptr);

    // 输出缓冲
    int nums = av_image_get_buffer_size(OUT_FORMAT, avCodecContext->width, avCodecContext->height, 1);
    outputBuffer = (uint8_t *) av_malloc(nums*sizeof(uint8_t));
    bufferFrame = av_frame_alloc();
    av_image_fill_arrays(bufferFrame->data, bufferFrame->linesize, outputBuffer, OUT_FORMAT, avCodecContext->width,
                        avCodecContext->height, 1);

}

BugMediaFFmpegVideoDecoder::~BugMediaFFmpegVideoDecoder() {
    quit = true;
    sem_post(&canTakeData);
    sem_post(&canFillData);

    while (!frameQueue.empty()) {
        auto *frame = frameQueue.front();
        delete frame;
        frameQueue.pop();
    }

    if (swsContext != nullptr) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }

    if (outputBuffer != nullptr) {
        free(outputBuffer);
        outputBuffer = nullptr;
    }

    if (bufferFrame != nullptr) {
        av_frame_free(&bufferFrame);
        bufferFrame = nullptr;
    }

    void *retval;
    pthread_join(decodeThread, &retval);
    sem_destroy(&this->canFillData);
    sem_destroy(&this->canTakeData);
}


void BugMediaFFmpegVideoDecoder::decode() {
#ifdef DEBUGAPP
    LOGD("开始解码视频");
#endif

    int ret;

    while (true) {

        sem_wait(&this->canFillData);
        if (quit) {
            return;
        }

        if (av_read_frame(avFormatContext, avPacket) != 0) {
            // 结束

            auto *vFrame = new BugMediaVideoFrame();
            vFrame->isEnd = true;
            frameQueue.push(vFrame);
            sem_post(&this->canTakeData);

            return;
        }

        // 读取的包视频和音频混合在一起，需要判断选取
        if (avPacket->stream_index != trackIndex) {
            sem_post(&canFillData);
            continue;
        }

        int packetRet = avcodec_send_packet(avCodecContext, avPacket);
        if (packetRet != 0) {
            if (AVERROR_EOF == packetRet) {
                // 正常结束
                //
                // 结束的时候也往队列放一个帧，将其结束标志设置上，
                // 让帧来作为判断结束的依据

                auto *vFrame = new BugMediaVideoFrame();
                vFrame->isEnd = true;
                frameQueue.push(vFrame);
                sem_post(&this->canTakeData);
                return;

            }
        }

        // 该方法执行不返回0需要再次av_read_frame从头开始执行读取帧，直至成功返回0表示取到了一帧
        ret = avcodec_receive_frame(avCodecContext, avFrame);
#ifdef DEBUGAPP
        LOGD("avcodec_receive_frame执行结果：%d ,%s", ret, av_err2str(ret));
#endif
        if (ret == AVERROR_EOF) {
            auto *vFrame = new BugMediaVideoFrame();
            vFrame->isEnd = true;
            frameQueue.push(vFrame);
            sem_post(&this->canTakeData);
            return;

        }

        if (ret == 0) {

#ifdef DEBUGAPP
            LOGD("成功接收到帧");
            LOGD("帧大小：%d X %d",avFrame->width,avFrame->height);
            LOGD("数据大小：%d",sizeof(avFrame->data));
#endif


            // 转换YUV为RGB，存储到缓冲帧
            int imgHeiht = sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avFrame->height, bufferFrame->data,
                      bufferFrame->linesize);

#ifdef DEBUGAPP

            LOGD("图像高度为:%d",imgHeiht);

#endif

            if (imgHeiht > 0) {
                auto *vFrame = new BugMediaVideoFrame();
                vFrame->isKeyframe = avFrame->key_frame == 1;

                // avFrame->pts是以stream.time_base为单位的时间戳，单位为秒
                // time_base不是一个数，是一个AVRational结构，可用av_q2d()转换成double,
                // 这个结构本质上是一个分子和一个分母表示的分数，av_q2d()就是用分子除以
                // 分母得出的数。
                // pts*av_q2d(time_base)就是这个值的最终表示，单位为秒
                vFrame->pts = avFrame->pts * av_q2d(avFormatContext->streams[trackIndex]->time_base) * 1000;


                // 以下两字段用缓冲里的数据
                int bufSize = av_image_get_buffer_size(OUT_FORMAT, avFrame->width, avFrame->height, 1);
                uint8_t *rgb= (uint8_t *) av_malloc(bufSize*sizeof(uint8_t));
                memcpy(rgb,bufferFrame->data[0],bufSize);
                vFrame->data = rgb;
                vFrame->lineSize =bufferFrame->linesize[0];

                vFrame->isInterlaced = avFrame->interlaced_frame == 1;
                vFrame->position = avFrame->pkt_pos;
                vFrame->format = avFrame->format;
                vFrame->width = avFrame->width;
                vFrame->height = avFrame->height;

                frameQueue.push(vFrame);
                sem_post(&this->canTakeData);

#ifdef DEBUGAPP
                static int couter = 0;
                LOGD("第%d帧视频解码完毕", ++couter);
                LOGD("队列大小：%d", frameQueue.size());


#endif

            } else {
                // 未写入缓冲，退还信号
                sem_post(&canFillData);

            }


        } else {
            // 这一次做的事情没有成功，返还信号
            sem_post(&canFillData);
        }


        // 解除对包的引用，以便下次再用
        av_packet_unref(avPacket);

    }  // 最外层while


}

void *BugMediaFFmpegVideoDecoder::decodeRoutine(void *ctx) {
    auto self = (BugMediaFFmpegVideoDecoder *) ctx;
    self->decode();
    return nullptr;
}

BugMediaVideoFrame *BugMediaFFmpegVideoDecoder::getFrame() {
    sem_wait(&this->canTakeData);
    if (quit) {
        // 用于退出等待，当没有数据时处于等待状态，
        // 析构函数里再次发送一个信号就会执行到这里，
        // 等待状态就退出了
        return nullptr;
    }
    auto *frame = frameQueue.front();
    frameQueue.pop();
    if (!frame->isEnd) {
        sem_post(&this->canFillData);
    }

    return frame;
}

void BugMediaFFmpegVideoDecoder::startDecode() {

    this->decodeThread = pthread_create(&decodeThread, nullptr, decodeRoutine, this);
}








