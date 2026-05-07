#include "ui/PanelWidgets.h"
#include "ui/UtilWidgets.h"
#include "ui/Theme.h"
#include "ui/Shortcut.h"

#include <QLabel>
#include <QFrame>
#include <QWidget>
#include <QVBoxLayout>
#include <QApplication>
#include <QString>
#include <QMenuBar>
#include <QMenu>
#include <QMap>
#include <QSplitter>
#include <QResizeEvent>
#include <QSettings>
#include <QShortcut>


const char* const APP_NAME = "Cue Engine";
const char* const APP_VERSION = "0.1";


class SecondaryWindow : public QFrame {
    Q_OBJECT
public:
    explicit SecondaryWindow();
    void closeEvent(QCloseEvent* event) override;
signals:
    void closed();
};


class QTUI : public QFrame {
    Q_OBJECT
private:
    QLabel* mSplashWidget;

    QSplitter* mMainSplitter;
    QSplitter* mLeftSplitter;
    QSplitter* mRightSplitter;

    SecondaryWindow* mSecondaryWindow;

    QMenuBar* mMenubar;
    QMenu* mFileMenu;
    QMenu* mEditMenu;
    QMenu* mLayoutMenu;
    QMenu* mWindowMenu;
    QMenu* mAboutMenu;

    // File menu
    QAction* mNewAction;
    QAction* mOpenAction;
    QAction* mSaveAction;
    QAction* mSaveAsAction;
    QAction* mSettingsAction;
    QAction* mExitAction;

    // Window menu
    QAction* mSecondaryWindowAction;

    // About menu
    QAction* mAboutAppAction;
    QAction* mAboutQtAction;

    inline void createActions(); // Create QActions held by this widget (application-wide shortcuts usually)
    inline void createMenus(); // Construct the main top menubar from the existing QActions
public:
    explicit QTUI(QWidget* parent);
    void start();
    void applyTheme(QString path);

    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    static constexpr int RIGHT_PANEL_WIDTH = 250;
    static constexpr int TOP_PANEL_HEIGHT = 225;
    static constexpr int BOTTOM_PANEL_HEIGHT = 300;
};