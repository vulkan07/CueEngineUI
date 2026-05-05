#pragma once

#include <QDialog>
#include <QScrollArea>
#include <QAction>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QKeySequenceEdit>
#include <QFormLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

#include <vector>

#include "ui/Shortcut.h"
#include "ui/AnimationClock.h"

class FoldingWidget : public QFrame {
    Q_OBJECT
private:
    QWidget* mWidget;
    QLabel* mLabel;
    QPushButton* mButton;
    AnimationHandle* mAnimHandle;
    float mCurrentPos;
    float mTargPos;
    bool mOpen = true;
public:
    explicit FoldingWidget(QWidget* parent);
    explicit FoldingWidget(QWidget* contentWidget, QWidget* parent);
    void setTitle(const QString& title);
    void setWidget(QWidget* widget);
    QWidget* widget();
    QString title() const;
    
    void toggleOpen();
    void setOpen(bool open);
    bool isOpen();

    void animationTick(float dt);
};


//--------------------------------------------------------------------------------
// TODO: implement SettingsPage class to unify handling of loading/saving/discarding
// settings properly with states rather than QSetting writes everywhere.
// This would also allow settings fields to show if they are modified and not applied yet.
// Sth like this:
struct ShortcutSettingsState {/*...*/};
struct InterfaceSettingsState {/*...*/};
struct SettingsState {
    ShortcutSettingsState shortcuts;
    InterfaceSettingsState interface;
    // ...
};

class SettingsPage : public QWidget {
    Q_OBJECT
private:
    void* mBaseState; // some adequate type instead
    void* mNewState;
public:
    explicit SettingsPage(QWidget* parent);

    void fromSettingsState(void* state);
    void applySettings(void* state);
};
//--------------------------------------------------------------------------------


// Encompasses a line in the shortcut editor for each action
class ShortcutWidget : public QFrame {
    Q_OBJECT
private:
    QLabel* mNameLabel;
    QKeySequenceEdit* mKeySequenceEdit1;
    QKeySequenceEdit* mKeySequenceEdit2;
    QPushButton* mRemoveButton;
    QAction* mActionRef = nullptr; // Points to the QAction that this shortcut field belongs to
    ShortcutId mShortcutId;
    void onShortcutModified(QKeySequenceEdit* widget);
public:
    explicit ShortcutWidget(ShortcutId shortcutId, QWidget* parent);
};

class SettingsShortcutsPage : public QWidget {
    Q_OBJECT
private:
    QScrollArea* mScrollWidget;
    QWidget* mScrollContent;
    std::vector<FoldingWidget*> mCategoryWidgets; // Index of the widgets map to ShortcutCategory enum
public:
    explicit SettingsShortcutsPage(QWidget* parent);
    void addShortcut(ShortcutId shortcutId, QAction* action);
    void applyShortcut(QAction* action, const QKeySequence& sequence);
};

class SettingsInterfacePage : public QWidget {
    Q_OBJECT
private:
    QFormLayout* mLayout;
    QCheckBox* mDoAnimationsCheckBox;
    QSpinBox* mAnimFrameRateSpinBox;
public:
    explicit SettingsInterfacePage(QWidget* parent);
    void apply();
};

class SettingsWidget : public QDialog {
    Q_OBJECT
private:
    QTabWidget* mTabsWidget;
    QDialogButtonBox* mButtons;
    SettingsShortcutsPage* mShortcutsPage;
    SettingsInterfacePage* mInterfacePage;
public:
    explicit SettingsWidget(QWidget* parent);
    void onApplyPressed();
    void onCancelPressed();
    void onOkPressed();
    void applySettings();
};


class AboutCueEngineWidget : public QDialog {
    Q_OBJECT
public:
    explicit AboutCueEngineWidget(QWidget* parent);
};