//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIACOMMON_H
#define SLOWER_BUGMEDIACOMMON_H
#include <android/log.h>
#include <linux/time.h>
#include <cstdint>
#include <sys/time.h>
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>


#define TAG "BugMediaOutput"
#define  LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)



enum State {
    UNSTART,STOP, PLAYING, PAUSE
};

int64_t getCurMsTime();

void getShowPictureSize(GLint &newWidth, GLint &newHeight, EGLint &viewWidth, EGLint &viewHeight, GLint &picWidth,
                        GLint &picHeight);

// 将图形进行缩放并移到视图的中间
void scaleCenter(EGLint &viewWidth, EGLint &viewHeight, EGLint &picWidth,
                 EGLint &picHeight);

void moveToCenter(EGLint &viewWidth, EGLint &viewHeight, GLint &picWidth, GLint &picHeight);

#endif //SLOWER_BUGMEDIACOMMON_H
