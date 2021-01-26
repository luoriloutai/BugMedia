#include <climits>
//
// Created by Gshine on 2021/1/25.
//


/*
 * OpenGL ES是渲染系统，只负责渲染图像，不管理输出。
 *
 * 由于OpenGL ES没有输出系统，所以由各平台自己实现，Android下的输出系统是EGL，要把EGL和OpenGL ES关联起来。
 * EGL接收OpenGL ES渲染的内容，并输出到屏幕
 * */



#include "BugMediaGraphics.h"

#ifdef __cplusplus
extern "C" {
#endif

// c编写的库


#ifdef __cplusplus
}
#endif


/*
 * EGL
 * */
BugMediaGraphicsEGL::BugMediaGraphicsEGL() {
    if (!init()) {
        throw "EGL初始化失败";
    }
}

EGLBoolean BugMediaGraphicsEGL::init() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay error:%d\n", eglGetError());
        return EGL_FALSE;

    }

    EGLBoolean ok = eglInitialize(display, NULL, NULL);
    if (!ok) {
        LOGE("eglInitialize error:%d\n", eglGetError());
        return EGL_FALSE;
    }

    // android下的配置
    const EGLint configAttribs[] = {EGL_BUFFER_SIZE, 32,
                                    EGL_ALPHA_SIZE, 8,
                                    EGL_BLUE_SIZE, 8,
                                    EGL_GREEN_SIZE, 8,
                                    EGL_RED_SIZE, 8,
                                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                    EGL_NONE}; // 必须以EGL_NONE结尾
    EGLint numConfigs = 0;

    // 选择符合配置选项的config
    // config_size:选择符合条件的多少个config输出到传入的参数gConfig中
    // numConfigs:符合条件的配置有多少个，输出到numConfigs
    // 第三个参数可以不传，这时numConfigs的值就是找到的符合条件的eglconfig的个数，
    // 然后，再将numConfigs传到第四个参数，同时传一个接收eglconfig的数组到第三个参数，可以获取所有符合条件的eglconfig
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig() returned error %d", eglGetError());
        return EGL_FALSE;
    }
    if (config == NULL) {
        LOGE("选择配置失败：%d\n", eglGetError());
        return EGL_FALSE;
    }
    EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

    if (!(context = eglCreateContext(display, config, NULL, contextAttribs))) {
        LOGE("eglCreateContext() returned error %d", eglGetError());
        return EGL_FALSE;
    }

    return EGL_TRUE;
}

EGLBoolean BugMediaGraphicsEGL::setPBufferSurface(EGLint width, EGLint height) {

    EGLint PbufferAttributes[] = {
            EGL_WIDTH, width,
            EGL_HEIGHT, height,
            EGL_NONE};
    if (!(PBufferSurface = eglCreatePbufferSurface(display, config, PbufferAttributes))) {
        LOGE("eglCreatePbufferSurface() returned error %d", eglGetError());
        return EGL_FALSE;
    }

    // Surface和Context绑定到当前线程
    //eglMakeCurrent(display, PBufferSurface, PBufferSurface, context);
    return EGL_TRUE;
}

EGLBoolean BugMediaGraphicsEGL::setWindowSurface(JNIEnv *env, jobject jSurface) {

    EGLint format;
    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
        return EGL_FALSE;
    }
    window = ANativeWindow_fromSurface(env, jSurface);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
    if (!(windowSurface = eglCreateWindowSurface(display, config, window, 0))) {
        LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
        return EGL_FALSE;
    }
    //eglMakeCurrent(display, windowSurface, windowSurface, context);
    return EGL_TRUE;
}

void BugMediaGraphicsEGL::release() {
    if (windowSurface != NULL) {
        eglDestroySurface(display, windowSurface);
        windowSurface = NULL;
    }
    if (PBufferSurface != NULL) {
        eglDestroySurface(display, PBufferSurface);
        PBufferSurface = NULL;
    }
    if (context != NULL) {
        eglDestroyContext(display, context);
        context = NULL;
    }
    isRelease = EGL_TRUE;

}

BugMediaGraphicsEGL::~BugMediaGraphicsEGL() {
    if (!isRelease) {
        release();
    }

}

EGLDisplay BugMediaGraphicsEGL::getDisplay() {
    return display;
}

EGLSurface BugMediaGraphicsEGL::getWindowSurface() {
    return windowSurface;
}

EGLSurface BugMediaGraphicsEGL::getPBufferSurface() {
    return PBufferSurface;
}

void BugMediaGraphicsEGL::makeCurrent() {
    if (windowSurface != NULL) {
        eglMakeCurrent(display, windowSurface, windowSurface, context);
    }
    if (PBufferSurface != NULL) {
        eglMakeCurrent(display, PBufferSurface, PBufferSurface, context);
    }
}


/*
 * BugMediaGraphicsShader
 * */
BugMediaGraphicsShader::BugMediaGraphicsShader(GLenum shaderType, const GLchar **const source) {
    shader = glCreateShader(shaderType);
    if (shader != 0) {
        glShaderSource(shader, 1, source, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled == 0) {
            LOGE("Could not compile shader %s:\n", shaderType);
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            GLchar logInfo[infoLen];
            glGetShaderInfoLog(shader, infoLen, NULL, logInfo);
            LOGE("%s", logInfo);
            glDeleteShader(shader);
            shader = 0;
        }
    }

}

GLuint BugMediaGraphicsShader::getShader() {
    return shader;
}

void BugMediaGraphicsShader::release() {
    if (shader != 0) {
        glDeleteShader(shader);
        shader = 0;
        isRelease = GL_TRUE;
    }
}

BugMediaGraphicsShader::~BugMediaGraphicsShader() {
    if (!isRelease) {
        release();
    }
}

/*
 * BugMediaGraphicsProgram
 * */
BugMediaGraphicsProgram::BugMediaGraphicsProgram(GLuint vertexShader, GLuint fragmentShader) {

    program = glCreateProgram();
    if (program != 0) {
        glAttachShader(program, vertexShader);
        if (!checkGLError("glAttachShader")) {
            throw "附加顶点着色器失败";
        }
        glAttachShader(program, fragmentShader);
        if (!checkGLError("glAttachShader")) {
            throw "附加片元着色器失败";
        }
        glLinkProgram(program);
        GLint linkStatus = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint len = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
            GLchar logInfo[len];
            glGetProgramInfoLog(program, len, NULL, logInfo);
            LOGE("Could not link program: %s", logInfo);

            glDeleteProgram(program);
            program = 0;
        }
    }

}

//void BugMediaGraphicsProgram::attachShader(GLuint shader) {}

GLuint BugMediaGraphicsProgram::getProgram() {
    return program;
}

void BugMediaGraphicsProgram::release() {
    if (program != 0) {
        glDeleteProgram(program);
        program = 0;
        isRelease = GL_TRUE;
    }
}

BugMediaGraphicsProgram::~BugMediaGraphicsProgram() {
    if (!isRelease) {
        release();
    }
}

GLboolean BugMediaGraphicsProgram::checkGLError(const char *op) {
    int error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        LOGE("%s glError:%d", op, error);
        return GL_FALSE;
    }

    return GL_TRUE;
}

/*
 * BugMediaGraphics
 * */
BugMediaGraphics::BugMediaGraphics() {
    pEGL = new BugMediaGraphicsEGL();
}

void BugMediaGraphics::setFragmentShader(const char **const source) {
    // 这种情况不用手动释放指针
    //BugMediaGraphicsShader fragShader(GL_FRAGMENT_SHADER, source);
    //pFragmentShader = &fragShader;

    pFragmentShader = new BugMediaGraphicsShader(GL_FRAGMENT_SHADER, source);

}

void BugMediaGraphics::setVertexShader(const char **const source) {
    // 这种情况不用手动释放指针
//    BugMediaGraphicsShader vertShader(GL_VERTEX_SHADER, source);
//    pVertextShader = &vertShader;

    pVertextShader = new BugMediaGraphicsShader(GL_VERTEX_SHADER, source);
}

void BugMediaGraphics::setWindowSurface(JNIEnv *env, jobject jSurface) {
    pEGL->setWindowSurface(env, jSurface);
}

void BugMediaGraphics::setPBufferSurface(EGLint width, EGLint height) {
    pEGL->setPBufferSurface(width, height);
}


void BugMediaGraphics::prepareProgram() {
    if (pVertextShader == NULL || pFragmentShader == NULL) {
        throw "必须先设置着色器：调用setXXXShader()方法";
    }
    GLuint vertexShader = pVertextShader->getShader();
    GLuint fragmentShader = pFragmentShader->getShader();
    pGLESProgram = new BugMediaGraphicsProgram(vertexShader, fragmentShader);
    GLuint program = pGLESProgram->getProgram();

    // 着色器链接到着色程序后就可以删除了
    pVertextShader->release();
    pFragmentShader->release();
    // 自己实现绘制前的准备工作
    onProgramPrepared();
    // 激活着色程序
    glUseProgram(program);
}


void BugMediaGraphics::draw() {

    onDraw();
    //
    EGLDisplay display = pEGL->getDisplay();
    EGLSurface PBufferSurface = pEGL->getPBufferSurface();
    if (PBufferSurface != NULL) {
        eglSwapBuffers(display, PBufferSurface);
    }
    EGLSurface windowSurface = pEGL->getWindowSurface();
    if (windowSurface != NULL) {
        eglSwapBuffers(display, windowSurface);
    }
}

void BugMediaGraphics::release() {
    if (!isRelease) {


        delete pVertextShader;
        pVertextShader = NULL;
        delete pFragmentShader;
        pFragmentShader = NULL;
        delete pGLESProgram;
        pGLESProgram = NULL;
        delete pEGL;
        pEGL = NULL;

        isRelease = GL_TRUE;
    }

}

BugMediaGraphics::~BugMediaGraphics() {
    if (!isRelease) {
        release();
    }
}

void BugMediaGraphics::makeCurrent() {
    pEGL->makeCurrent();
}




