//
// Created by Gshine on 2021/2/4.
//

#include "BugMediaAudioDecoder.h"

BugMediaAudioFrame *BugMediaAudioDecoder::getFrame() {
    return nullptr;
}

bool BugMediaAudioDecoder::end() const {
    return isEnd;
}
