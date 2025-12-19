#pragma once

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

// AnimationClock returns instances of these to objects who ask ticking for animation
// It's lifetime represents whether the animation is running (destructor/done() means end of animation)
class AnimationHandle {
public:
    AnimationHandle() = default;
    void done(); // just an alias for destructor
    ~AnimationHandle();
};

// This class stores a global clock that ticks at a set framerate,
// and calls every object subscribed to its signal, also supplying it with delta time
// It stops itself if all animations are ended (AnimationHandle objects express animation lifetimes)
class AnimationClock : public QObject {
    Q_OBJECT
    friend class AnimationHandle;
private:
    QTimer mTimer;
    QElapsedTimer mElapsedTimer; 
    int mRunningAnimCount = 0;
    int mFrameRate;
    
    AnimationClock();

    void decrementRunningCount();

public:
    static const int DEFAULT_FRAME_RATE = 20;

    static AnimationClock& getInstance() {
        static AnimationClock animClock;
        return animClock;
    }
    AnimationClock(AnimationClock const&) = delete;
    void operator=(AnimationClock const&) = delete;

    void setFrameRate(int frameRate);
    int getFrameRate() const;

    AnimationHandle* const resumeAnimation();

signals:
    void tick(float dt);
};

static constexpr float PIXEL_SNAP_THERSHOLD = .3f;

// Animation helper functions

static inline float decayToLerpConstant(float k, float dt) {
    return k * dt / (1.0f + k * dt);
}

static inline float lerp(float value, float target, float f) {
    return value * (1.0 - f) + (target * f);
}