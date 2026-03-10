#include "ui/AnimationClock.h"
#include <spdlog/spdlog.h>
#include <QGuiApplication>
#include <QScreen>

AnimationClock::AnimationClock() : QObject(), mTimer(), mElapsedTimer() {
    
    // Try to set refresh rate to primary screen's native value
    auto* screen = QGuiApplication::primaryScreen(); 
    auto fps = screen->refreshRate();
    this->setFrameRate(fps);

    // Update fps when screen refresh rate changes
    connect(screen, &QScreen::refreshRateChanged, this, [=](qreal fps){
        this->setFrameRate(fps);
    });

    connect(&mTimer, &QTimer::timeout, this, [=]{
        emit tick( mElapsedTimer.restart() / 1000.0f );
    });
}

void AnimationClock::setFrameRate(int frameRate) {
    if (frameRate < 1)
        spdlog::warn("[ui/AnimationClock.cpp] Tried to set fps less than 1");
    
    if (frameRate < MIN_FRAME_RATE) frameRate = MIN_FRAME_RATE;
    if (frameRate > MAX_FRAME_RATE) frameRate = MAX_FRAME_RATE;

    mFrameRate = frameRate;
    mTimer.setInterval(1000/frameRate); // delay in ms
}

int AnimationClock::getFrameRate() const {
    return mFrameRate;
}

void AnimationClock::setAnimationsEnabled(bool enabled) {
    mAnimationsEnabled = enabled;
    if (enabled && mRunningAnimCount > 0) 
        mTimer.start();
    else 
        mTimer.stop();
}

bool AnimationClock::isAnimationsEnabled() const {
    return mAnimationsEnabled;
}


void AnimationClock::incrementRunningCount() {
    if (!mAnimationsEnabled) return;

    mRunningAnimCount++;
    if (mRunningAnimCount == 1) {
        mElapsedTimer.restart();
        mTimer.start();
    }
}
void AnimationClock::decrementRunningCount() {
    if (mRunningAnimCount > 0)
        mRunningAnimCount--;

    if (mRunningAnimCount == 0)
        mTimer.stop();
}

AnimationHandle* const AnimationClock::resumeAnimation() {
    if (!mAnimationsEnabled) return nullptr;

    auto* handle = new AnimationHandle;
    handle->start(); // increments running count

    return new AnimationHandle;
}

void AnimationHandle::start() {
    if (!mRunning) {
        AnimationClock::getInstance().incrementRunningCount();
        mRunning = true;
    }
}
void AnimationHandle::stop() {
    if (mRunning) {
        AnimationClock::getInstance().decrementRunningCount();
        mRunning = false;
    }
}
AnimationHandle::~AnimationHandle() {
    if (mRunning) {
        AnimationClock::getInstance().decrementRunningCount();
    }
}
bool AnimationHandle::isRunning() {
    return mRunning;
}    