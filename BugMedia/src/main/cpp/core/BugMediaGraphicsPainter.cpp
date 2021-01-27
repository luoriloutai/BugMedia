//
// Created by Gshine on 2021/1/27.
//

#include "BugMediaGraphicsPainter.h"

BugMediaGraphicsPainter::BugMediaGraphicsPainter(IBugMediaGraphicsRender *render) {
    this->render=render;
    render->setViewPort(0.0,500,300);
    render->setWindowSurface(JNIEnv,jsurface);
    render->draw();
}

BugMediaGraphicsPainter::~BugMediaGraphicsPainter() {

}

void BugMediaGraphicsPainter::paint() {

}
