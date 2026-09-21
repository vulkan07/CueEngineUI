#include "ui/Waveform.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>

#include <algorithm>
#include <cmath>
#include <qdebug.h>
#include <qregion.h>
#include <spdlog/spdlog.h>

// TODO: offload conversion between pixel positions and audio sample positions
// in all functions here

WaveformViewportWidget::WaveformViewportWidget(QWidget* parent) : 
    QWidget(parent), mDisplayedWaveform(new WaveformData<unsigned char>) {
    this->setMinimumHeight(48);
    this->setMaximumHeight(255*2); // Waveform cache uses 1 byte so theoretical max height per side is 256
    this->setMinimumWidth(256);

    this->setMouseTracking(true);

    //TEST
    mPlayheadPlayback = {.color={230,230,230,200},.style=Playhead::Style::PLAIN,.visible=false};
    mPlayheadMouse = {.color={200,200,200,150},.visible=false};

    // List order controls paint order too
    mPlayheads.push_back(&mPlayheadPlayback);
    mPlayheads.push_back(&mPlayheadMouse);
}

void WaveformViewportWidget::setWaveformData(WaveformData<asample_t>* data){
    mWaveformData=data;
}
WaveformData<asample_t>& WaveformViewportWidget::getWaveformData(){
    return *mWaveformData;
}


void WaveformViewportWidget::recomputeDisplayedWaveform() {
    if (!mWaveformData) return;
    int w = this->width();
    mDisplayedWaveform->samples.resize(w);
    int samples_per_px = std::max(1.0f, ((float)mWaveformData->samples.size() / w )-mScale);

    // Skip recompute when the function was called without actual state change
    if (mSamplesPerPx != 0 && mSamplesPerPx == samples_per_px && !mRecomputeFlag)
        return;
    mSamplesPerPx = samples_per_px;

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
    mRecomputeFlag = false;
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
    QPainter p(this);

    if (mWaveformData) {
        int w = this->width();
        int h = this->height();
        float h_half = h/2.0f;

        p.setRenderHint(QPainter::Antialiasing,false);
        p.setClipRegion(event->region());

        bool singleSided = h <= 75 || mSingleSideWaveform;

        for (auto it = event->region().begin(); it < event->region().end(); it++) {
            int startPixel = it->left();
            int endPixel = std::min(it->right()+2, width()); // For partial repaints, an extension at the right bound is needed for some reason
            //qDebug()<< "PAINT ["<<startPixel<<","<<endPixel<<"]  | playhead:"<<(mPlayheadMouse.position-mScroll)/mSamplesPerPx;
            for (int i = startPixel; i < endPixel; i++) {
                uint8_t vlinear = mDisplayedWaveform->samples[i].valueLinear;
                uint8_t vrms = mDisplayedWaveform->samples[i].valueRMS;
                //p.setPen({vlinear+100,30,30});

                // Linear line
                p.setPen({180,20,20});
                if (singleSided)
                    p.drawLine(i,h-vlinear*2,i,h);
                else
                    p.drawLine(i,(h_half-vlinear),i,h_half+vlinear);

                // RMS line
                p.setPen({230,30,30});
                if (singleSided)
                    p.drawLine(i,h-vrms*2,i,h);
                else
                    p.drawLine(i,(h_half-vrms),i,h_half+vrms);

                // partial repaint debug
                //if(startPixel!=0) {
                //    p.setPen({255,255,0,180});
                //    p.drawLine(i,0,i,h_half+vrms);
                //}
            }
        }

        // Playheads
        constexpr int triangleW = 4;
        constexpr int triangleH = 5;
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
        p.setPen({230,200,200});
        if (singleSided)
            p.drawLine(0,h-1,w,h-1);
        else
            p.drawLine(0,h_half,w,h_half);
    }

    //// Draw qss style

    // Initialize the style option for a standard QWidget
    QStyleOption opt;
    opt.initFrom(this);
    // Force Qt to draw the QSS background, borders, and margins
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void WaveformViewportWidget::setScale(float scale) {
    if (mScale == scale) return;
    if (mScale < 1) mScale = 1;
    this->mScale = scale;
    this->mRecomputeFlag = true;
    this->updateMousePlayhead();
    this->recomputeDisplayedWaveform();
    this->repaint();
}
float WaveformViewportWidget::getScale() {
    return mScale;
}

void WaveformViewportWidget::scrollBy(int64_t n_samples) {
    if (!mWaveformData) return;

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
    if (!mWaveformData) return;
    if (mScroll == n_samples) return;
    mScroll = n_samples;

    this->mRecomputeFlag = true;
    this->updateMousePlayhead();
    this->recomputeDisplayedWaveform();
    this->repaint();
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
    if (!mWaveformData) return;
    int x = this->mapFromGlobal(QCursor::pos()).x();
    int samples_per_px = std::max(1.0f, ((float)mWaveformData->samples.size() / width() )-mScale);
    int oldX = (float)(mPlayheadMouse.position-mScroll)/samples_per_px;
    mPlayheadMouse.position = x*samples_per_px + mScroll;


    // Optimization Note: region restricted repaint could be used, but I couldn't be
    // bothered to compute the old and new playhead regions
    this->repaint(
        QRegion(   x-REPAINT_PADDING,0,1+REPAINT_PADDING*2,height()) |
        QRegion(oldX-REPAINT_PADDING,0,1+REPAINT_PADDING*2,height())
    );
}

void WaveformViewportWidget::setPlaybackPos(apos_t sample) {
    int x = (sample-mScroll)/mSamplesPerPx;
    int oldX = (mPlayheadPlayback.position-mScroll)/mSamplesPerPx;

    mPlayheadPlayback.position = sample;
    mPlayheadPlayback.visible = true;

    this->repaint(
        QRegion(   x-REPAINT_PADDING,0,1+REPAINT_PADDING*2,height()) |
        QRegion(oldX-REPAINT_PADDING,0,1+REPAINT_PADDING*2,height())
    );
}