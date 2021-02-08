//
// Created by Gshine on 2021/2/7.
//

#ifndef SLOWER_BUGMEDIAAUDIORENDERER_H
#define SLOWER_BUGMEDIAAUDIORENDERER_H


#include "BugMediaVideoLoader.h"

class BugMediaAudioRenderer {
    State currentState = UNSTART;
    BugMediaVideoLoader *videoLoader{};
public:
    void play();

    void pause();

    void stop();

    BugMediaAudioRenderer(BugMediaVideoLoader *loader);
};


#endif //SLOWER_BUGMEDIAAUDIORENDERER_H
