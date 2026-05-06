#include "ui/UtilWidgets.h"
#include "ui/AnimationClock.h"
#include "ui/QTUI.h"
#include <spdlog/spdlog.h>

FoldingWidget::FoldingWidget(QWidget* parent)
        : FoldingWidget(nullptr, parent) {
}
FoldingWidget::FoldingWidget(QWidget* contentWidget, QWidget* parent)
        : QFrame(parent), mWidget(contentWidget), mAnimHandle(new AnimationHandle) {
    
    auto* layout = new QVBoxLayout();
    auto* barLayout = new QHBoxLayout();
    this->setLayout(layout);
    layout->setContentsMargins(2,2,2,2);
    
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
    line->setFixedHeight(2);
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
        this->setMaximumHeight(60);
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
    this->setWindowTitle(QString(APP_NAME)+": Settings");

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
    auto* label = new QLabel("Note: fos");
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

void ShortcutWidget::onShortcutModified(QKeySequenceEdit* widget) {
    mRemoveButton->setEnabled(
        !(mKeySequenceEdit1->keySequence().isEmpty() && mKeySequenceEdit2->keySequence().isEmpty())
    );

    auto sequence = widget->keySequence();
    if (sequence.isEmpty())
        return;

    int key = sequence[0].key();

    // For better UX, since one edit widget can only store one sequence,
    // end recording if the sequence is finished (any non-modifier key pressed) 
    if (!( key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta)) {
        widget->clearFocus();
    }
}

ShortcutWidget::ShortcutWidget(ShortcutId shortcutId, QWidget* parent)
        : QFrame(parent), mShortcutId(shortcutId) {

    //this->setProperty("state","modified // invalid"); // change color of widget
    auto* layout = new QHBoxLayout(this);
    this->setLayout(layout);
    layout->setContentsMargins(4,1,4,1);

    mNameLabel = new QLabel("???",this);

    mKeySequenceEdit1 = new QKeySequenceEdit(this);
    mKeySequenceEdit1->setFixedWidth(200);
    mKeySequenceEdit1->setClearButtonEnabled(true);
    mKeySequenceEdit1->setMaximumSequenceLength(1);
    connect(mKeySequenceEdit1, &QKeySequenceEdit::keySequenceChanged, this, [=]{this->onShortcutModified(mKeySequenceEdit1);});

    mKeySequenceEdit2 = new QKeySequenceEdit(this);
    mKeySequenceEdit2->setFixedWidth(200);
    mKeySequenceEdit2->setClearButtonEnabled(true);
    mKeySequenceEdit2->setMaximumSequenceLength(1);
    connect(mKeySequenceEdit2, &QKeySequenceEdit::keySequenceChanged, this, [=]{this->onShortcutModified(mKeySequenceEdit2);});

    mRemoveButton = new QPushButton(this);
    mRemoveButton->setFixedSize(24,24);
    connect(mRemoveButton, &QPushButton::clicked, this, [=]{ // Button press clears both shortcuts
        this->mKeySequenceEdit1->setKeySequence({});
        this->mKeySequenceEdit2->setKeySequence({});
    });

    layout->addWidget(mNameLabel);
    layout->addWidget(mKeySequenceEdit1);
    layout->addWidget(mKeySequenceEdit2);
    layout->addWidget(mRemoveButton);

    // Load sequences from ShortcutManager into the edit widgets
    auto shortcutData = GetShortcutData(shortcutId);
    if (shortcutData) {
        mActionRef = ShortcutManager::getAction(shortcutId);
        if (mActionRef) {
            mNameLabel->setText(shortcutData->displayText);
            auto shortcuts = mActionRef->shortcuts();
            if (shortcuts.length() == 1) {
                mKeySequenceEdit1->setKeySequence(shortcuts[0]);
            }
            else if (shortcuts.length() > 1) {
                mKeySequenceEdit1->setKeySequence(shortcuts[0]);
                mKeySequenceEdit2->setKeySequence(shortcuts[1]);
            }
            if (shortcuts.length() > 2) {
                spdlog::error("Action {} has more than the allowed shortcuts (2)", static_cast<int>(shortcutId));
            }
        }
    }
}

SettingsShortcutsPage::SettingsShortcutsPage(QWidget* parent) : QWidget(parent) {
    this->setLayout(new QVBoxLayout);

    mScrollWidget = new QScrollArea(this);
    mScrollWidget->setWidgetResizable(true);
    mScrollWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->layout()->addWidget(mScrollWidget);

    mScrollContent = new QWidget(mScrollWidget);
    mScrollContent->setLayout(new QVBoxLayout);
    mScrollContent->layout()->setSpacing(10);
    mScrollWidget->setWidget(mScrollContent);

    // Auto-create a folding widget inside thes scroll area for each shortcut category
    for (int i = 0; i < static_cast<int>(ShortcutCategory::_COUNT_); i++) {
        auto* foldingWidget = new FoldingWidget(this);
        auto* contentWidget = new QWidget(foldingWidget); // Contains the ShortcutWidgets in a VboxLayout
        contentWidget->setLayout(new QVBoxLayout);
        contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        contentWidget->layout()->setSpacing(8);
        contentWidget->layout()->setContentsMargins(0,0,0,0);
        foldingWidget->setWidget(contentWidget);
        foldingWidget->setTitle(ShortcutCategoryNames[i]);

        mScrollContent->layout()->addWidget(foldingWidget);
        mCategoryWidgets.push_back(foldingWidget);
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
    // TODO Rework settings applying
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


AboutAppWidget::AboutAppWidget(QWidget* parent)
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
