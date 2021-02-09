//
// Created by Gshine on 2021/2/9.
//

#ifndef SLOWER_BUGMEDIARENDERER_H
#define SLOWER_BUGMEDIARENDERER_H

class BugMediaRenderer {
public:
    virtual void render() = 0;

    virtual void play() = 0;

    virtual void pause() = 0;

    virtual void stop() = 0;

    virtual ~BugMediaRenderer() {};
};

#endif //SLOWER_BUGMEDIARENDERER_H
