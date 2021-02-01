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
}


void BugMediaGraphicsGLES::release() {

    if (!isRelease) {
#ifdef DEBUGAPP
        LOGD("GLES开始释放资源");
#endif
        delete pFragmentShader;
        pFragmentShader = NULL;
        delete pVertexShader;
        pVertexShader = NULL;
        delete pProgram;
        pProgram = NULL;

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

    if (pProgram != NULL) {
        if (pProgram->instance() == 0) {
            LOGE("program未成功初始化，不能激活");
            return;
        }
        pProgram->active();
    }
}

//
// 不使用缓冲区为顶点属性赋值
//
// name：顶点着色器中属性的名字
// attribDim:属性维度
// eleType:数组元素类型
// normalized：是否标准化将坐标映射到0到1
// stride：步长，跨度，即一个数据占多大，当多种数据都放在一个数组中时用来跳过一组数据
// array：数组对象
GLuint
BugMediaGraphicsGLES::setVertexAttribArray(const GLchar *name, GLint attribDim, GLenum eleType, GLboolean normalized,
                                           GLsizei stride,const void *array) {

    GLuint attribPosition = glGetAttribLocation(pProgram->instance(), name);
    glVertexAttribPointer(attribPosition, attribDim, eleType, normalized, stride, array);

    // 启用顶点属性变量，默认是禁止的
    glEnableVertexAttribArray(attribPosition);
    return attribPosition;
}

void BugMediaGraphicsGLES::drawArrays(GLenum mode, GLint first, GLsizei count) {
    setViewport(viewport);
    glDrawArrays(mode, first, count);
}

void BugMediaGraphicsGLES::drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
    setViewport(viewport);
    glDrawElements(mode, count, type, indices);
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

void BugMediaGraphicsGLES::resize(GLint x, GLint y, GLint width, GLint height) {
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;


}

void BugMediaGraphicsGLES::setViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
}

void BugMediaGraphicsGLES::setViewport(BugMediaGraphicsGLES::Viewport v) {
    glViewport(v.x, v.y, v.width, v.height);
}

GLuint
BugMediaGraphicsGLES::set2DTexture0(const GLchar *uniformTexSamplerName, uint8_t *data, GLint width, GLint height) {
    GLuint textureId = -1;
    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // 给纹理对象设置数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    GLuint samplerId = glGetUniformLocation(pProgram->instance(), uniformTexSamplerName);
    // 将激活的纹理单元传送到着色器中,相当于给着色器中的sampler赋值。
    // 第二个参数表示激活的是哪个纹理单元，这取决于前面glActiveTexture()参数，
    // GL_TEXTURE[n]后面的数字就是第二个参数的值。
    glUniform1i(samplerId, 0);


    return textureId;
}

// 解绑纹理单元并删除纹理
void BugMediaGraphicsGLES::unbind2DTexture0(GLuint *texLocation) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, texLocation);
}

GLuint BugMediaGraphicsGLES::getAttribLocation(const GLchar *name) {
    GLuint location = glGetAttribLocation(pProgram->instance(),name);
    return location;
}


//
// Shader
//

// 这里字符串source要传指针，不能是指针的指针。
BugMediaGraphicsGLES::Shader::Shader(GLenum shaderType, const GLchar *source) {
    this->type = shaderType;
    this->source = source;

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
        glShaderSource(handler, 1, &source, NULL);
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
            glGetShaderInfoLog(handler, infoLen, NULL, logInfo);
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
            glGetProgramInfoLog(handler, len, NULL, logInfo);
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
