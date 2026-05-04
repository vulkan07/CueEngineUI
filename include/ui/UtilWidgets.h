#pragma once

#include <QDialog>
#include <QAction>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QSpinBox>

#include "ui/Shortcut.h"

class SettingsShortcutsPage : public QWidget {
    Q_OBJECT
private:
    QFormLayout* mLayout;
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