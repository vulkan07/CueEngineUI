#include "ui/PanelWidgets.h"
#include "ui/QTUI.h"
#include "ui/Waveform.h"
#include "_asample.h"
#include "backend/Backend.h"

#include <QBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QSpinBox>
#include <QTime>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include <iostream>
#include <chrono>

BPanel::BPanel(QWidget* parent) : QFrame(parent) {
    auto* layout = new QVBoxLayout;
    layout->setContentsMargins(2,2,2,2);
    this->setLayout(layout);
    this->setMinimumWidth(240);
    this->setMinimumHeight(200);
}

TestPanel::TestPanel(QWidget* parent) : BPanel(parent) {

    QLabel* l = new QLabel("I'm a label", this);
    QCheckBox* c = new QCheckBox("I'm a checkbox", this);
    QCheckBox* c2 = new QCheckBox("I'm another checkbox!", this);
    QCheckBox* c3 = new QCheckBox("almafa idk korte", this);
    QCheckBox* c4 = new QCheckBox("balls 12345", this);
    QSpinBox* s = new QSpinBox(this);

    auto* btn1 = new QPushButton("I'M a button!", this);
    auto* btn2 = new QPushButton("I'M a better button!", this);
    auto* btn3 = new QPushButton("Balls", this);

    btn2->setDisabled(true);

    auto* lcd = new QLCDNumber(8,this);
    auto* line = new QLineEdit("nigger",this);
    auto* line2 = new QLineEdit("disabled nig",this);
    auto* te = new QTextEdit(this);

    line2->setDisabled(true);
    c->setChecked(true);
    c3->setDisabled(true);
    c4->setDisabled(true);
    c4->setChecked(true);

    lcd->setSegmentStyle(QLCDNumber::Flat);
    lcd->setFixedHeight(50);

    s->setSuffix("s");

    this->setMaximumWidth(500);

    this->layout()->addWidget(btn1);
    this->layout()->addWidget(btn2);
    this->layout()->addWidget(btn3);

    this->layout()->addWidget(l);
    this->layout()->addWidget(c);
    this->layout()->addWidget(c2);
    this->layout()->addWidget(c4);
    this->layout()->addWidget(te);
    this->layout()->addWidget(c3);
    this->layout()->addWidget(s);
    this->layout()->addWidget(line);
    this->layout()->addWidget(line2);
    this->layout()->addWidget(lcd);

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


MiscPanel::MiscPanel(QWidget* parent) : BPanel(parent) {}


PlayingPanel::PlayingPanel(QWidget* parent) : BPanel(parent) {
    WaveformData<asample_t>* data = new WaveformData<asample_t>;
    data->samples.resize(audio_samples_len);
    for (int i = 0; i < audio_samples_len; i++)
        data->samples[i] = {(asample_t)audio_samples[i],0};
    
    WaveformViewportWidget* w = new WaveformViewportWidget(this);
    layout()->addWidget(w);
    w->setWaveformData(data);

    QSlider* s = new QSlider(this);
    layout()->addWidget(s);
    s->setMinimum(10);
    s->setMaximum(1000);
    s->setOrientation(Qt::Horizontal);
    s->setTickPosition(QSlider::TickPosition::NoTicks);

    connect(s, &QSlider::valueChanged, this, [=](int value){
        w->setScale(value/10);
    });

}

CueListPanel::CueListPanel(QWidget* parent) : BPanel(parent) {
    srand(QDateTime::currentMSecsSinceEpoch());
    backend.addCue(std::make_unique<MediaCue>("I'm a media cue!", (rand()%200000), "this is a basic cue"));
    for (int i = 0; i < 10'000; i++) 
        backend.addCue(std::make_unique<TextCue>("I'm a text cue!", "super cool description"));
    

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
    
    this->layout()->addWidget(mTabWidget);

    auto* page = new IdkRandomPage(mTabWidget);

    this->addPage(new CueGeneralPage(mTabWidget));
    this->addPage(new TextCuePage(mTabWidget));
    this->addPage(page);

    
    //Temporary
    mTabWidget->setTabVisible(2, false); // !
    mTabWidget->addTab(new TestPanel(this), "Testing stuff");


}

void PropertiesPanel::addPage(PropertyPage* page) {
    mPages.push_back(page);
    mTabWidget->addTab(page, page->getPageName());
}