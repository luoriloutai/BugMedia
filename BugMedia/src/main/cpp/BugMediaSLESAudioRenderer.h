//
// Created by Gshine on 2021/2/7.
//

#ifndef SLOWER_BUGMEDIASLESAUDIORENDERER_H
#define SLOWER_BUGMEDIASLESAUDIORENDERER_H

extern "C" {
#include "include/ffmpeg/libavutil/opt.h"
#include "include/ffmpeg/libavutil/mem.h"
}

#include "BugMediaVideoLoader.h"
#include "interfaces/BugMediaRenderer.h"
#include <pthread.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class BugMediaSLESAudioRenderer : public BugMediaRenderer {
    class PcmData {
    public:
        PcmData(uint8_t *pcm, int size) {
            this->pcm = pcm;
            this->size = size;
        }

        ~PcmData() {
            if (pcm != NULL) {
                //释放已使用的内存
                free(pcm);
                pcm = NULL;
                used = false;
            }
        }

        uint8_t *pcm = NULL;
        int size = 0;
        bool used = false;
    };

    State currentState = UNSTART;

    BugMediaVideoLoader *videoLoader{};

    // 开始执行操作时的时间（毫秒）
    int64_t startTimeMs = -1;

    int64_t pst{};

    int64_t delay{};

    pthread_t renderThread{};

    static void *renderRoutine(void *ctx);

    // 格式转换上下文
    SwrContext *swrContext{};
    // 输出缓冲
    uint8_t *outputBuffer[2] = {};

    // 重采样个数
    // acc默认为1024，重采样后可能会变化
    int resampleCount = 1024;

    // 重采样以后，一帧数据的大小
    size_t resampleSize = 0;

    // 编码格式：浮点型数据（32位）
    static const AVSampleFormat ENCODE_FORMAT = AV_SAMPLE_FMT_FLTP;
    // 采样率
    static const int SAMPLE_RATE = 44100;
    // 声道数
    static const int CHANNEL_COUNTS = 2;
    // 声道格式:立体声（双声道）
    static const uint64_t CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
    // 比特率
    static const int BIT_RATE = 64000;
    // ACC音频一帧采样数
    static const int SAMPLES_COUNT = 1024;

    const SLuint32 SL_QUEUE_BUFFER_COUNT = 2;

    // 引擎接口
    SLObjectItf m_engine_obj = NULL;
    SLEngineItf m_engine = NULL;

    //混音器
    SLObjectItf m_output_mix_obj = NULL;
    SLEnvironmentalReverbItf m_output_mix_evn_reverb = NULL;
    SLEnvironmentalReverbSettings m_reverb_settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;

    //pcm播放器
    SLObjectItf m_pcm_player_obj = NULL;
    SLPlayItf m_pcm_player = NULL;
    SLVolumeItf m_pcm_player_volume = NULL;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf m_pcm_buffer;

    std::queue<PcmData *> m_data_queue;


    AVSampleFormat getSampleFmt();


    int getSampleRate(int inSampleRate);

    //渲染并返回是否结束条件
    bool renderOnce();

    void doRender();

    void initSwrContext();

    void initOutputBuffer();

    bool createEngine();

    bool createOutputMixer();

    bool configPlayer();

public:

    void render();

    void play();

    void pause();

    void stop();

    BugMediaSLESAudioRenderer(BugMediaVideoLoader *loader);

    ~BugMediaSLESAudioRenderer();

};


#endif //SLOWER_BUGMEDIASLESAUDIORENDERER_H
