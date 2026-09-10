#include "ui/DBMeter.h"

#include <QPaintEvent>
#include <QPainter>

DBMeter::DBMeter(QWidget* parent) : QWidget(parent) {
    this->setMinimumWidth(32);
    this->setMaximumWidth(48);
    this->setMinimumHeight(96);
    this->setMaximumHeight(256);
    this->setFixedWidth(40);
}

void DBMeter::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setClipRegion(event->region());


    constexpr int gap = 6;
    constexpr int gaphalf = gap/2;
    int w = width();
    int h = height();
    float whalf = w/2;
    int rectwidth = whalf-gap;


    // Frames
    p.setPen(this->frameColor);
    p.setRenderHint(QPainter::Antialiasing, false);
    p.drawRect(0,0,rectwidth,h-1);
    p.drawRect(rectwidth+gaphalf,0,rectwidth,h-1);

    p.setRenderHint(QPainter::Antialiasing, true);

    // Bars
    QLinearGradient grad({0,(float)h-1}, {0,0});
    grad.setStops({
        { 0.0, {0  ,200,  0} },
        { 0.7, {255,220,  0} },
        { 1.0, {255,0  ,  0} }
    });
    p.setBrush(grad);
    p.setPen({});

    float paintHeight = h-2;
    float LH = .6 * paintHeight;
    float RH = .2 * paintHeight;
    p.drawRect(2,h-LH+1,rectwidth-3,LH-2);
    p.drawRect(2+rectwidth+gaphalf,h-RH+1,rectwidth-3,RH-2);
}

// Takes raw audio samples [-1,1]
// For downsampling, supply the peak value of each subrange
// DBFS conversion is handled by this
void DBMeter::setLevels(float L, float R) {
    constexpr double floorDB = -60;
    mSampleL = L;
    mSampleR = R;
    mLevelL = std::max(floorDB, 20*log10(std::abs(L)));
    mLevelR = std::max(floorDB, 20*log10(std::abs(R)));
    // Map Level to [0,1]
    mLevelL = (mLevelL - floorDB) / (0.0f - floorDB);
    mLevelR = (mLevelR - floorDB) / (0.0f - floorDB);

    this->repaint();
}
float DBMeter::getLevel_L() { return mSampleL;}
float DBMeter::getLevel_R() { return mSampleR;}