#pragma once

#include <QWidget>

class DBMeter : public QWidget {
    Q_OBJECT

public:
    DBMeter(QWidget* parent=nullptr);

    void paintEvent(QPaintEvent* event) override;
    void setLevels(float L, float R);
    float getLevel_L();
    float getLevel_R();

    QColor frameColor{100,100,100};
    QGradientStops gradient{
        { 0.3, {0  ,200,  0} },
        { 0.7, {255,220,  0} },
        { 0.8, {255,220,  0} },
        { 1.0, {255,0  ,  0} }
    };

    // Moving average is used for smoothing the wave
    // `false` -> 2 sample window
    // `true` -> 3 sample window
    // TODO revise this when actual audio playback is done
    void setSmoothing(bool smoothing);
    bool smoothing(); // Is smoothing enabled

private:

    bool mSmoothing = false;

    float mSampleL=0;
    float mSampleR=0;
    float mLevelL=0;
    float mLevelR=0;

    // Used for smoothing the bars (averaging)
    float mSampleL_old=0;
    float mSampleR_old=0;

    const int CLIP_DECAY = 20;
    int clipDecayTimer = 0;
};