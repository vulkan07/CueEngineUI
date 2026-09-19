#include "ui/PlayingCueWidget.h"

#include "_asample.h"

#include <QBoxLayout>
#include <QTimer>
#include <qboxlayout.h>
#include <qmargins.h>
#include <qwidget.h>

PlayingCueWidget::PlayingCueWidget(QWidget* parent) : QWidget(parent) {
    constexpr QMargins NO_MARGIN{0,0,0,0}; // im lazy

    mHSplitter = new QSplitter(Qt::Horizontal, this);
    mLeftSplitter = new QWidget(this);
    mMiddleSplitter = new QSplitter(Qt::Vertical, this);
    mLCDSPlitter = new QWidget(this);
    mTitleSplitter = new QWidget(this);

    mWaveformWidget = new WaveformViewportWidget(this);
    mDBMeterWidget = new DBMeter(this);
    mRemainingWidget = new QLCDNumber(this);
    mElapsedWidget = new QLCDNumber(this);
    mDurationWidget = new QLCDNumber(this);
    mIndexWidget = new QLabel(this);
    mTitleWidget = new QLabel(this);
    mDescriptionWidget = new QTextEdit(this);

    // Set object names for styling
    mTitleWidget->setObjectName("PlayingCueTitle");
    mIndexWidget->setObjectName("PlayingCueIndex");
    mDescriptionWidget->setObjectName("PlayingCueDescription");
    mDurationWidget->setObjectName("PlayingCueDuration");
    mRemainingWidget->setObjectName("PlayingCueRemaining");
    mElapsedWidget->setObjectName("PlayingCueElapsed");
    mWaveformWidget->setObjectName("PlayingCueWaveform");

    // Construct layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mHSplitter);
    this->setLayout(mainLayout);

    mHSplitter->addWidget(mLeftSplitter);
    mHSplitter->addWidget(mDBMeterWidget);
    mHSplitter->addWidget(mMiddleSplitter);

    QBoxLayout* layout = new QVBoxLayout();
    mLeftSplitter->setLayout(layout);
    layout->setContentsMargins(NO_MARGIN);
    layout->addWidget(mTitleSplitter);
    layout->addWidget(mDescriptionWidget);
    
    mMiddleSplitter->addWidget(mLCDSPlitter);
    mMiddleSplitter->addWidget(mWaveformWidget);

    layout = new QHBoxLayout();
    mLCDSPlitter->setLayout(layout);
    layout->setContentsMargins(NO_MARGIN);
    layout->addWidget(mRemainingWidget);
    layout->addWidget(mElapsedWidget);
    layout->addWidget(mDurationWidget);
    layout->addStretch();

    layout = new QHBoxLayout();
    mTitleSplitter->setLayout(layout);
    layout->setContentsMargins(NO_MARGIN);
    layout->addWidget(mIndexWidget);
    layout->addWidget(mTitleWidget);

    // Setup
    mDescriptionWidget->setReadOnly(true);
    mDescriptionWidget->setFocusPolicy(Qt::NoFocus);

    std::vector<QLCDNumber*> v{mRemainingWidget,mElapsedWidget,mDurationWidget};
    for (auto it = v.begin(); it < v.end(); it++) {
        auto w = *it;
        w->setSegmentStyle(QLCDNumber::Flat);
        w->setMinimumWidth(270);
        w->setMinimumHeight(40);
        w->setDigitCount(5);
        w->display("--:--");
    }

    // EXtra super duper temporary emulation code until backend exists xd
    QTimer* timer = new QTimer(this); 
    timer->setInterval(1000/144);
    timer->start();
    connect(timer, &QTimer::timeout, this, [=]{
        static uint64_t sample = 0; //TEST ONLY


        sample += (48000/144);
        if (sample > audio_samples_len)
            sample = 0;

        float v = (float)audio_samples[sample]/SAMPLE_MAX_VALUE*2;
        v = std::min(v,1.0f);
        
        mDBMeterWidget->setLevels(v,v);
        mWaveformWidget->setPlaybackPos(sample);
        int sec = sample/48000;
        int len = audio_samples_len/48000;
        mElapsedWidget->display("  :0"+QString::number(sec));
        mRemainingWidget->display("  :0"+QString::number(len-sec));
        mDurationWidget->display("  :"+QString::number(len));
    } );

    mLeftSplitter->setMinimumWidth(250);
    mDBMeterWidget->setFixedWidth(30);


    mIndexWidget->setFixedSize(30,30);
    mIndexWidget->setAlignment(Qt::AlignCenter);

    
    mHSplitter->setContentsMargins(0, 0, 0, 0);
    mLeftSplitter->setContentsMargins(0, 0, 4, 0);
    mMiddleSplitter->setContentsMargins(4, 0, 0, 0);
    mTitleSplitter->setContentsMargins(0, 0, 0, 0);
    mLCDSPlitter->setContentsMargins(0, 0, 0, 0);

    mHSplitter->setStretchFactor(0,0);
    mHSplitter->setStretchFactor(1,1);
    mHSplitter->setStretchFactor(2,2);


    //TEST

    mTitleWidget->setText("Ridiculously long cue title mmm");

    mDescriptionWidget->setText("SuperCoolDesc\nasdasd\nasdasd2");
    mIndexWidget->setText("1");

    
    WaveformData<asample_t>* data = new WaveformData<asample_t>;
    data->samples.resize(audio_samples_len);
    for (int i = 0; i < audio_samples_len; i++)
        data->samples[i] = {(asample_t)audio_samples[i],0};
    mWaveformWidget->setWaveformData(data);

}