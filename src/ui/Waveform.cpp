#include "ui/Waveform.h"

#include <QPaintEvent>
#include <QPainter>

#include <algorithm>

WaveformViewportWidget::WaveformViewportWidget(QWidget* parent) : 
    QWidget(parent), mDisplayedWaveform(new WaveformData<unsigned char>) {
    this->setMinimumHeight(48);
    this->setMaximumHeight(255*2); // Waveform cache uses 1 byte so theoretical max height per side is 256
    this->setMinimumWidth(256);

    this->setMouseTracking(true);

    //TEST
    mPlayheadPlayback = {.color={225,0,0},.visible=false};
    mPlayheadMouse = {.color={225,225,225,150},.visible=false};
    mPlayheads.push_back(&mPlayheadMouse);
}

void WaveformViewportWidget::setWaveformData(WaveformData<asample_t>* data){
    mWaveformData=data;
}
WaveformData<asample_t>& WaveformViewportWidget::getWaveformData(){
    return *mWaveformData;
}


void WaveformViewportWidget::recomputeDisplayedWaveform() {
    int w = this->width();
    mDisplayedWaveform->samples.resize(w);
    int samples_per_px = std::max(1.0f, ((float)mWaveformData->samples.size() / w )-mScale);
    //qDebug()<<samples_per_px;

    float remap_factor = (float)this->height() / 2.0 / SAMPLE_MAX_VALUE;

    for (int i = 0; i < w; i++) {
        /*
        constexpr int  maxn = 5;
        int maxi = 0;
        std::vector<float> maxes(maxn);
        */

        float max = 0;
        float avg = 0;
        float rms = 0;
        for (int j = i*samples_per_px + mScroll; j < std::min((int)( (i+1)*samples_per_px +mScroll), (int)mWaveformData->samples.size()); j++) {
            int v = mWaveformData->samples[j].valueLinear;
            avg += v;
            rms += v*v;

            if (v > max) max = v;

        }

        
        avg /= samples_per_px;
        rms = sqrt(rms/samples_per_px);
        float v = avg*.2+max*.8;
        unsigned char r_linear = (unsigned char)(v * remap_factor);
        unsigned char r_rms = (unsigned char)(rms * remap_factor);
        mDisplayedWaveform->samples[i] = {r_linear, r_rms};
    }
    //this->repaint();
}

void WaveformViewportWidget::leaveEvent(QEvent* event) {
    mPlayheadMouse.visible = false;
    this->repaint();
}

void WaveformViewportWidget::wheelEvent(QWheelEvent *event) {
    int steps = event->angleDelta().x() * -200; // (+)=right
    int zoom = event->angleDelta().y();

    this->scrollBy(steps);
    this->setScale(mScale+zoom);
}

void WaveformViewportWidget::mouseMoveEvent(QMouseEvent* event) {

    mPlayheadMouse.visible = true;

    this->updateMousePlayhead();
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
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setClipRegion(event->region());

    bool singleSided = h <= 64 || mSingleSideWaveform;

    // Note: separate regions are not supported this way, only left- and rightmost
    // boundaries are, but this shouldnt be a significant bottlenec enough
    int startPixel = event->region().boundingRect().left();
    int endPixel = event->region().boundingRect().right();
    for (int i = startPixel; i < endPixel; i++) {
        unsigned char vlinear = mDisplayedWaveform->samples[i].valueLinear;
        unsigned char vrms = mDisplayedWaveform->samples[i].valueRMS;
        //p.setPen({vlinear+100,30,30});

        // Linear line
        p.setPen({250,30,30});
        if (singleSided)
            p.drawLine(i,h-vlinear*2,i,h);
        else
            p.drawLine(i,(h_half-vlinear),i,h_half+vlinear);

        // RMS line
        p.setPen({230,80,30, 255});
        if (singleSided)
            p.drawLine(i,h-vrms*2,i,h);
        else
            p.drawLine(i,(h_half-vrms),i,h_half+vrms);
        
    }

    // Playheads
    constexpr int triangleW = 3;
    constexpr int triangleH = 4;
    constexpr int HtriangleW = 8;
    constexpr int HtriangleH = 7;
    for (size_t i = 0; i < mPlayheads.size(); i++) {
        if (!mPlayheads[i]->visible) continue;

        int samples_per_px = std::max(1.0f, ((float)mWaveformData->samples.size() / w )-mScale);
        apos_t pos = (mPlayheads[i]->position - mScroll) / samples_per_px;

        p.setPen(mPlayheads[i]->color);
        p.setBrush(mPlayheads[i]->color);
        p.setRenderHint(QPainter::Antialiasing,false);
        p.drawRect(pos-1,0,1,h);
        p.setRenderHint(QPainter::Antialiasing,true);

        if (mPlayheads[i]->style & Playhead::Style::TOP_MARKER) {
            QPolygon topTriangle;
            topTriangle << QPoint(pos-triangleW,0) << QPoint(pos+triangleW,0) << QPoint(pos,triangleH);
            p.drawConvexPolygon(topTriangle);
        }
        if (mPlayheads[i]->style & Playhead::Style::BOTTOM_MARKER) {
            QPolygon bottomTriangle;
            bottomTriangle << QPoint(pos-triangleW,h) << QPoint(pos+triangleW,h) << QPoint(pos,h-triangleH);
            p.drawConvexPolygon(bottomTriangle);
        }
        if (mPlayheads[i]->style & Playhead::Style::RIGHT_MARKER) {
            QPolygon rTriangle;
            rTriangle << QPoint(pos,h/2-HtriangleW) << QPoint(pos,h/2+HtriangleW) << QPoint(pos+HtriangleH,h/2);
            p.drawConvexPolygon(rTriangle);
        }
        if (mPlayheads[i]->style & Playhead::Style::LEFT_MARKER) {
            QPolygon rTriangle;
            rTriangle << QPoint(pos,h/2-HtriangleW) << QPoint(pos,h/2+HtriangleW) << QPoint(pos-HtriangleH,h/2);
            p.drawConvexPolygon(rTriangle);
        }
    
    }



    //center line
    p.setRenderHint(QPainter::Antialiasing,false);
    p.setPen({255,220,220});
    if (singleSided)
        p.drawLine(0,h-1,w,h-1);
    else
        p.drawLine(0,h_half,w,h_half);
}

void WaveformViewportWidget::setScale(float scale) {
    if (mScale == scale) return;
    if (mScale < 1) mScale = 1;
    this->mScale = scale;
    this->updateMousePlayhead();
    this->recomputeDisplayedWaveform();
}
float WaveformViewportWidget::getScale() {
    return mScale;
}

void WaveformViewportWidget::scrollBy(int64_t n_samples) {

    apos_t newScroll;
    float samples_per_px = std::max(1.0f, ((float)mWaveformData->samples.size() / width() )-mScale);
    int maxScroll = mWaveformData->samples.size() - samples_per_px*width();

    // Check left bound
    if (n_samples<0 && mScroll+n_samples > mScroll)
        newScroll = 0;
    // Right bound
    else if (mScroll+n_samples > maxScroll)
        newScroll = maxScroll;
    else
        newScroll = mScroll+n_samples;
    
    
    this->setScroll(newScroll);
}
void WaveformViewportWidget::setScroll(apos_t n_samples) {
    if (mScroll == n_samples) return;
    mScroll = n_samples;

    this->updateMousePlayhead();
    this->recomputeDisplayedWaveform();
}
apos_t WaveformViewportWidget::getScroll() {
    return mScroll;
}

void WaveformViewportWidget::setSingleSidedWaveform(bool v) {
    if (mSingleSideWaveform == v) return;
    this->mSingleSideWaveform = v;
    this->repaint();
}
bool WaveformViewportWidget::isSingleSidedWaveform() {
    return mSingleSideWaveform;
}

void WaveformViewportWidget::updateMousePlayhead() {
    int x = this->mapFromGlobal(QCursor::pos()).x();
    int samples_per_px = std::max(1.0f, ((float)mWaveformData->samples.size() / width() )-mScale);
    mPlayheadMouse.position = x*samples_per_px + mScroll;

    // Optimization Note: region restricted repaint could be used, but I couldn't be
    // bothered to compute the old and new playhead regions
    this->repaint();
}