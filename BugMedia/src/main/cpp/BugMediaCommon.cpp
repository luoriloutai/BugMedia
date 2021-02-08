//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaCommon.h"

int64_t getCurMsTime() {
    struct timeval tv; //timeval.tv_sec单位为秒，timeval.tv_usec单位为微秒
    gettimeofday(&tv, NULL);
    int64_t ts = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return ts;
}
