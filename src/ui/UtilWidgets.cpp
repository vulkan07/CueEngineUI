#include "ui/UtilWidgets.h"
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

    mButtons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel,
        this);
    mButtons->setMaximumHeight(50);
    connect(mButtons->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsWidget::applyPressed);
    connect(mButtons->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsWidget::okPressed);
    connect(mButtons->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SettingsWidget::cancelPressed);

    mShortcutsPage = new SettingsShortcutsPage(this);

    auto actions = ShortcutManager::getActions();
    for (auto i = actions.cbegin(), end = actions.cend(); i != end; ++i) {
        mShortcutsPage->addShortcut(i.key(), i.value());
    }

    //Temporary
    auto* label = new QLabel("Note: settings get applied immediately as of now!");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color:gold; font-weight:bold; margin: 4px;");
    layout->addWidget(label);
    //Temporary

    layout->addWidget(mTabsWidget);
    layout->addWidget(mButtons);
    mTabsWidget->addTab(mShortcutsPage, "Shortcuts");
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
    ShortcutManager::saveShortcutsToSettings();
    ShortcutManager::loadShortcutsFromSettings(); // REMOVE TODO test xxx 
}


SettingsShortcutsPage::SettingsShortcutsPage(QWidget* parent) : QWidget(parent) {
    mLayout = new QFormLayout(this);
    this->setLayout(mLayout);
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
