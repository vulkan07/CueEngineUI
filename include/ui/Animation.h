#pragma once

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

// If something uses animation frames, it should have an instance of this class
// call start() and stop() to indicate the animation clock how many animations are running
class AnimationHandle {
private:
    bool mRunning = false;
public:
    AnimationHandle() = default;
    ~AnimationHandle();
    void start();
    void stop();
    bool isRunning();
};

// This class stores a global clock that ticks at a set framerate, to animate the whole application
// it calls every object that connects to its tick signal, also providing delta time
// It stops if no animations are running
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

    void incrementRunningCount();
    void decrementRunningCount();

public:
    static constexpr int MIN_FRAME_RATE = 20;
    static constexpr int DEFAULT_FRAME_RATE = 60;
    static constexpr int MAX_FRAME_RATE = 200;

    // Singleton //
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

static constexpr float PIXEL_SNAP_THERSHOLD = 0.2f;


static inline float lerpSpeedBoost(float speed, float dist, float boost) {
    return speed + boost*1.0f/(dist+.1f);
}

static inline float decayToLerpConstant(float k, float dt) {
    return k * dt / (1.0f + k * dt);
}

static inline float lerp(float value, float target, float f) {
    return value * (1.0 - f) + (target * f);
}