#pragma once

#include <QWidget>
#include <vector>


typedef uint16_t asample_t;
constexpr int SAMPLE_MAX_VALUE = UINT16_MAX;
constexpr int DEFAULT_SAMPLE_RATE = 44100;

template <typename T> 
struct MonoSample {
    T valueLinear;
    T valueRMS;
};

template <typename T> 
struct StereoSample {
    MonoSample<T> sampleL;
    MonoSample<T> sampleR;
};

template <typename T> 
struct WaveformData {
    int bitrate=44100;
    std::vector<MonoSample<T>> samples{};
};


class WaveformViewportWidget : public QWidget  {
    Q_OBJECT

public:
    WaveformViewportWidget(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // scale=1 -> full waveform
    // scale>1 -> zoom in 
    void setScale(float scale); 
    float getScale();

    void setWaveformData(WaveformData<asample_t>* data);
    WaveformData<asample_t>& getWaveformData();

    void recomputeDisplayedWaveform();
    static inline unsigned char remapSample(asample_t sample);
private:
    float mScale = 1;
    WaveformData<asample_t>* mWaveformData = nullptr;
    WaveformData<unsigned char>* mDisplayedWaveform; // To cache values on each resize event instead of each repaint

public slots:
    void setSampleIndex(uint n);
};