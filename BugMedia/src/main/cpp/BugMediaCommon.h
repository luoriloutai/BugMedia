//
// Created by Gshine on 2021/2/4.
//

#ifndef SLOWER_BUGMEDIACOMMON_H
#define SLOWER_BUGMEDIACOMMON_H
#include <android/log.h>

#ifndef BUGMEDIAlOG
#define BUGMEDIAlOG

#define TAG "BugMediaOutput"
#define  LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

#endif



#endif //SLOWER_BUGMEDIACOMMON_H
