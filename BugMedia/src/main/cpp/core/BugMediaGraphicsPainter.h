//
// Created by Gshine on 2021/1/27.
//

#ifndef SLOWER_BUGMEDIAGRAPHICSPAINTER_H
#define SLOWER_BUGMEDIAGRAPHICSPAINTER_H
#include "BugMediaGraphics.h"
#include "BugMediaGraphicsInterface.h"

class BugMediaGraphicsPainter {
public:
    BugMediaGraphicsPainter(IBugMediaGraphicsRender *render);
    ~BugMediaGraphicsPainter();
void paint();

private:
    IBugMediaGraphicsRender * render=NULL;
};


#endif //SLOWER_BUGMEDIAGRAPHICSPAINTER_H
