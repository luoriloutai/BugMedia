//
// Created by Gshine on 2021/2/4.
//
// Updated by Gshine on 2021/2/15
//
// 一次只解一个流，多个流需要创建多个该类的实例，
// 向构造函数传入相应的媒体类型创建相应的解码器。
// 创建多个实例的目的是为了各流解码操作互不干扰，
// 各自将数据放入各自的队列，装填和取出数据的等
// 待状态也不会互相干扰。
//

#ifndef SLOWER_BUGMEDIAFFMPEGDECODER_H
#define SLOWER_BUGMEDIAFFMPEGDECODER_H


extern "C" {
#include "include/ffmpeg/libavcodec/avcodec.h"
#include "include/ffmpeg/libavformat/avformat.h"
#include "include/ffmpeg/libswscale/swscale.h"
#include "include/ffmpeg/libswresample/swresample.h"
#include "include/ffmpeg/libavutil/pixdesc.h"
#include "include/ffmpeg/libavutil/frame.h"
#include "include/ffmpeg/libavutil/time.h"
#include "include/ffmpeg/libavutil/avutil.h"
#include "include/ffmpeg/libavutil/opt.h"
#include "include/ffmpeg/libavutil/imgutils.h"
}

#include <vector>
#include "BugMediaCommon.h"
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include "BugMediaAudioFrame.h"
#include "BugMediaVideoFrame.h"

using namespace std;

class BugMediaFFmpegDecoder {
    pthread_t startThread{};
    int bufferSize{};
    const char *url{};
    queue<void *> frameQueue{};
    sem_t canFillData{};
    sem_t canTakeData{};
    bool quit = false;
    int64_t streamDuration = 0;
    AVCodec *avCodec = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    AVPacket *avPacket = nullptr;
    AVFrame *avFrame = nullptr;
    AVFormatContext *avFormatContext{};
    const char *codecType{};
    int64_t duration{};
    AVMediaType mediaType{};
    vector<int> streamIndices{};
    int currentStreamIndex = -1;

    /*
     * 音频相关
     * */

    // 音频重采样上下文
    SwrContext *swrContext{};

    // 音频格式转换输出缓冲
    uint8_t *audioOutputBuffer[2] = {};

    // 音频重采样个数
    // acc默认为1024，重采样后可能会变化
    int resampleCount = 1024;

    // 音频重采样以后，一帧数据的大小
    size_t resampleSize = 0;

    // 音频编码格式：浮点型数据（32位）
    const AVSampleFormat ENCODE_FORMAT = AV_SAMPLE_FMT_FLTP;

    const AVSampleFormat AUDIO_OUT_SAMPLE_FORMAT = AV_SAMPLE_FMT_S32;

    // 音频采样率
    const int SAMPLE_RATE = 44100;

    // 音频声道数
    const int CHANNEL_COUNTS = 2;

    // 音频声道格式:立体声（双声道）
    const uint64_t CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;

    // 音频比特率
    const int BIT_RATE = 64000;

    // ACC音频一帧采样数
    const int SAMPLES_COUNT = 1024;


//    void initAudioOutputBuffer();

    void initSwrContext();

    /*
     * 视频相关
     * */

    // 用于从YUV转换成RGB
    SwsContext *swsContext{};

    // 输出格式
    const AVPixelFormat VIDEO_OUT_FORMAT = AV_PIX_FMT_RGBA;

    // 输出缓冲
    uint8_t *videoOutputBuffer{};

    // 视频帧转换的缓冲帧
    AVFrame *bufferFrame{};

    void initSwsContext();

    void initVideoOutputBuffer();


    /*
     * 解码操作相关
     * */
    void decode();

    void openDecoder();

    static void *startRoutine(void *pVoid);

    void start();

    void sendAudioEndFrame();

    void sendVideoEndFrame();

    void sendEndFrame();

    // 转换音频帧数据为可播放的PCM并且入队
    void convertAudioFrame();

    // 转换视频帧数据为可显示的RGBA模式并且入队
    void convertVideoFrame();

    void convertFrame();

    void findIndices();


public:
    int64_t getStreamDuration() const;

    virtual ~ BugMediaFFmpegDecoder();

    BugMediaFFmpegDecoder(const char *url, int bufferSize, AVMediaType mediaType);

    void startDecode();

    int64_t getDuration() const;

    // 当前解码器为音频解码器时调用
    BugMediaAudioFrame *getAudioFrame();

    // 当前解码器为视频解码器时调用
    BugMediaVideoFrame *getVideoFrame();


};


#endif //SLOWER_BUGMEDIAFFMPEGDECODER_H
