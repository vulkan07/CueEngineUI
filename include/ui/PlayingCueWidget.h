#pragma once

#include "ui/Waveform.h"
#include "ui/DBMeter.h"

#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QLCDNumber>
#include <QSplitter>
#include <qsvgwidget.h>
#include <qwidget.h>

class PlayingCueWidget : public QWidget {
    Q_OBJECT
private:

    QSplitter* mHSplitter; // [Title region] [DB Meter] [Waveform region]
    QSplitter* mMiddleSplitter;
    // Some layout widgets use a BoxLayout instead of QSplitter for better sizing management
    QWidget* mLeftSplitter;
    QWidget* mTitleSplitter;
    QWidget* mLCDSPlitter;

    
    WaveformViewportWidget* mWaveformWidget;
    DBMeter* mDBMeterWidget;
    QLCDNumber* mRemainingWidget;
    QLCDNumber* mElapsedWidget;
    QLCDNumber* mDurationWidget;
    QLabel* mIndexWidget;
    QSvgWidget* mIconWidget;
    QLabel* mTitleWidget;
    QTextEdit* mDescriptionWidget;
    
public:
    PlayingCueWidget(QWidget* parent);
};