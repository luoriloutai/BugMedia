//
// Created by Gshine on 2021/1/27.
//

// debug
// 使用Debug.h调试，最后注释或移除这一项即可
//#define DEBUGIT



#ifdef DEBUGIT

#include "../Debug.h"

#else

#include "core/BugMediaBaseRenderer.h"
#include <jni.h>
#include "BugMediaGraphicsPainter.h"
#include "BugMediaTriangleRenderer.h"
#include "core/BugMediaGraphicsCommon.h"
#include "BugMediaPictureRenderer.h"
#include "BugMediaVideoRenderer.h"
#include <vector>
#include <mutex>
#include <map>

using namespace std;

#endif

extern "C" {

//
//// 稍后移除
// 这个要与Java里的值对应
enum rendererType {
    TRIANGLE_RENDERER = 1,
    BITMAP_RENDERER = 2,
    VIDEO_RENDERER = 3
};

//
//// 稍后移除
//
BugMediaBaseRenderer *renderer = nullptr;

//
//// 稍后移除
//
void initRenderer(int renderType) {

#ifdef DEBUGIT

#else
    if (renderType == TRIANGLE_RENDERER) {
        renderer = new BugMediaTriangleRenderer();
    } else if (renderType == BITMAP_RENDERER) {
        //renderer = new BugMediaPictureRenderer();

    } else if (renderType == VIDEO_RENDERER) {

    }
#endif

}


// 记录放入存储的对象在存储区的索引，并赋给对象id
// 目的是为了直接就可以以下标访问而不需要查询。
// 共享变量，注意加锁
static int rendererIndex = -1;
mutex lockObj;
map<int, BugMediaBaseRenderer &> renderers;

void addRenderer(BugMediaBaseRenderer &renderer) {
    rendererIndex++;
    renderer.id = rendererIndex;
    pair<int, BugMediaBaseRenderer &> pair(rendererIndex, renderer);
    renderers.insert(pair);

}

void removeRenderer(int idx) {
    lock_guard<mutex> lockGuard(lockObj);
    if (idx < 0) {
        return;
    }
    //renderers[idx].release(); // 这个不行，报错说引用的那个类型需要一个初始化器，有空研究吧
    renderers.at(idx).release();
    renderers.erase(idx);
}

// 创建三角形渲染器
int createTriangleRenderer() {
    lock_guard<mutex> lockGuard(lockObj);

    // tmd C++无参构造函数这样调用，不需要加个括号
    BugMediaTriangleRenderer newRenderer;
    addRenderer(newRenderer);
    return newRenderer.id;
}

// 创建图像渲染器
int createPictureRenderer(uint8_t *data, GLint width, GLint height) {
    lock_guard<mutex> lockGuard(lockObj);

    BugMediaPictureRenderer newRenderer(data, width, height);
    addRenderer(newRenderer);
    return newRenderer.id;
}

// 创建视频渲染器
int createVideoRenderer() {
    lock_guard<mutex> lockGuard(lockObj);

    BugMediaVideoRenderer newRenderer;
    addRenderer(newRenderer);
    return newRenderer.id;
}


//// extern C
} // extern C


//^^^^^^^^^^^ jni ^^^^^^^^^^^^

extern "C" JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_setWindowSurface(JNIEnv *env, jobject clazz, jobject surface) {
#ifdef DEBUGIT
    testCreate(getWindow(env, surface));
#else
    if (renderer != NULL) {
        renderer->setWindowSurface(env, surface);
    }
#endif
}


extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_setPBufferSurface(JNIEnv *env, jobject clazz, jint width, jint height) {
#ifdef DEBUGIT
#else
    if (renderer != NULL) {
        renderer->setPBufferSurface(width, height);
    }
#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_draw(JNIEnv *env, jobject clazz) {
#ifdef DEBUGIT
#else
    if (renderer != NULL) {
        renderer->draw();
    }
#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_selectRenderer(JNIEnv *env, jobject clazz, jint rendererType) {
#ifdef DEBUGIT

#else
    initRenderer(rendererType);
    if (renderer == NULL) {
        throw "初始化渲染器失败";

#ifdef DEBUGAPP

#endif
        LOGD("渲染器初始化成功");
    }

#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_destroy(JNIEnv *env, jobject clazz) {
#ifdef DEBUGIT
#else
    
        delete renderer;

#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_pause(JNIEnv *env, jobject clazz) {
#ifdef DEBUGIT

#else

#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bugmedia_media_GraphicsBridge_resize(JNIEnv *env, jobject thiz, jint x, jint y, jint width, jint height) {
    renderer->resize(x, y, width, height);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_bugmedia_media_GraphicsBridge_createPictureRenderer(JNIEnv *env, jobject thiz, jbyteArray data, jint width,
                                                             jint height) {
    jbyte* bytes = env->GetByteArrayElements(data,nullptr);
    if(bytes == nullptr) {
        return -1;
    }
    int len=env->GetArrayLength(data);
    auto *buf =(uint8_t *)calloc(len,sizeof(uint8_t));
    if(buf == nullptr)
    {
        return -1;
    }
    for(int i=0;i<len;i++)
    {
        *(buf+i)=(uint8_t)(*(bytes+i));

    }
    //释放资源
    env->ReleaseByteArrayElements(data,bytes,0);
    //free(buf);
    renderer = new BugMediaPictureRenderer(buf, width, height);
#ifdef DEBUGAPP
    LOGD("创建渲染器完成,数据长度为：%d",len);
#endif
    return -1;
}