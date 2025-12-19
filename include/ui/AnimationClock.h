#pragma once

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

// AnimationClock returns instances of these to objects who ask ticking for animation
// It's lifetime represents whether the animation is running (destructor/done() means end of animation)
class AnimationHandle {
public:
    AnimationHandle() = default;
    ~AnimationHandle();
    void done(); // just an alias for destructor
};

// This class stores a global clock that ticks at a set framerate,
// and calls every object subscribed to its signal, also supplying it with delta time
// It stops itself if all animations are ended (AnimationHandle objects express animation lifetimes)
class AnimationClock : public QObject {
    Q_OBJECT
    friend class AnimationHandle;

private:
    bool mAnimationsEnabled = true;

    QTimer mTimer;
    QElapsedTimer mElapsedTimer; 
    int mRunningAnimCount = 0;
    int mFrameRate;
    
    AnimationClock();

    void decrementRunningCount();

public:
    static const int MIN_FRAME_RATE = 20;
    static const int DEFAULT_FRAME_RATE = 60;
    static const int MAX_FRAME_RATE = 144;

    static AnimationClock& getInstance() {
        static AnimationClock animClock;
        return animClock;
    }
    AnimationClock(AnimationClock const&) = delete;
    void operator=(AnimationClock const&) = delete;

    void setFrameRate(int frameRate);
    int getFrameRate() const;


    void setAnimationsEnabled(bool enabled);
    bool isAnimationsEnabled() const;

    // Returns a handle whose lifetime indicates that the animation is running, or nullptr if animations are disabled
    AnimationHandle* const resumeAnimation();


signals:
    void tick(float dt);
};



// ------------- Animation helpers ------------- //

static constexpr float PIXEL_SNAP_THERSHOLD = 0.3f;


static inline float lerpSpeedBoost(float speed, float dist, float boost) {
    return speed + boost*1.0f/(dist+.1f);
}

static inline float decayToLerpConstant(float k, float dt) {
    auto q = (1.0f + k * dt);
    return k * dt / q;
}

static inline float lerp(float value, float target, float f) {
    return value * (1.0 - f) + (target * f);
}