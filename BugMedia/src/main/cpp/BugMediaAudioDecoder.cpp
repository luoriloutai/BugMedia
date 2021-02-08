//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaAudioDecoder.h"

BugMediaAudioFrame *BugMediaAudioDecoder::getFrame() {

    sem_wait(&this->canTakeData);
    BugMediaAudioFrame *frame = frameQueue.front();
    if(!frame->isEnd){
        sem_post(&this->canFillData);
    }

    return frame;
}


BugMediaAudioDecoder::BugMediaAudioDecoder(AVFormatContext *formatContext, int trackIdx,int bufferSize) :
        BugMediaBaseDecoder(formatContext, trackIdx) {
    this->bufferSize = bufferSize;
    sem_init(&this->canFillData, 0, this->bufferSize);
    sem_init(&this->canTakeData, 0, 0);
    this->decodeThread = pthread_create(&decodeThread, nullptr, decodeRoutine, this);
}

void BugMediaAudioDecoder::decode() {
    bool continueDecode = true;
    while (continueDecode) {
        sem_wait(&this->canFillData);

        if (av_read_frame(avFormatContext, avPacket) != 0) {
            // 非正常结束
            auto *frame = new BugMediaAudioFrame();
            frame->isEnd = true;
            frameQueue.push(frame);
            sem_post(&this->canTakeData);
            LOGE("av_read_frame 失败");
            return;
        }

        // avcodec_send_packet()如果在帧没有完全被取完时不会返回0，
        // 而是返回AVERROR(EAGAIN)
        // 这就需要不断调用avcodec_receive_frame()消耗完。
        // 这种情形一般只存在于音频包，音频包有可能一个包内含有多个帧，
        // 视频包一般只有一个帧
        int packetRet = avcodec_send_packet(avCodecContext, avPacket);
        if (packetRet != 0) {
            if (AVERROR_EOF == packetRet) {
                // 正常结束
                //
                // 结束的时候也往队列放一个帧，将其结束标志设置上，
                // 让帧来作为判断结束的依据
                auto *frame = new BugMediaAudioFrame();
                frame->isEnd = true;
                frameQueue.push(frame);
                sem_post(&this->canTakeData);
                return;
            }
        }

        // 取一个包内的所有帧
        // 取完所有帧后下次avcodec_send_packet()才能调用成功
        while (true) {
            sem_wait(&this->canFillData);
            int receiveRet = avcodec_receive_frame(avCodecContext, avFrame);
            if (receiveRet == 0) {
                // 返回0说明取帧成功，并且该包中还有其他帧没取出来，需要再次取

                auto *aFrame = new BugMediaAudioFrame();
                aFrame->channels = avFrame->channels;
                aFrame->format = avFrame->format;
                aFrame->position = avFrame->pkt_pos;
                aFrame->sampleRate = avFrame->sample_rate;
                // avFrame->pts是以stream.time_base为单位的时间戳，单位为秒
                // time_base不是一个数，是一个AVRational结构，可用av_q2d()转换成double,
                // 这个结构本质上是一个分子和一个分母表示的分数，av_q2d()就是用分子除以
                // 分母得出的数。
                // pts*av_q2d(time_base)就是这个值的最终表示，单位为秒
                aFrame->pts = avFrame->pts * av_q2d(avFormatContext->streams[trackIndex]->time_base) * 1000;
                aFrame->data = avFrame->data;

                frameQueue.push(aFrame);
                sem_post(&this->canTakeData);

            } else if (receiveRet == AVERROR_EOF) {
                // 正常结束
                // 一个包有可能是流中的最后一个包，最后一个包的最后一帧
                // 就是整个流中的最后一帧，此时解码应结束
                auto *frame = new BugMediaAudioFrame();
                frame->isEnd = true;
                frameQueue.push(frame);
                continueDecode = false;
                sem_post(&this->canTakeData);
                break;
            } else {
                // 返回其他值说明这个包的帧都取完了，开始下一个包
                break;
            }


        }

        // 解除对包的引用，以便下次再用
        av_packet_unref(avPacket);
        // av_frame_unref()可以对帧解引用，但这里不需要调用，因为avcodec_receive_frame()会调用它。
    }
}

void *BugMediaAudioDecoder::decodeRoutine(void *ctx) {
    auto self = (BugMediaAudioDecoder *) ctx;
    self->decode();
    return nullptr;
}

BugMediaAudioDecoder::~BugMediaAudioDecoder() {
    while (!frameQueue.empty()){
        BugMediaAudioFrame * frame=frameQueue.front();
        delete frame;
        frameQueue.pop();
    }

    sem_destroy(&this->canFillData);
    sem_destroy(&this->canTakeData);
}
