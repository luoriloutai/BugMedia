//
// Created by Gshine on 2021/1/26.
// OpenGL 着色器
//

#include "BugMediaGraphicsShader.h"
#include "BugMediaGraphicsCommon.h"

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