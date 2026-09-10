#include "ui/DBMeter.h"

#include <QPaintEvent>
#include <QPainter>

DBMeter::DBMeter(QWidget* parent) : QWidget(parent) {
    this->setMinimumWidth(24);
    this->setMaximumWidth(48);
    this->setMinimumHeight(80);
    this->setMaximumHeight(256);
    this->setFixedWidth(28);
}

void DBMeter::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setClipRegion(event->region());

    constexpr float clip_threshold = 0.99;
    constexpr float min_threshold = 0.05;

    constexpr int topsegmentHeight = 4;
    constexpr int hgap = 2;
    constexpr int hoffs = topsegmentHeight+hgap;
    constexpr int wgap = 2;
    constexpr int wgaphalf = wgap/2;
    int w = width();
    int h = height();
    float whalf = w/2;
    int rectwidth = whalf-wgaphalf;

    p.setPen(this->frameColor);
    p.setRenderHint(QPainter::Antialiasing, false);
    // Top (clipping) indicators
    p.drawRect(0,0,rectwidth,topsegmentHeight);
    p.drawRect(rectwidth+wgap,0,rectwidth-1,topsegmentHeight);
    // Bar frames
    p.drawRect(0,hoffs,rectwidth,h-1-hoffs);
    p.drawRect(rectwidth+wgap,hoffs,rectwidth-1,h-1-hoffs);

    // Clipping indicator fills
    p.setBrush({255,30,30});
    p.setPen({255,150,150});
    if (mLevelL >= clip_threshold)
        p.drawRect(0,0,rectwidth,topsegmentHeight);
    if (mLevelR >= clip_threshold)
        p.drawRect(rectwidth+wgap,0,rectwidth-1,topsegmentHeight);

    p.setRenderHint(QPainter::Antialiasing, true);

    // Bars
    QLinearGradient grad({0,(float)h-hoffs-1}, {0,0});
    grad.setStops(this->gradient);
    p.setBrush(grad);
    p.setPen({});

    float paintHeight = h-hoffs;
    float LH = mLevelL * paintHeight;
    float RH = mLevelR * paintHeight;
    if (mLevelL >= min_threshold)
        p.drawRect(
            2,
            h-LH+1,
            rectwidth-3,
            ceil(LH-2.0)
        );
    if (mLevelR >= min_threshold)
        p.drawRect(
            2+rectwidth+wgap,
            h-RH+1,
            rectwidth-4,
            ceil(RH-2.0)
        );
}

// Takes raw audio samples `[-1,1]`
// DBFS conversion is handled by this method
// For downsampling, supply the peak value of each subrange
void DBMeter::setLevels(float L, float R) {
    constexpr double floorDB = -60;
    mSampleL = L;
    mSampleR = R;
    // Sample -> DBFS
    mLevelL = std::max(floorDB, 20*log10(std::abs(L)));
    mLevelR = std::max(floorDB, 20*log10(std::abs(R)));
    // Map Level to [0,1]
    mLevelL = (mLevelL - floorDB) / (0.0f - floorDB);
    mLevelR = (mLevelR - floorDB) / (0.0f - floorDB);

    this->repaint();
}
float DBMeter::getLevel_L() { return mLevelL; }
float DBMeter::getLevel_R() { return mLevelR; }