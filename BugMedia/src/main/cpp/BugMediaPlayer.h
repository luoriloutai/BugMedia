//
// Created by Gshine on 2021/2/7.
//

#ifndef SLOWER_BUGMEDIAPLAYER_H
#define SLOWER_BUGMEDIAPLAYER_H

#include "BugMediaAudioRenderer.h"
#include "BugMediaVideoRenderer.h"
#include "BugMediaVideoLoader.h"

class BugMediaPlayer {

    BugMediaAudioRenderer *audioRenderer{};
    BugMediaVideoRenderer *videoRenderer{};
    BugMediaVideoLoader *videoLoader{};

public:
    BugMediaPlayer(const char *url);
    ~BugMediaPlayer();
    void play();
    void pause();
    void stop();
};


#endif //SLOWER_BUGMEDIAPLAYER_H
