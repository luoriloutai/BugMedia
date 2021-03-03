//
// Created by Gshine on 2021/1/27.
//

// debug
// 使用Debug.h调试，最后注释或移除这一项即可
//#define DEBUGIT



#ifdef DEBUGIT

#include "../Debug.h"

#else

#include "openGL/BugMediaBaseRenderer.h"
#include <jni.h>
#include "BugMediaBridge.h"
#include "openGL/BugMediaTriangleRenderer.h"
#include "openGL/core/BugMediaGraphicsCommon.h"
#include "openGL/BugMediaPictureRenderer.h"
#include "BugMediaGLESVideoRenderer.h"
#include <mutex>
#include <map>
#include <thread>
#include "BugMediaPlayer.h"

using namespace std;

#endif

//
//// 记录放入存储的对象在存储区的索引，并赋给对象id
//// 目的是为了直接就可以以下标访问而不需要查询。
//// 共享变量，注意加锁
//static int rendererIndex = -1;
//mutex lockObj;
//map<int, BugMediaBaseRenderer *> renderers;
//
//void addRenderer(BugMediaBaseRenderer *r) {
//    rendererIndex++;
//    r->id = rendererIndex;
//    pair<int, BugMediaBaseRenderer *> pair(rendererIndex, r);
//    renderers.insert(pair);
//
//}
//
//void removeRenderer(int idx) {
//    lock_guard<mutex> lockGuard(lockObj);
//    if (idx < 0) {
//        return;
//    }
//    renderers[idx]->release();
//    //renderers.at(idx)->release();
//    renderers.erase(idx);
//}
//
//
//// 创建三角形渲染器
//int createTriangleRenderer() {
//    lock_guard<mutex> lockGuard(lockObj);
//
//    // tmd C++无参构造函数这样调用，不需要加个括号
//    auto *newRenderer = new BugMediaTriangleRenderer;
//    addRenderer(newRenderer);
//    return newRenderer->id;
//}
//
//// 创建图像渲染器
//int createPictureRenderer(uint8_t *data, GLint width, GLint height) {
//    lock_guard<mutex> lockGuard(lockObj);
//
//    auto *newRenderer = new BugMediaPictureRenderer(data, width, height);
//
//    addRenderer(newRenderer);
//
//    return newRenderer->id;
//}
//
//
//
//// 创建视频渲染器
//int createVideoRenderer() {
//    lock_guard<mutex> lockGuard(lockObj);
//
//    //auto *newRenderer= new BugMediaGLESVideoRenderer();
//    //addRenderer(newRenderer);
//    //return newRenderer->id;
//
//    return -1;
//}
//
//
//void startRenderer(int32_t rendererId) {
//    renderers.at(rendererId)->render();
//}
//
//void setWindowSurface(JNIEnv *env, jobject surface, int32_t rendererId) {
//    renderers.at(rendererId)->setWindowSurface(env, surface);
//}
//
//void setPBufferSurface(int32_t width, int32_t height, int32_t rendererId) {
//    renderers.at(rendererId)->setPBufferSurface(width, height);
//}
//
//void resizeView(int32_t x, int32_t y, int32_t width, int32_t height, int32_t rendererId) {
//    renderers.at(rendererId)->resizeView(x, y, width, height);
//}

// ==================
//        测试
// ==================


BugMediaPlayer *player{};

BugMediaPictureRenderer *testRenderer;

//^^^^^^^^^^^ jni ^^^^^^^^^^^^

extern "C" JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_setWindowSurface(JNIEnv *env, jclass clazz, jobject surface, jint rendererId) {
#ifdef DEBUGIT
    testCreate(getWindow(env, surface));
#else
#endif


    //setWindowSurface(env, surface, rendererId);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_setPBufferSurface(JNIEnv *env, jclass clazz, jint width, jint height,
                                                         jint renderer_id) {
    //setPBufferSurface(width, height, renderer_id);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_destroy(JNIEnv *env, jclass clazz, jint renderer_id) {
    //removeRenderer(renderer_id);

    //delete player;

    delete testRenderer;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_pause(JNIEnv *env, jclass clazz, jint renderer_id) {
    player->pause();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_resizeView(JNIEnv *env, jclass thiz, jint x, jint y, jint width, jint height,
                                                  jint renderer_id) {
    //resizeView(x, y, width, height, renderer_id);
    player->resizeView(x, y, width, height);
    //testRenderer->resizeView(x, y, width, height);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_bugmedia_media_BugMediaBridge_createPictureRenderer(JNIEnv *env, jclass thiz, jbyteArray data, jint width,
                                                             jint height) {
    jbyte *bytes = env->GetByteArrayElements(data, nullptr);
    if (bytes == nullptr) {
        return -1;
    }
    int len = env->GetArrayLength(data);
    auto *buf = (uint8_t *) calloc(len, sizeof(uint8_t));
    if (buf == nullptr) {
        return -1;
    }
    for (int i = 0; i < len; i++) {
        *(buf + i) = (uint8_t) (*(bytes + i));

    }
    //释放资源
    env->ReleaseByteArrayElements(data, bytes, 0);

    int rId = -1;
    //rId = createPictureRenderer(buf, width, height);

    return rId;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_startRenderer(JNIEnv *env, jclass thiz, jint renderer_id) {

    //startRenderer(renderer_id);
    testRenderer->render();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_play(JNIEnv *env, jclass clazz, jint renderer_id) {
#ifdef DEBUGAPP
    LOGD("播放");
#endif

    player->play();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_stop(JNIEnv *env, jclass clazz, jint renderer_id) {
    player->stop();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_createPlayer(JNIEnv *env, jclass clazz, jstring url, jobject surface,
                                                    jint width, jint height, jint decoderBufferSize,
                                                    jboolean createPBufferSurface) {
    player = new BugMediaPlayer(env->GetStringUTFChars(url, nullptr),
                                decoderBufferSize, env,
                                surface, width, height);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_BugMediaBridge_load(JNIEnv *env, jclass clazz, jint rendererId) {
    player->load();
}


JNIEnv *nenv;
JavaVM *javaVm;
jobject sur;
int width1;
int height1;
uint8_t *buff;

static void *create(void *pVoid) {
    LOGD("附加开始");
    javaVm->AttachCurrentThread(&nenv, nullptr);
    LOGD("附加成功");
    testRenderer = new BugMediaPictureRenderer(buff, width1, height1,nenv,sur);
    return nullptr;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_bugmedia_media_BugMediaBridge_createPictureRenderer1(JNIEnv *env, jclass clazz, jbyteArray data, jint width,
                                                              jint height, jobject surface) {
    jbyte *bytes = env->GetByteArrayElements(data, nullptr);
    if (bytes == nullptr) {
        return -1;
    }
    int len = env->GetArrayLength(data);
    auto *buf = (uint8_t *) calloc(len, sizeof(uint8_t));
    if (buf == nullptr) {
        return -1;
    }
    for (int i = 0; i < len; i++) {
        *(buf + i) = (uint8_t) (*(bytes + i));

    }
    //释放资源
    env->ReleaseByteArrayElements(data, bytes, 0);

    int rId = -1;
    //rId = createPictureRenderer1(buf, width, height,env,surface);


    env->GetJavaVM(&javaVm);
    sur=env->NewGlobalRef(surface);
    buff=buf;
    width1=width;
    height1=height;

    pthread_t pthread;
    pthread_create(&pthread, nullptr, create, nullptr);

    // C++线程不行
//    thread tt([buf,width,height](JavaVM *javaVm1,JNIEnv *env1,jobject surface1){
//        LOGD("线程里创建");
//JNIEnv * eenv;
//        int ret =javaVm1->AttachCurrentThread(&eenv, nullptr);
//        LOGD("附加结果：%d",ret);
//
//        //jclass jclass1 = nenv->GetObjectClass(s);
//
//        testRenderer = new BugMediaPictureRenderer(buf, width, height,eenv,surface1);
//    },javaVm,nenv,sur);



    //testRenderer = new BugMediaPictureRenderer(buf, width, height,env,surface);

    return rId;
}

