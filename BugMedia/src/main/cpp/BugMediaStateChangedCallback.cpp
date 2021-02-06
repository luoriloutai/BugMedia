//
// Created by Gshine on 2021/2/5.
//

#include "BugMediaStateChangedCallback.h"
#include "openGL/BugMediaVideoRenderer.h"

void BugMediaStateChangedCallback::stateChanged(void *obj) {
    ((BugMediaVideoRenderer *) context)->stop();
}

BugMediaStateChangedCallback::BugMediaStateChangedCallback(void *ctx) {
    context = ctx;
}
