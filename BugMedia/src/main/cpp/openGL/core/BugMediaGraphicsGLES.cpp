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


void BugMediaGraphicsGLES::setShaderSource(const GLchar * vertexShadersource,
                                           const GLchar * fragmentShadersource) {

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


GLuint
BugMediaGraphicsGLES::setVertexAttribArray(const GLchar *name, GLint vertexDim, GLenum eleType, GLboolean normalized,
                                           GLsizei stride, const void *pointer) {
//    // 分配顶点缓冲区
//    GLuint VBO;
//    glGenBuffers(1, &VBO);
//
//    // 可绑不同类型的多个Buffer
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//
//    //  将顶点数据放入Buffer,之后的操作都从这里取数据
//    // * GL_STATIC_DRAW ：数据不会或几乎不会改变。
//    // * GL_DYNAMIC_DRAW：数据会被改变很多。
//    // * GL_STREAM_DRAW ：数据每次绘制时都会改变。
//    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexCoords, GL_STATIC_DRAW);

    GLuint attribPosition = glGetAttribLocation(pProgram->instance(), name);

    // 告诉OpenGL该如何解析顶点数据,为着色器属性变量设置值。
    // 该方法将从顶点缓冲区中获取数据，具体是哪个缓冲区取决于之前绑定的缓冲区
    //
    // 第二个参数表示一个点由几个元素构成
    // 第三个参数是坐标数组元素的数组类型
    // 第四个参数表示是否标准化坐标，即把坐标映射到0到1之间。
    // 第五个参数是步长，或叫跨度，即一个顶点所占用的字节数,即 顶点维度*数组中每个元素的大小
    // 第六个参数表示位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0。
    //glVertexAttribPointer(aPosition, vertexDim, GL_FLOAT, GL_FALSE, vertexDim * eleSize, (void *) 0);
    glVertexAttribPointer(attribPosition, vertexDim, eleType, normalized, stride, pointer);
    // 启用顶点属性变量，默认是禁止的
    glEnableVertexAttribArray(attribPosition);

    return attribPosition;
}

void BugMediaGraphicsGLES::enable(GLenum cap) {
    glEnable(cap);
}

void BugMediaGraphicsGLES::blendFunc(GLenum sfactor, GLenum dfactor) {
    glBlendFunc(sfactor, dfactor);
}

void BugMediaGraphicsGLES::clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    glClearColor(red, green, blue, alpha);
}

void BugMediaGraphicsGLES::clear(GLbitfield mask) {
    glClear(mask);
}

void BugMediaGraphicsGLES::drawArrays(GLenum mode, GLint first, GLsizei count) {
    glDrawArrays(mode, first, count);
}

void BugMediaGraphicsGLES::drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
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


//
// Shader
//

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
        const GLchar * shaderName=type==GL_VERTEX_SHADER?"vertex":"fragment";
        LOGD("%s shader 开始释放资源",shaderName);
#endif
        glDeleteShader(handler);
        isRelease = GL_TRUE;

    }
#ifdef DEBUGAPP
    const GLchar * shaderName=type==GL_VERTEX_SHADER?"vertex":"fragment";
    LOGD("%s shader 释放资源完毕",shaderName);
#endif
}

void BugMediaGraphicsGLES::Shader::init() {
#ifdef DEBUGAPP
    const char* shaderName=type == GL_VERTEX_SHADER ? "vertex" : "fragment";
    LOGD("%s Shader创建开始\n", shaderName);

#endif
    handler = glCreateShader(type);

    if (handler != 0) {
#ifdef DEBUGAPP
        LOGD("%s Shader创建完成 handler： %d\n", shaderName, handler);
#endif
        glShaderSource(handler, 1, &source, NULL);
#ifdef DEBUGAPP
        LOGD("%s Shader载入源码完成:\n%s", shaderName,source);
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
LOGD("%s shader 初始化流程完成，如有错误参考前面信息\n",shaderName);
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
