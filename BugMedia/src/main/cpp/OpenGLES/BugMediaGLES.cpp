//
// Created by Gshine on 2021/3/3.
//

#include "BugMediaGLES.h"
#include "../BugMediaCommon.h"


BugMediaGLES::BugMediaGLES(ANativeWindow *nativeWindow) {
    pEGL = new BugMediaEGL(nativeWindow);
}

BugMediaGLES::~BugMediaGLES() {
    glDeleteProgram(glProgram);
    delete pEGL;
}

void BugMediaGLES::render() {
    //EGL的东西可以在线程外创建，然后在线程中使用makeCurrent()将上下文绑至线程
    //OpenGL的东西必须在同一个线程创建

    pEGL->init();
    pEGL->createDefaultWindowSurface();

    pthread_create(&renderThread, nullptr, renderRoutine, this);
}

GLboolean BugMediaGLES::init() {
    const GLchar *vertexShaderSource = getVertexShaderSource();
    GLuint glVertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    if (glVertexShader == GL_NONE) {
        return GL_FALSE;
    }
    const GLchar *fragmentShaderSource = getFragmentShaderSource();
    GLuint glFragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (glFragmentShader == GL_NONE) {
        return GL_FALSE;
    }
    glProgram = createProgram(glVertexShader, glFragmentShader);
    if (glProgram == GL_NONE) {
        return GL_FALSE;
    }

    return GL_TRUE;

}

GLuint BugMediaGLES::createShader(GLenum type, const GLchar *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == 0) {
        glDeleteShader(shader);
        const GLchar *shaderName = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
        LOGE("Can't compile %s shader:\n", shaderName);
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        GLchar logInfo[infoLen];
        glGetShaderInfoLog(shader, infoLen, nullptr, logInfo);
        LOGE("%s Shader compile error：%s\n", shaderName, logInfo);
        return GL_NONE;
    }
    return shader;
}

GLuint BugMediaGLES::createProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    int error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        LOGE("attach vertex shader error:%d", error);
        return GL_NONE;
    }
    error = GL_NO_ERROR;
    glAttachShader(program, fragmentShader);
    if ((error = glGetError()) != GL_NO_ERROR) {
        LOGE("attach fragment shader error:%d", error);
        return GL_NONE;
    }
    glLinkProgram(program);
    GLint linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        GLchar logInfo[len];
        glGetProgramInfoLog(program, len, nullptr, logInfo);
        LOGE("Could not link program: %s", logInfo);

        glDeleteProgram(program);
        program = GL_NONE;
        LOGE("link program failed");
        return GL_NONE;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void BugMediaGLES::useProgram() const {
    glUseProgram(glProgram);
}

GLuint BugMediaGLES::getProgram() const {
    return glProgram;
}

void *BugMediaGLES::renderRoutine(void *pVoid) {
    auto self = (BugMediaGLES *) pVoid;
    self->execRender();
    return nullptr;
}

void BugMediaGLES::execRender() {
    pEGL->makeDefaultWindowSurfaceCurrent();
    useProgram();
    onRender();
    pEGL->unmakeCurrent();
}

EGLBoolean BugMediaGLES::swapDefaultBuffers() {
    return pEGL->swapDefaultBuffers();
}

EGLBoolean BugMediaGLES::unmakeCurrent() {
    return pEGL->unmakeCurrent();
}

void BugMediaGLES::resizeView(GLint x, GLint y, GLsizei width, GLsizei height) {
//    this->x = x;
//    this->y = y;
//    this->width = width;
//    this->height = height;
//    pEGL->makeDefaultWindowSurfaceCurrent();
    glViewport(x,y,width,height);
//    pEGL->unmakeCurrent();
}

EGLBoolean BugMediaGLES::makeDefaultWindowSurfaceCurrent() {
    return pEGL->makeDefaultWindowSurfaceCurrent();
}
