//
// Created by Gshine on 2021/2/5.
//

#ifndef SLOWER_BUGMEDIASTATECHANGEDCALLBACK_H
#define SLOWER_BUGMEDIASTATECHANGEDCALLBACK_H


class BugMediaStateChangedCallback {
    void *context{};
public:
    BugMediaStateChangedCallback(void * ctx);
    void stateChanged(void *obj);
};


#endif //SLOWER_BUGMEDIASTATECHANGEDCALLBACK_H
