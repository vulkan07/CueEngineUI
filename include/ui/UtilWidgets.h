#pragma once

#include <QDialog>
#include <QAction>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QFormLayout>


class SettingsShortcutsPage : public QWidget {
    Q_OBJECT
private:
    QFormLayout* mLayout;
public:
    explicit SettingsShortcutsPage(QWidget* parent);
    void addShortcut(const QString& name, QAction* action);
    void applyShortcut(QAction* action, const QKeySequence& sequence);
};

class SettingsWidget : public QDialog {
    Q_OBJECT
private:
    QTabWidget* mTabsWidget;
    QDialogButtonBox* mButtons;
    SettingsShortcutsPage* mShortcutsPage;
public:
    explicit SettingsWidget(QWidget* parent);
    void applyPressed();
    void cancelPressed();
    void okPressed();
    void applySettings();
};


class AboutCueEngineWidget : public QDialog {
    Q_OBJECT
private:
    QAction* mCloseAction;
public:
    explicit AboutCueEngineWidget(QWidget* parent);
};