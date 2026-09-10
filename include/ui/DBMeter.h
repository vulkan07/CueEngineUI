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
        { 0.0, {0  ,200,  0} },
        { 0.5, {255,220,  0} },
        { 0.7, {255,220,  0} },
        { 1.0, {255,0  ,  0} }
    };

private:

    float mSampleL=0;
    float mSampleR=0;
    float mLevelL=0;
    float mLevelR=0;
};