//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphicsGLES.h"
#include "BugMediaGraphicsCommon.h"

//
// BugMediaGraphicsGLES
//
BugMediaGraphicsGLES::BugMediaGraphicsGLES() {
    pProgram = new Program();
    pVertexShader = nullptr;
    pFragmentShader = nullptr;
    isRelease = GL_FALSE;
}


void BugMediaGraphicsGLES::release() {

    if (!isRelease) {
#ifdef DEBUGAPP
        LOGD("GLES开始释放资源");
#endif
        delete pFragmentShader;
        pFragmentShader = nullptr;
        delete pVertexShader;
        pVertexShader = nullptr;
        delete pProgram;
        pProgram = nullptr;

        isRelease = GL_TRUE;
    }
#ifdef DEBUGAPP
    LOGD("GLES释放资源完毕");
#endif
}

BugMediaGraphicsGLES::~BugMediaGraphicsGLES() {
    if (!isRelease) {
        release();
    }
}

GLboolean BugMediaGraphicsGLES::checkGLError(const char *op) {
    int error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        LOGE("%s glError:%d", op, error);
        return GL_FALSE;
    }

    return GL_TRUE;
}


void BugMediaGraphicsGLES::setShaderSource(const GLchar *vertexShadersource,
                                           const GLchar *fragmentShadersource) {

    pVertexShader = new Shader(GL_VERTEX_SHADER, vertexShadersource);
    pFragmentShader = new Shader(GL_FRAGMENT_SHADER, fragmentShadersource);

}

void BugMediaGraphicsGLES::activeProgram() {

    if (pProgram != nullptr) {
        if (pProgram->instance() == 0) {
            LOGE("program未成功初始化，不能激活");
            return;
        }
        pProgram->active();
    }
}

void BugMediaGraphicsGLES::init() {
#ifdef DEBUGAPP
    LOGD("GLES初始化");
#endif
    pVertexShader->init();
    pFragmentShader->init();
    pProgram->init(pVertexShader, pFragmentShader);
    //pProgram->active();
#ifdef DEBUGAPP
    LOGD("GLES初始化完毕");
#endif
}


GLuint BugMediaGraphicsGLES::getProgram() {
    return pProgram->instance();
}


//
// Shader
//

// 这里字符串source要传指针，不能是指针的指针。
BugMediaGraphicsGLES::Shader::Shader(GLenum shaderType, const GLchar *source) {
    this->type = shaderType;
    this->source = source;
    handler = 0;
    isRelease = GL_FALSE;

}

GLuint BugMediaGraphicsGLES::Shader::instance() {
    return handler;
}

BugMediaGraphicsGLES::Shader::~Shader() {
    if (!isRelease) {
        release();
    }
}

void BugMediaGraphicsGLES::Shader::release() {
    if (!isRelease) {
#ifdef DEBUGAPP
        const GLchar *shaderName = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
        LOGD("%s shader 开始释放资源", shaderName);
#endif
        glDeleteShader(handler);
        isRelease = GL_TRUE;

    }
#ifdef DEBUGAPP
    const GLchar *shaderName = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
    LOGD("%s shader 释放资源完毕", shaderName);
#endif
}

void BugMediaGraphicsGLES::Shader::init() {
#ifdef DEBUGAPP
    const char *shaderName = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
    LOGD("%s Shader创建开始\n", shaderName);

#endif
    handler = glCreateShader(type);

    if (handler != 0) {
#ifdef DEBUGAPP
        LOGD("%s Shader创建完成 handler： %d\n", shaderName, handler);
#endif
        glShaderSource(handler, 1, &source, nullptr);
#ifdef DEBUGAPP
        LOGD("%s Shader载入源码完成:\n%s", shaderName, source);
#endif
        glCompileShader(handler);

        GLint compiled = 0;
        glGetShaderiv(handler, GL_COMPILE_STATUS, &compiled);
        if (compiled == 0) {
#ifdef DEBUGAPP
            LOGE("不能编译 %s shader:\n", shaderName);
            GLint infoLen = 0;
            glGetShaderiv(handler, GL_INFO_LOG_LENGTH, &infoLen);
            GLchar logInfo[infoLen];
            glGetShaderInfoLog(handler, infoLen, nullptr, logInfo);
            LOGE("%s Shader 编译错误信息：%s\n", shaderName, logInfo);
#endif
            glDeleteShader(handler);
            handler = 0;
        }
    }

#ifdef DEBUGAPP
    LOGD("%s shader 初始化流程完成，如有错误参考前面信息\n", shaderName);
#endif

}




//
// Program
//

BugMediaGraphicsGLES::Program::Program() {
    handler = 0;
    isRelease = GL_FALSE;
}

GLboolean BugMediaGraphicsGLES::Program::checkGLError(const char *op) {
    int error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        LOGE("%s glError:%d", op, error);
        return GL_FALSE;
    }

    return GL_TRUE;
}

void BugMediaGraphicsGLES::Program::release() {
    if (!isRelease) {
#ifdef DEBUGAPP
        LOGD("program 开始释放资源");
#endif
        glDeleteProgram(handler);
        isRelease = GL_TRUE;
    }

#ifdef DEBUGAPP
    LOGD("program 释放资源完毕");
#endif
}

BugMediaGraphicsGLES::Program::~Program() {
    if (!isRelease) {
        release();
    }
}

void BugMediaGraphicsGLES::Program::active() {

    glUseProgram(handler);
}

GLuint BugMediaGraphicsGLES::Program::instance() {
    return handler;
}

void BugMediaGraphicsGLES::Program::init(BugMediaGraphicsGLES::Shader *vertexShader,
                                         BugMediaGraphicsGLES::Shader *fragmentShader) {

#ifdef DEBUGAPP
    LOGD("开始初始化program");
#endif
    if ((handler = glCreateProgram()) == 0) {
#ifdef DEBUGAPP
        LOGE("初始化program失败:%d", glGetError());
#endif
        return;
    }


    if (handler != 0) {
        glAttachShader(handler, vertexShader->instance());
        if (!checkGLError("glAttachShader")) {
            throw "附加顶点着色器失败";
        }
#ifdef DEBUGAPP
        LOGD("附加vertex shader成功");
#endif
        glAttachShader(handler, fragmentShader->instance());
        if (!checkGLError("glAttachShader")) {
            throw "附加片元着色器失败";
        }
#ifdef DEBUGAPP
        LOGD("附加fragment shader成功");
#endif
        glLinkProgram(handler);
        GLint linkStatus = 0;
        glGetProgramiv(handler, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint len = 0;
            glGetProgramiv(handler, GL_INFO_LOG_LENGTH, &len);
            GLchar logInfo[len];
            glGetProgramInfoLog(handler, len, nullptr, logInfo);
            LOGE("Could not link program: %s", logInfo);

            glDeleteProgram(handler);
            handler = 0;
            throw "绘制程序初始化失败";
        }
#ifdef DEBUGAPP
        LOGD("链接program成功");
#endif
    }

#ifdef DEBUGAPP
    LOGD("program初始化完毕");
#endif
}
