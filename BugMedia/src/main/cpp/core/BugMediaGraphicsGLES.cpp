//
// Created by Gshine on 2021/1/26.
//

#include "BugMediaGraphicsGLES.h"
#include "BugMediaGraphicsCommon.h"

BugMediaGraphicsGLES::BugMediaGraphicsGLES(GLuint vertexShader, GLuint fragmentShader) {

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

GLuint BugMediaGraphicsGLES::getProgram() {
    return program;
}

void BugMediaGraphicsGLES::release() {
    if (program != 0) {
        glDeleteProgram(program);
        program = 0;
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
