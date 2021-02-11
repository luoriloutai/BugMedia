//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaFFmpegVideoDecoder.h"

#define DEBUGAPP

BugMediaFFmpegVideoDecoder::BugMediaFFmpegVideoDecoder(AVFormatContext *formatContext, int trackIdx, int bufferSize)
        : BugMediaFFmpegBaseDecoder(formatContext, trackIdx) {
    this->bufferSize = bufferSize;
    sem_init(&this->canFillData, 0, this->bufferSize);
    sem_init(&this->canTakeData, 0, 0);

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
            vFrame->isEnd= true;
            frameQueue.push(vFrame);
            sem_post(&this->canTakeData);
            LOGE("av_read_frame 失败");
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
                vFrame->isEnd= true;
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
            auto *vFrame = new BugMediaVideoFrame();
            vFrame->isKeyframe = avFrame->key_frame == 1;

            // avFrame->pts是以stream.time_base为单位的时间戳，单位为秒
            // time_base不是一个数，是一个AVRational结构，可用av_q2d()转换成double,
            // 这个结构本质上是一个分子和一个分母表示的分数，av_q2d()就是用分子除以
            // 分母得出的数。
            // pts*av_q2d(time_base)就是这个值的最终表示，单位为秒
            vFrame->pts = avFrame->pts * av_q2d(avFormatContext->streams[trackIndex]->time_base) * 1000;
            vFrame->data = avFrame->data;
            vFrame->isInterlaced = avFrame->interlaced_frame == 1;
            vFrame->position = avFrame->pkt_pos;
            vFrame->format = avFrame->format;
            vFrame->width = avFrame->width;
            vFrame->height = avFrame->height;

            frameQueue.push(vFrame);
            sem_post(&this->canTakeData);

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
    if (quit){
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








