#include "backend/Backend.h"

void Backend::addCue(std::unique_ptr<Cue> cue) {
    mCues.push_back(std::move(cue));
}

Cue* Backend::getCue(int index) {
    if (index < 0 || index >= mCues.size())
        return nullptr;
    return &(*mCues[index]);
}

const std::vector<std::unique_ptr<Cue>>& Backend::getCues() const {
    return mCues;
}


size_t Backend::getLength() {
    return mCues.size();
}