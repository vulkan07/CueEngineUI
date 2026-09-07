#include "ui/Waveform.h"

#include <QPaintEvent>
#include <QPainter>

#include <algorithm>

WaveformViewportWidget::WaveformViewportWidget(QWidget* parent) : 
    QWidget(parent), mDisplayedWaveform(new WaveformData<unsigned char>) {
    this->setMinimumHeight(64);
    this->setMinimumWidth(256);
}

void WaveformViewportWidget::setWaveformData(WaveformData<asample_t>* data){
    mWaveformData=data;
}
WaveformData<asample_t>& WaveformViewportWidget::getWaveformData(){
    return *mWaveformData;
}

inline unsigned char WaveformViewportWidget::remapSample(asample_t sample) {
    constexpr float f = 255.0 / SAMPLE_MAX_VALUE;
    return (sample) * f;
}

void WaveformViewportWidget::recomputeDisplayedWaveform() {
    int w = this->width();
    mDisplayedWaveform->samples.resize(w);
    int samples_per_px = ceil((float)mWaveformData->samples.size() / w / mScale);
    qDebug()<<samples_per_px;

    for (int i = 0; i < w; i++) {
        float max = 0;
        float avg = 0;
        float rms = 0;
        std::vector<float> dataset;
        dataset.reserve(samples_per_px);
        for (int j = i*samples_per_px; j < std::min((int)((i+1)*samples_per_px), (int)mWaveformData->samples.size()); j++) {
            int v = mWaveformData->samples[j].valueLinear;
            avg += v;
            rms += v*v;
            if (v > max) max = v;
           // dataset.push_back(mWaveformData->samples[j].valueLinear); 
        }

        std::sort(dataset.begin(),dataset.end());
        const int percentile = 97;
        int index = percentile/100.0f * samples_per_px;
        qDebug() << "spp:" << samples_per_px << ", index:" << index << " " << dataset.size();
        //float pct = dataset[0];

        
        avg /= samples_per_px;
        rms = sqrt(rms/samples_per_px);
        unsigned char r_linear = WaveformViewportWidget::remapSample(max);
        unsigned char r_rms = WaveformViewportWidget::remapSample(rms);
        mDisplayedWaveform->samples[i] = {r_linear, r_rms};
    }
    this->repaint();
}

void WaveformViewportWidget::resizeEvent(QResizeEvent* event) {
    this->recomputeDisplayedWaveform();
    QWidget::resizeEvent(event);
}

void WaveformViewportWidget::paintEvent(QPaintEvent* event){
    if (!mWaveformData) return;
    int w = this->width();
    int h = this->height();
    float h_half = h/2.0f;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setClipRegion(event->region());


    for (int i = 0; i < w; i++) {
        unsigned char vlinear = mDisplayedWaveform->samples[i].valueLinear;
        unsigned char vrms = mDisplayedWaveform->samples[i].valueRMS;
        p.setPen({vlinear+100,30,30});
        p.drawLine(i,(h_half-vlinear),i,h_half+vlinear);

        p.setPen({0,50,255, 255});
        p.drawLine(i,(h_half-vrms),i,h_half+vrms);
    }


    //center line
    p.setPen({100,0,0});
    p.drawLine(0,h_half,w,h_half);
}

void WaveformViewportWidget::setScale(float scale) {
    this->mScale = scale;
    this->recomputeDisplayedWaveform();
}
float WaveformViewportWidget::getScale() {
    return mScale;
}

void WaveformViewportWidget::setSampleIndex(uint n){/*TODO*/}