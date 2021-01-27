//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphicsGLES.h"
#include "BugMediaGraphicsCommon.h"

//
// BugMediaGraphicsGLES
//
BugMediaGraphicsGLES::BugMediaGraphicsGLES() {


}


void BugMediaGraphicsGLES::release() {
    if (!isRelease) {
        delete pFragmentShader;
        pFragmentShader = NULL;
        delete pVertexShader;
        pVertexShader = NULL;
        delete pProgram;
        pProgram = NULL;

        isRelease = GL_TRUE;
    }

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

void BugMediaGraphicsGLES::setViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    glViewport(x, y, width, height);

}


void BugMediaGraphicsGLES::setShaderSource(const GLchar **const vertexShadersource,
                                           const GLchar **const fragmentShadersource) {
    pVertexShader = new Shader(GL_VERTEX_SHADER, vertexShadersource);
    pFragmentShader = new Shader(GL_FRAGMENT_SHADER, fragmentShadersource);
    pProgram = new Program(pVertexShader, pFragmentShader);
}

void BugMediaGraphicsGLES::activeProgram() {
    if (pProgram != NULL) {
        pProgram->active();
    }
}


//
// Shader
//

BugMediaGraphicsGLES::Shader::Shader(GLenum shaderType, const GLchar *const *source) {
    handler = glCreateShader(shaderType);
    if (handler != 0) {
        glShaderSource(handler, 1, source, NULL);
        glCompileShader(handler);
        GLint compiled = 0;
        glGetShaderiv(handler, GL_COMPILE_STATUS, &compiled);
        if (compiled == 0) {
            LOGE("Could not compile shader %s:\n", shaderType);
            GLint infoLen = 0;
            glGetShaderiv(handler, GL_INFO_LOG_LENGTH, &infoLen);
            GLchar logInfo[infoLen];
            glGetShaderInfoLog(handler, infoLen, NULL, logInfo);
            LOGE("%s", logInfo);
            glDeleteShader(handler);
            handler = 0;
        }
        throw "初始化着色器失败，着色器类型：" + shaderType;
    }
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
        glDeleteShader(handler);
        isRelease = GL_TRUE;

    }
}


//
// Program
//

BugMediaGraphicsGLES::Program::Program(BugMediaGraphicsGLES::Shader *vertexShader,
                                       BugMediaGraphicsGLES::Shader *fragmentShader) {
    handler = glCreateProgram();
    if (handler != 0) {
        glAttachShader(handler, vertexShader->instance());
        if (!checkGLError("glAttachShader")) {
            throw "附加顶点着色器失败";
        }
        glAttachShader(handler, fragmentShader->instance());
        if (!checkGLError("glAttachShader")) {
            throw "附加片元着色器失败";
        }
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
    }

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
        glDeleteProgram(handler);
        isRelease = GL_TRUE;
    }


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
