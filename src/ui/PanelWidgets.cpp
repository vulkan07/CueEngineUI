#include "ui/PanelWidgets.h"
#include "ui/QTUI.h"
#include "backend/Backend.h"

#include <QBoxLayout>
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
    lcd->display(" 3:07");
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
    this->mLCD->setMinimumHeight(80);
    this->mLCD->setSegmentStyle(QLCDNumber::Flat);
    this->mLCD->display(QTime::currentTime().toString("hh:mm:ss"));

    this->mTitleLabel->setObjectName("StatusTitle");
    this->mTitleLabel->setAlignment(Qt::AlignCenter);
    this->updateTitleSize();
    mTitleLabel->setText("Goon Engine");

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

void StatusPanel::updateTitleSize() {
    /*
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
    mTitleLabel->setFont(f);*/
}

void StatusPanel::resizeEvent(QResizeEvent* event) {
    this->updateTitleSize();
}


MiscPanel::MiscPanel(QWidget* parent) : BPanel(parent) {}

PlayingPanel::PlayingPanel(QWidget* parent) : BPanel(parent) {}

CueListPanel::CueListPanel(QWidget* parent) : BPanel(parent) {

    for (int i = 0; i < 67; i++) {
        backend.addCue(std::make_unique<Cue>("I'm a cue!", "faszom nagyon hosszue leiras blah blhah leiras"));
    }

    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSpacing(CueListWidget::GAP_WIDTH+1);
    

    mHeaderWidget = new CueListHeader(this);
    mCueListWidget = new CueListWidget(mHeaderWidget, this);
    mScrollWidget = new QScrollArea(this);

    mScrollWidget->setWidgetResizable(true);
    mScrollWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mScrollWidget->setWidget(mCueListWidget);


    mUpAction = new QAction(this);
    mDownAction = new QAction(this);
    mPlayAction = new QAction(this);

    mUpAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mDownAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mPlayAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    mUpAction->setShortcut(QKeySequence("Up"));
    mDownAction->setShortcut(QKeySequence("Down"));
    mPlayAction->setShortcuts({QKeySequence("Space"),QKeySequence("Return")});

    connect(mUpAction, &QAction::triggered, mCueListWidget, &CueListWidget::onUpAction);
    connect(mDownAction, &QAction::triggered, mCueListWidget, &CueListWidget::onDownAction);
    connect(mPlayAction, &QAction::triggered, mCueListWidget, &CueListWidget::onPlayAction);

    this->addActions({mUpAction,mDownAction,mPlayAction});

    ShortcutManager::registerAction("Play Cue", mPlayAction);

    this->layout()->addWidget(mHeaderWidget);
    this->layout()->addWidget(mScrollWidget);
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