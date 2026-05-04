#include "ui/UtilWidgets.h"
#include "ui/AnimationClock.h"
#include "ui/QTUI.h"

FoldingWidget::FoldingWidget(QWidget* parent)
        : FoldingWidget(nullptr, parent) {
}
FoldingWidget::FoldingWidget(QWidget* contentWidget, QWidget* parent)
        : QFrame(parent), mWidget(contentWidget), mAnimHandle(new AnimationHandle) {
    
    auto* layout = new QVBoxLayout();
    auto* barLayout = new QHBoxLayout();
    this->setLayout(layout);
    
    mButton = new QPushButton(this);
    mButton->setFixedSize(26,26);
    connect(mButton, &QPushButton::pressed, this, [=]{this->setOpen(!this->isOpen());});
    mLabel = new QLabel(this);
    barLayout->addWidget(mLabel);
    barLayout->addStretch();
    barLayout->addWidget(mButton);
    layout->addLayout(barLayout);

    auto* line = new QFrame(this);
    line->setObjectName("Line");
    line->setFrameShape(QFrame::HLine);
    line->setLineWidth(1);
    line->setMidLineWidth(0);
    layout->addWidget(line);

    this->setWidget(contentWidget);
    this->setOpen(true),

    connect(&AnimationClock::getInstance(), &AnimationClock::tick, this, &FoldingWidget::animationTick);
}
void FoldingWidget::setWidget(QWidget* widget) { 
    // Remove old if exists
    if (mWidget)
        this->layout()->removeWidget(mWidget);
    
    mWidget = widget;

    // Set new if exists 
    if (mWidget)
        this->layout()->addWidget(mWidget);

    // Redraw
    this->updateGeometry();
    this->update();
}
void FoldingWidget::animationTick(float dt) {
    if (!mAnimHandle->isRunning()) return;
    
}
QWidget* FoldingWidget::widget() {
    return mWidget;
}
void FoldingWidget::setTitle(const QString& title) {
    mLabel->setText(title);
}
QString FoldingWidget::title() const {
    return mLabel->text();
}
void FoldingWidget::setOpen(bool open) {
    if (open == mOpen) return;
    mOpen = open;
    if (mWidget)
        mWidget->setVisible(open);
    if (open && mWidget)
        this->setMaximumHeight(QWIDGETSIZE_MAX);
    else
        this->setMaximumHeight(70);
}
bool FoldingWidget::isOpen() {
    return mOpen;
}

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
    connect(mButtons->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsWidget::onApplyPressed);
    connect(mButtons->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsWidget::onOkPressed);
    connect(mButtons->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SettingsWidget::onCancelPressed);


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
void SettingsWidget::onApplyPressed() {
    this->applySettings();
}
void SettingsWidget::onCancelPressed() {
    this->reject();
}
void SettingsWidget::onOkPressed() {
    this->applySettings();
    this->accept();
}
void SettingsWidget::applySettings() {
    mInterfacePage->apply();
    ShortcutManager::saveShortcutsToSettings();
}

ShortcutWidget::ShortcutWidget(ShortcutId shortcutId, QWidget* parent)
        : QFrame(parent), mShortcutId(shortcutId) {
    auto* layout = new QHBoxLayout(this);
    this->setLayout(layout);

    mNameLabel = new QLabel("???",this);
    mKeySequenceEdit = new QKeySequenceEdit(this);
    mKeySequenceEdit->setFixedWidth(320);
    mRemoveButton = new QPushButton(this);
    mRemoveButton->setFixedSize(24,24);
    layout->addWidget(mNameLabel);
    layout->addWidget(mKeySequenceEdit);
    layout->addWidget(mRemoveButton);

    auto shortcutData = GetShortcutData(shortcutId);
    if (shortcutData) {
        mActionRef = ShortcutManager::getAction(shortcutId);
        if (mActionRef) {
            mNameLabel->setText(shortcutData->displayText);
            mKeySequenceEdit->setKeySequence(mActionRef->shortcut());
        }
    }
}

SettingsShortcutsPage::SettingsShortcutsPage(QWidget* parent) : QWidget(parent) {
    this->setLayout(new QVBoxLayout);
    mScrollWidget = new QScrollArea(this);
    mScrollWidget->setWidgetResizable(true);
    mScrollWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mScrollContent = new QWidget(mScrollWidget);
    mScrollContent->setLayout(new QVBoxLayout);
    this->layout()->addWidget(mScrollWidget);
    mScrollWidget->setWidget(mScrollContent);

    for (int i = 0; i < static_cast<int>(ShortcutCategory::_COUNT_); i++) {
        auto* fw = new FoldingWidget(this);
        auto* w = new QWidget(fw);
        w->setLayout(new QVBoxLayout);
        fw->setWidget(w);
        fw->setTitle(ShortcutCategoryNames[i]);
        mScrollContent->layout()->addWidget(fw);
        mCategoryWidgets.push_back(fw);
    }
    mScrollContent->layout()->addWidget(new QWidget(mScrollContent));

    auto actions = ShortcutManager::getActions();
    for (auto i = actions.cbegin(), end = actions.cend(); i != end; ++i) {
        this->addShortcut(i.key(), i.value());
    }
}

void SettingsShortcutsPage::addShortcut(ShortcutId shortcutId, QAction* action) {
    auto* shortcutData = GetShortcutData(shortcutId);
    if (!shortcutData) return;

    auto categoryWidget = mCategoryWidgets[static_cast<int>(shortcutData->shortcutCategory)];
    auto* widget = new ShortcutWidget(shortcutId, categoryWidget);
    categoryWidget->widget()->layout()->addWidget(widget);
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

    mAnimFrameRateSpinBox->setMinimum(AnimationClock::MIN_FRAME_RATE);
    mAnimFrameRateSpinBox->setMaximum(AnimationClock::MAX_FRAME_RATE);
    
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
