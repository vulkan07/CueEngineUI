#include "ui/CoreWidgets.h"

#include <QBoxLayout>

#include <QCheckBox>
#include <QSpinBox>
#include <QTime>
#include <iostream>

BPanel::BPanel(QWidget* parent) : QFrame(parent) {
    auto* layout = new QVBoxLayout;
    layout->setContentsMargins(4,4,4,4);
    this->setLayout(layout);
    this->setMinimumWidth(240);
    this->setMinimumHeight(200);
/*
    auto* l = new QLabel("asd",this);
    l->setMaximumWidth(200);
    l->setMaximumHeight(100);
    layout->addWidget(l);
*/
}

TestPanel::TestPanel(QWidget* parent) : BPanel(parent) {

    QLabel* l = new QLabel("I'm a label", this);
    QCheckBox* c = new QCheckBox("I'm a checkbox", this);
    QSpinBox* s = new QSpinBox(this);
    auto* lcd = new QLCDNumber(8,this);
    
    lcd->setSegmentStyle(QLCDNumber::Flat);
    lcd->display("12:34:56");
    lcd->setFixedHeight(50);

    s->setSuffix("s");


    this->layout()->addWidget(l);
    this->layout()->addWidget(c);
    this->layout()->addWidget(s);
    this->layout()->addWidget(lcd);
}

StatusPanel::StatusPanel(QWidget* parent) : 
    BPanel(parent), 
    mLCD(new QLCDNumber(this)),
    mTitleLabel(new QLabel(this)),
    mClockTimer(new QTimer(this)),
    mSyncTimer(new QTimer(this))
{

    this->setFixedHeight(200);

    QBoxLayout* layout = qobject_cast<QBoxLayout*>(this->layout());
    layout->addWidget(mTitleLabel);
    layout->addWidget(mLCD);
    //layout->addStretch();

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    this->mLCD->setDigitCount(8);
    this->mLCD->setMinimumHeight(80);
    this->mLCD->setSegmentStyle(QLCDNumber::Flat);
    this->mTitleLabel->setObjectName("StatusTitle");
    this->mTitleLabel->setAlignment(Qt::AlignCenter);
    this->updateTitleSize();
    mTitleLabel->setText("Goon Engine");

    connect(mClockTimer, &QTimer::timeout, this, &StatusPanel::updateTime);
    connect(mSyncTimer, &QTimer::timeout, this, &StatusPanel::syncTimer);
    this->mSyncTimer->start(1);
    this->mClockTimer->start(1000);

    this->updateTime();
}

void StatusPanel::updateTime() {
        this->mLCD->display(QTime::currentTime().toString("hh:mm:ss"));
};

void StatusPanel::syncTimer() {

}

void StatusPanel::updateTitleSize() {
    if (mTitleLabel->text().isEmpty()) return;

    int w = mTitleLabel->width();
    QFont f = mTitleLabel->font();
    
    QFontMetrics fm = mTitleLabel->fontMetrics();

    // Shrink until it fits
    int size = 500;
    while (fm.boundingRect(mTitleLabel->text()).width() > w && size > 1) {
        size--;
        f.setPointSize(size);
        std::cout << fm.boundingRect(mTitleLabel->text()).width() << std::endl;
    
    }

    f.setPointSize(32);
    mTitleLabel->setFont(f);
}

void StatusPanel::resizeEvent(QResizeEvent* event) {
    this->updateTitleSize();
}