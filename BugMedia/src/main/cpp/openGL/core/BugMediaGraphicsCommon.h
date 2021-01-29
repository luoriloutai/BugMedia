//
// Created by Gshine on 2021/1/26.
//

#ifndef SLOWER_BUGMEDIAGRAPHICSCOMMON_H
#define SLOWER_BUGMEDIAGRAPHICSCOMMON_H

#define DEBUGAPP

#include <android/log.h>
#define TAG "BugMediaOutput"
#define  LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

enum surfaceTypeEnum{NONE_SURFACE,WINDOW_SURFACE,PBUFFER_SURFACE};

#endif //SLOWER_BUGMEDIAGRAPHICSCOMMON_H
