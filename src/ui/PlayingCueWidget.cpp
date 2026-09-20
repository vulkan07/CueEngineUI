#include "ui/PlayingCueWidget.h"

#include "_asample.h"
#include "ui/IconManager.h"

#include <QBoxLayout>
#include <QTimer>
#include <qboxlayout.h>
#include <qmargins.h>
#include <qsvgwidget.h>
#include <qwidget.h>

PlayingCueWidget::PlayingCueWidget(QWidget* parent) : QWidget(parent) {
    constexpr QMargins NO_MARGIN{0,0,0,0}; // im lazy
    constexpr int BOX_SIZE = 29; // for icon & index boxes
    constexpr int SPACING = 2;

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
    mIconWidget = new QSvgWidget(this);

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
    layout->setSpacing(SPACING);
    layout->addWidget(mTitleSplitter);
    layout->addWidget(mDescriptionWidget);
    
    mMiddleSplitter->addWidget(mLCDSPlitter);
    mMiddleSplitter->addWidget(mWaveformWidget);

    layout = new QHBoxLayout();
    mLCDSPlitter->setLayout(layout);
    layout->setContentsMargins(NO_MARGIN);
    layout->setSpacing(SPACING);
    layout->addWidget(mRemainingWidget);
    layout->addWidget(mElapsedWidget);
    layout->addWidget(mDurationWidget);
    layout->addStretch();

    // Wrap SVG widget in another widget to create padding
    QWidget* iconHolder = new QWidget(this);
    iconHolder->setLayout(new QHBoxLayout);
    iconHolder->layout()->addWidget(mIconWidget);
    iconHolder->layout()->setContentsMargins(4,4,4,4);
    iconHolder->setFixedSize(BOX_SIZE,BOX_SIZE);
    iconHolder->setObjectName("PlayingCueIcon");

    layout = new QHBoxLayout();
    mTitleSplitter->setLayout(layout);
    layout->setContentsMargins(NO_MARGIN);
    layout->setSpacing(SPACING);
    layout->addWidget(mIndexWidget);
    layout->addWidget(iconHolder);
    layout->addWidget(mTitleWidget);

    // Setup
    mDescriptionWidget->setReadOnly(true);
    mDescriptionWidget->setFocusPolicy(Qt::NoFocus);

    std::vector<QLCDNumber*> v{mRemainingWidget,mElapsedWidget,mDurationWidget};
    for (auto it = v.begin(); it < v.end(); it++) {
        auto w = *it;
        w->setSegmentStyle(QLCDNumber::Flat);
        w->setMinimumWidth(240);
        w->setMinimumHeight(40);
        w->setDigitCount(5);
        w->display("--:--");
    }


    // EXtra super duper temporary emulation code until backend exists xd
    mIconWidget->load(IconManager::getIconPathForCueType("timer"));
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
        //mWaveformWidget->setPlaybackPos(sample);
        int sec = sample/48000;
        int len = audio_samples_len/48000;
        mElapsedWidget->display("  :0"+QString::number(sec));
        mRemainingWidget->display("  :0"+QString::number(len-sec));
        mDurationWidget->display("  :"+QString::number(len));
    } );

    mLeftSplitter->setMinimumWidth(250);
    mLeftSplitter->setMaximumWidth(420);
    mDBMeterWidget->setFixedWidth(30);


    mIndexWidget->setFixedSize(BOX_SIZE,BOX_SIZE);
    mIndexWidget->setAlignment(Qt::AlignCenter);

    mTitleWidget->setFixedHeight(BOX_SIZE);
    
    mHSplitter->setContentsMargins(0, 0, 0, 0);
    mLeftSplitter->setContentsMargins(0, 0, 4, 0);
    mMiddleSplitter->setContentsMargins(4, 0, 0, 0);
    mTitleSplitter->setContentsMargins(0, 0, 0, 0);
    mLCDSPlitter->setContentsMargins(0, 0, 0, SPACING-1);

    mHSplitter->setStretchFactor(0,1);
    mHSplitter->setStretchFactor(1,1);
    mHSplitter->setStretchFactor(2,2);


    //TEST

    mTitleWidget->setText("Cue title 123");

    mDescriptionWidget->setText("start when balls123 idk almafa\ncool description\n\nparallelepiped");
    mIndexWidget->setText("1");

    
    WaveformData<asample_t>* data = new WaveformData<asample_t>;
    data->samples.resize(audio_samples_len);
    for (int i = 0; i < audio_samples_len; i++)
        data->samples[i] = {(asample_t)audio_samples[i],0};
    mWaveformWidget->setWaveformData(data);

}