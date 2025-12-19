#include "ui/AnimationClock.h"

AnimationHandle::~AnimationHandle() {
    AnimationClock::getInstance().decrementRunningCount();
}

AnimationClock::AnimationClock() : QObject(), mTimer(), mElapsedTimer() {
    this->setFrameRate(DEFAULT_FRAME_RATE);

    connect(&mTimer, &QTimer::timeout, this, [=]{
        emit tick( mElapsedTimer.restart() / 1000.0f );
    });
}

void AnimationClock::setFrameRate(int frameRate) {
    mFrameRate = frameRate;
    mTimer.setInterval(1000/frameRate); // delay in ms
}

int AnimationClock::getFrameRate() const {
    return mFrameRate;
}

void AnimationClock::decrementRunningCount() {
    if (--mRunningAnimCount <= 0) {
        mTimer.stop();
    }
}

AnimationHandle* const AnimationClock::resumeAnimation() {
    if (mRunningAnimCount++ <= 0) {
        mElapsedTimer.restart();
        mTimer.start();
    }
    return new AnimationHandle;
}

void AnimationHandle::done() {
    delete this;
}