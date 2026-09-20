#include "ui/PanelWidgets.h"
#include "ui/CueListWidget.h"
#include "ui/PropertyTabWidgets.h"
#include "ui/QTUI.h"
#include "ui/PlayingCueWidget.h"
#include "ui/IconManager.h"
#include "ui/UtilWidgets.h"

#include <QBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QSpinBox>
#include <QTime>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QDrag>
#include <QMimeData>

#include <iostream>
#include <chrono>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

BPanel::BPanel(QWidget* parent) : QFrame(parent) {
    auto* layout = new QVBoxLayout;
    layout->setContentsMargins(4,4,4,4);
    this->setLayout(layout);
    this->setMinimumWidth(240);
    this->setMinimumHeight(200);
}

StatusPanel::StatusPanel(QWidget* parent) : 
    BPanel(parent), 
    mLCD(new QLCDNumber(this)),
    mTitleLabel(new QLabel(this))
{
    
    this->setFixedHeight(215);

    layout()->addWidget(mTitleLabel);
    layout()->addWidget(mLCD);
    
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    this->mLCD->setDigitCount(8);
    this->mLCD->setObjectName("ClockLCD");
    this->mLCD->setMaximumHeight(75);
    this->mLCD->setSegmentStyle(QLCDNumber::Flat);
    this->mLCD->display(QTime::currentTime().toString("hh:mm:ss"));

    this->mTitleLabel->setObjectName("StatusTitle");
    this->mTitleLabel->setAlignment(Qt::AlignCenter);
    mTitleLabel->setText("projectname");

    this->updateTime();

}

// calls itself with a singleshot timer
void StatusPanel::updateTime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto next = time_point_cast<seconds>(now) + seconds{1};  // next whole second
    auto delay = duration_cast<milliseconds>(next - now);

    QTimer::singleShot(delay.count(), this, [this] {
        this->updateTime();
        this->mLCD->display(QTime::currentTime().toString("hh:mm:ss"));
    });
};

void StatusPanel::resizeEvent(QResizeEvent* event) {
}


MiscPanel::MiscPanel(QWidget* parent) : BPanel(parent) {
    mCuePicker = new CuePikkerWidget(this);
    layout()->addWidget(mCuePicker);
    ((QVBoxLayout*)layout())->addStretch();
}


PlayingPanel::PlayingPanel(QWidget* parent) : BPanel(parent) {
    // EXTREMELY temporary test code xdd


    PlayingCueWidget* w = new PlayingCueWidget(this);
    layout()->addWidget(w);
    
    /*
    WaveformViewportWidget* w = new WaveformViewportWidget(this);
    layout()->addWidget(w);

    DBMeter* meter = new DBMeter(this);
    layout()->addWidget(meter);
    QTimer* timer = new QTimer(this); 
    timer->setInterval(1000/144);
    timer->start();
    connect(timer, &QTimer::timeout, this, [=](){
        this->sample += (48000/144);
        if (sample > audio_samples_len)
            sample = 0;
        meter->setLevels(
            (float)audio_samples[sample]/SAMPLE_MAX_VALUE*2,
            (float)audio_samples[sample]/SAMPLE_MAX_VALUE*2
        );
    } );
    */

    /*
    QSlider* s = new QSlider(this);
    layout()->addWidget(s);
    s->setMinimum(1);
    s->setMaximum(1000);
    s->setOrientation(Qt::Horizontal);
    s->setTickPosition(QSlider::TickPosition::NoTicks);

    QSlider* s2 = new QSlider(this);
    layout()->addWidget(s2);
    s2->setMinimum(1);
    s2->setMaximum(audio_samples_len/4);
    s2->setOrientation(Qt::Horizontal);
    s2->setTickPosition(QSlider::TickPosition::NoTicks);

    connect(s, &QSlider::valueChanged, this, [=](int value){
        w->setScale(value);
    });

    connect(s2, &QSlider::valueChanged, this, [=](int value){
        w->setScroll(value);
    });
    */
}

CueListPanel::CueListPanel(QWidget* parent) : BPanel(parent) {
    srand(QDateTime::currentMSecsSinceEpoch());
    //backend.addCue(std::make_unique<MediaCue>("I'm a media cue!", (rand()%200000), "this is a basic cue"));
    //for (int i = 0; i < 10'000; i++) 
    //    backend.addCue(std::make_unique<TextCue>("I'm a text cue!", "super cool description"));
    

    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSpacing(0);

    mHeaderWidget = new CueListHeader(this);
    mScrollWidget = new QScrollArea(this);
    mCueListWidget = new CueListWidget(mHeaderWidget, mScrollWidget->verticalScrollBar(), this);

    mScrollWidget->setWidgetResizable(true);
    mScrollWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mScrollWidget->setWidget(mCueListWidget);


    this->layout()->addWidget(mHeaderWidget);
    this->layout()->addWidget(mScrollWidget);

    QTimer::singleShot(0, [=]{mCueListWidget->setFocus();});
}


PropertiesPanel::PropertiesPanel(QWidget* parent) : BPanel(parent) {
    this->layout()->setContentsMargins(0,2,0,0);

    mTabWidget = new QTabWidget(this);
    
    auto* layout_old = this->layout();
    if (layout_old) // should always exist
        delete layout_old;
    QBoxLayout* layout = new QHBoxLayout();
    this->setLayout(layout);

    layout->addWidget(new CueGeneralPage(mTabWidget));
    layout->addWidget(mTabWidget);


    this->addPage(new TextCuePage(mTabWidget));
    this->addPage(new IdkRandomPage(mTabWidget));

    
    //mTabWidget->setTabVisible(2, false);

}

void PropertiesPanel::addPage(PropertyPage* page) {
    mPages.push_back(page);
    mTabWidget->addTab(page, page->getPageName());
}

inline QWidget* CuePikkerWidget::constructCueCategory(QString name, std::vector<CueItemWidget*> widgets) {
    QWidget* w1 = new QWidget(this); 
    w1->setLayout(new QVBoxLayout());
    for (auto it = widgets.begin(); it < widgets.end(); it++)
        w1->layout()->addWidget(*it);
    FoldingWidget* f1 = new FoldingWidget(this);
    f1->setWidget(w1);
    f1->setTitle(name);
    return f1;
}

CuePikkerWidget::CuePikkerWidget(QWidget* parent) : QFrame(parent) {
    auto layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->setContentsMargins(0,0,0,0);

    mTitleLabel = new QLabel("Drag to add new cue", this);
    mTitleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(mTitleLabel);

    mCuesFrame = new QFrame(this);
    mCuesFrame->setObjectName("CuePickerFrame");
    layout->addWidget(mCuesFrame);


    auto frameLayout = new QVBoxLayout(mCuesFrame);
    mCuesFrame->setLayout(frameLayout);
    frameLayout->setContentsMargins(2,2,2,2);
    frameLayout->setSpacing(5);

    frameLayout->addWidget(constructCueCategory("General", {
        new CueItemWidget("text","Text Cue"),
        new CueItemWidget("command","Command Cue")
    }));
    frameLayout->addWidget(constructCueCategory("Media", {
        new CueItemWidget("audio","Audio Cue"),
        new CueItemWidget("video","Video Cue"),
        new CueItemWidget("image","Image Cue")
    }));
    frameLayout->addWidget(constructCueCategory("Control", {
        new CueItemWidget("index","Index Cue"),
        new CueItemWidget("group","Group Cue"),
        new CueItemWidget("stopall","Stop All Cue"),
        new CueItemWidget("timer","Scheduled Cue"),
    }));
    frameLayout->addWidget(constructCueCategory("Integration", {
        new CueItemWidget("midi","MIDI Cue"),
        new CueItemWidget("osc","OSC Cue"),
        new CueItemWidget("light","Lighting Cue"),
    }));
}

void CueItemWidget::mousePressEvent(QMouseEvent* event){
    auto* drag = new QDrag(this);
    auto* mime = new QMimeData;
    mime->setData("custom/new-cue", mCueType.toUtf8()); // payload is cueType name string
    drag->setMimeData(mime);
    drag->setPixmap(this->grab());

    //QSizePolicy p = this->sizePolicy();
    //p.setRetainSizeWhenHidden(true);
    //this->setSizePolicy(p);
    //this->hide();

    drag->exec(Qt::MoveAction);

    //this->show();
}

CueItemWidget::CueItemWidget(QString cueType, QString displayName, QWidget* parent) : QFrame(parent), mCueType(cueType) {
    auto layout = new QHBoxLayout(this);
    this->setLayout(layout);
    layout->setContentsMargins(3,1,3,1);

    mSvgWidget = new QSvgWidget(this);
    mSvgWidget->load(IconManager::getIconPathForCueType(cueType));
    mSvgWidget->setFixedSize(17,17);
    layout->addWidget(mSvgWidget);

    mLabel = new QLabel(displayName, this);
    layout->addWidget(mLabel);
}