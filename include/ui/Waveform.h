#pragma once

#include <QWidget>
#include <QColor>

#include <vector>


typedef uint64_t apos_t;
typedef uint16_t asample_t;
constexpr int SAMPLE_MAX_VALUE = UINT16_MAX;
constexpr int DEFAULT_SAMPLE_RATE = 44100;

// Templating to allow any numerical type for samples and waveform data
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
    struct Playhead {
        enum Style{
            PLAIN = 0,
            TOP_MARKER = 1,
            BOTTOM_MARKER = 2,
            LEFT_MARKER = 4,
            RIGHT_MARKER = 8,
        };

        // Style
        QColor color = {255,255,255};
        char style = Playhead::Style::BOTTOM_MARKER | Playhead::Style::TOP_MARKER;
        bool visible = true;

        // State
        apos_t position = 0;
    };

    WaveformViewportWidget(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    //void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;


    // scale=1 -> full waveform
    // scale>1 -> zoom in 
    void setScale(float scale); 
    float getScale();

    void scrollBy(int64_t n_samples_delta); // positive=right
    void setScroll(apos_t n_samples);
    apos_t getScroll();

    void setSingleSidedWaveform(bool v);
    bool isSingleSidedWaveform();

    void setWaveformData(WaveformData<asample_t>* data);
    WaveformData<asample_t>& getWaveformData();

    void recomputeDisplayedWaveform();
    void updateMousePlayhead();

    void setPlaybackPos(apos_t sample);
private:
    bool mSingleSideWaveform = false;
    float mScale = 1;
    apos_t mScroll = 0;
    std::vector<Playhead*> mPlayheads;
    Playhead mPlayheadPlayback;
    Playhead mPlayheadMouse;
    WaveformData<asample_t>* mWaveformData = nullptr;
    WaveformData<unsigned char>* mDisplayedWaveform; // To cache values on each resize event instead of each repaint
};