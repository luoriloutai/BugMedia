//
// Created by Gshine on 2021/2/7.
//

#ifndef SLOWER_BUGMEDIAPLAYER_H
#define SLOWER_BUGMEDIAPLAYER_H

#include "BugMediaSLESAudioRenderer.h"
#include "BugMediaGLESVideoRenderer.h"
#include "BugMediaVideoLoader.h"

class BugMediaPlayer {

    BugMediaRenderer *audioRenderer{};
    BugMediaRenderer *videoRenderer{};
    BugMediaVideoLoader *videoLoader{};

public:
    BugMediaPlayer(const char *url);
    ~BugMediaPlayer();
    void play();
    void pause();
    void stop();
};


#endif //SLOWER_BUGMEDIAPLAYER_H
