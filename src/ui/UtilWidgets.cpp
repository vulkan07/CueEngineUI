#include "ui/UtilWidgets.h"
#include "ui/AnimationClock.h"
#include "ui/QTUI.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QKeySequenceEdit>

SettingsWidget::SettingsWidget(QWidget* parent) : QDialog(parent) {

    auto* layout = new QVBoxLayout(this);
    this->setLayout(layout);

    layout->setContentsMargins(6,6,6,0);
    layout->setSpacing(0);

    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle("Settings");

    this->setMinimumSize(QSize(700,550));
    this->resize(QSize(800,640));


    mTabsWidget = new QTabWidget(this);
    mShortcutsPage = new SettingsShortcutsPage(this);
    mInterfacePage = new SettingsInterfacePage(this),

    mButtons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel,
        this);
    mButtons->setMaximumHeight(50);
    connect(mButtons->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsWidget::applyPressed);
    connect(mButtons->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsWidget::okPressed);
    connect(mButtons->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SettingsWidget::cancelPressed);


    //Temporary
    auto* label = new QLabel("Note: settings get applied immediately as of now!");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color:gold; font-weight:bold; margin: 4px;");
    layout->addWidget(label);
    //Temporary

    layout->addWidget(mTabsWidget);
    layout->addWidget(mButtons);
    mTabsWidget->addTab(mShortcutsPage, "Shortcuts");
    mTabsWidget->addTab(mInterfacePage, "Interface");
    mTabsWidget->addTab(new QLabel("blah blah",this), "Stuff");
    mTabsWidget->addTab(new QLabel("alma",this), "More Stuff");
}
void SettingsWidget::applyPressed() {
    this->applySettings();
}
void SettingsWidget::cancelPressed() {
    this->reject();
}
void SettingsWidget::okPressed() {
    this->applySettings();
    this->accept();
}
void SettingsWidget::applySettings() {
    mInterfacePage->apply();
    ShortcutManager::saveShortcutsToSettings();
}


SettingsShortcutsPage::SettingsShortcutsPage(QWidget* parent) : QWidget(parent) {
    mLayout = new QFormLayout(this);
    this->setLayout(mLayout);

    auto actions = ShortcutManager::getActions();
    for (auto i = actions.cbegin(), end = actions.cend(); i != end; ++i) {
        this->addShortcut(i.key(), i.value());
    }
}

void SettingsShortcutsPage::addShortcut(const QString& name, QAction* action) {
    auto* widget = new QKeySequenceEdit(action->shortcut(), this);
    mLayout->addRow(name, widget);
    connect(widget, &QKeySequenceEdit::keySequenceChanged, this, [this, action](const QKeySequence &seq){
        this->applyShortcut(action,seq);
    });
}

void SettingsShortcutsPage::applyShortcut(QAction* action, const QKeySequence& sequence) {
    // TODO apply checks here (eg. if the sequence is already used by another action...)
    action->setShortcut(sequence);
}


SettingsInterfacePage::SettingsInterfacePage(QWidget* parent) : QWidget(parent) {
    mLayout = new QFormLayout(this);
    this->setLayout(mLayout);

    mDoAnimationsCheckBox = new QCheckBox(this);
    mAnimFrameRateSpinBox = new QSpinBox(this);

    mAnimFrameRateSpinBox->setMinimum(20);
    mAnimFrameRateSpinBox->setMaximum(144);
    
    QSettings s;
    s.beginGroup("Interface");
    bool doAnim = s.value("doAnimations", true).toBool();
    auto fpsVal = s.value("AnimationRate");
    s.endGroup();

    auto& animClock = AnimationClock::getInstance();
    if (fpsVal.isValid()){ 
        int fps = fpsVal.toInt();
        mAnimFrameRateSpinBox->setValue(fps);
        animClock.setFrameRate(fps);
    } else {
        mAnimFrameRateSpinBox->setValue(animClock.getFrameRate());
    }
    mDoAnimationsCheckBox->setChecked(doAnim);
    animClock.setAnimationsEnabled(doAnim);
    
    QPushButton* btn = new QPushButton("RESET", this);
    connect(btn, &QPushButton::pressed, this, [this]{
        QSettings().clear();
    });

    mLayout->addRow("Do animations", mDoAnimationsCheckBox);
    mLayout->addRow("Animation FPS", mAnimFrameRateSpinBox);
    mLayout->addRow("Reset all Qt Settings", btn);
}

void SettingsInterfacePage::apply() {
    auto& animClock = AnimationClock::getInstance();
    bool doAnims = mDoAnimationsCheckBox->isChecked();
    int fps = mAnimFrameRateSpinBox->value();
    animClock.setAnimationsEnabled(doAnims);
    animClock.setFrameRate(fps);
    QSettings s;
    s.beginGroup("Interface");
    s.setValue("doAnimations", doAnims);
    s.setValue("AnimationRate", fps);
    s.endGroup();
}


AboutCueEngineWidget::AboutCueEngineWidget(QWidget* parent)
    : QDialog(parent) {

    auto* layout = new QVBoxLayout(this);
    this->setLayout(layout);
    
    QLabel* l = new QLabel("balls",this);
    QPushButton* btn = new QPushButton("Cool",this);

    setFixedWidth(300);
    setFixedHeight(200);

    layout->addWidget(l);
    layout->addWidget(btn);

    connect(btn, &QPushButton::pressed, this, [=](){this->done(0);}); // hmm cursed

}
