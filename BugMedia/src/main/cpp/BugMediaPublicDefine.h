//
// Created by Gshine on 2021/3/3.
//

#ifndef SLOWER_BUGMEDIAPUBLICDEFINE_H
#define SLOWER_BUGMEDIAPUBLICDEFINE_H

#include <android/log.h>


#define TAG "BugMediaOutput"
#define  LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

#endif // SLOWER_BUGMEDIAPUBLICDEFINE_H

