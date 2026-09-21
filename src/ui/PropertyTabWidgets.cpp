#include "ui/PropertyTabWidgets.h"
#include "ui/IconManager.h"

#include <QVBoxLayout>
#include <QLabel>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qspinbox.h>
#include <qsvgwidget.h>
#include <qtextedit.h>
#include <qwidget.h>

PropertyPage::PropertyPage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    this->setLayout(layout);
}

static inline QWidget* labelled(const QString& text, QWidget* widget, int maxLabelWidth=0) {
    QWidget* w = new QWidget();
    auto* layout = new QHBoxLayout();
    w->setLayout(layout);
    w->setParent(widget->parentWidget()); // automatically attach to the widgets ownership

    QLabel* label = new QLabel(text);
    if (maxLabelWidth>0)
        label->setMaximumWidth(maxLabelWidth);

    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(label);
    layout->addWidget(widget);

    return w;
};

CueGeneralPage::CueGeneralPage(QWidget* parent) : PropertyPage(parent) {
    constexpr QMargins NO_MARGIN{0,0,0,0}; // im lazy
    constexpr int BOX_SIZE = 29; // for icon & index boxes
    constexpr int SPACING = 3;

    this->setMaximumWidth(330);
    mCueIndexWidget = new QLabel(this);
    mCueIconWidget = new QSvgWidget(this);
    mCueDescriptionWidget = new QTextEdit(this);
    mCueNameWidget = new QLineEdit(this);
    mCueNextActionWidget = new QComboBox(this);
    mCuePreWaitWidget = new QTimeEdit(this);
    mCuePostWaitWidget = new QTimeEdit(this);
    
    mTitleSplitter = new QWidget(this);

    mCueNameWidget->setObjectName("PropsCueTitle");
    mCueIndexWidget->setObjectName("PropsCueIndex");
    mCueDescriptionWidget->setObjectName("PropsCueDesc");


    //Construct Layout

    // Wrap SVG widget in another widget to create padding
    QWidget* iconHolder = new QWidget(this);
    iconHolder->setLayout(new QHBoxLayout);
    iconHolder->layout()->addWidget(mCueIconWidget);
    iconHolder->layout()->setContentsMargins(4,4,4,4);
    iconHolder->setFixedSize(BOX_SIZE,BOX_SIZE);
    iconHolder->setObjectName("PropsCueIcon");

    mCueIndexWidget->setFixedSize(BOX_SIZE,BOX_SIZE);
    mCueNameWidget->setMaximumHeight(BOX_SIZE);
    mCueIndexWidget->setAlignment(Qt::AlignCenter);

    auto* layout = new QHBoxLayout();
    mTitleSplitter->setLayout(layout);
    layout->setContentsMargins(NO_MARGIN);
    layout->setSpacing(SPACING);
    layout->addWidget(mCueIndexWidget);
    layout->addWidget(iconHolder);
    layout->addWidget(mCueNameWidget);
    
    //QLabel* title = new QLabel("General Settings");
    //title->setObjectName("PropsPageTitle");
    //this->layout()->addWidget(title);
    this->layout()->addWidget(mTitleSplitter);
    this->layout()->addWidget(mCueDescriptionWidget);
    constexpr int labelWidth = 125;
    this->layout()->addWidget(labelled("Next action:", mCueNextActionWidget, labelWidth));
    this->layout()->addWidget(labelled("Pre wait:", mCuePreWaitWidget, labelWidth));
    this->layout()->addWidget(labelled("Post wait:", mCuePostWaitWidget,labelWidth));

    mCueNameWidget->setPlaceholderText("Cue Name");
    mCueDescriptionWidget->setPlaceholderText("Cue Description");


    mCuePreWaitWidget->setDisplayFormat("mm:ss.zzz");
    mCuePreWaitWidget->setMaximumTime(QTime(59,59,999)); // 1 hour
    mCuePreWaitWidget->setCurrentSection(QDateTimeEdit::SecondSection);
    mCuePostWaitWidget->setDisplayFormat("mm:ss.zzz");
    mCuePostWaitWidget->setMaximumTime(QTime(59,59,999)); // 1 hour
    mCuePostWaitWidget->setCurrentSection(QDateTimeEdit::SecondSection);


    // TEMPORARY

    mCueIndexWidget->setText("3");
    mCueIconWidget->load(IconManager::getIconPathForCueType("audio"));
    mCueNameWidget->setText("Faasza title");


}
bool CueGeneralPage::supportsCue(Cue& cue) { return true; }


TextCuePage::TextCuePage(QWidget* parent) : PropertyPage(parent) {
    auto* label = new QLabel("Text cue idk", this);
    this->layout()->addWidget(label);
}
bool TextCuePage::supportsCue(Cue& cue) { return true; }



IdkRandomPage::IdkRandomPage(QWidget* parent) : PropertyPage(parent) {
    auto* label = new QLabel("Random stuff\nyeey multiple lines\nidk", this);
    this->layout()->addWidget(label);
}