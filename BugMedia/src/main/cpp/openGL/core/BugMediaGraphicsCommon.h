//
// Created by Gshine on 2021/1/26.
//

#ifndef SLOWER_BUGMEDIAGRAPHICSCOMMON_H
#define SLOWER_BUGMEDIAGRAPHICSCOMMON_H

#define DEBUGAPP


#include <android/log.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#define TAG "BugMediaOutput"
#define  LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

enum surfaceTypeEnum {
    NO_SURFACE, WINDOW_SURFACE, PBUFFER_SURFACE
};

void getShowPictureSize(GLint &newWidth, GLint &newHeight, EGLint &viewWidth, EGLint &viewHeight, GLint &picWidth,
                        GLint &picHeight);

void scaleCenter(EGLint &viewWidth, EGLint &viewHeight, EGLint &width,
                EGLint &height);

void moveToCenter(EGLint &viewWidth,EGLint &viewHeight,GLint &width,GLint &height);


#endif //SLOWER_BUGMEDIAGRAPHICSCOMMON_H
