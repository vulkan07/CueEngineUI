#pragma once

#include <QDialog>
#include <QAction>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QSpinBox>


class SettingsShortcutsPage : public QWidget {
    Q_OBJECT
private:
    QFormLayout* mLayout;
public:
    explicit SettingsShortcutsPage(QWidget* parent);
    void addShortcut(const QString& name, QAction* action);
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
    void applyPressed();
    void cancelPressed();
    void okPressed();
    void applySettings();
};


class AboutCueEngineWidget : public QDialog {
    Q_OBJECT
public:
    explicit AboutCueEngineWidget(QWidget* parent);
};