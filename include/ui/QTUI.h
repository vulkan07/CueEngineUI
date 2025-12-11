#include "ui/PanelWidgets.h"
#include "ui/UtilWidgets.h"
#include "ui/Theme.h"

#include <QLabel>
#include <QFrame>
#include <QWidget>
#include <QVBoxLayout>
#include <QApplication>
#include <QString>
#include <QMenuBar>
#include <QMenu>
#include <QMap>


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
    QAction* mPreferencesAction;
    QAction* mExitAction;

    // Edit menu
    QAction* mUndoAction;
    QAction* mRedoAction;
    QAction* mCopyAction;
    QAction* mCutAction;
    QAction* mPasteAction;
    QAction* mDuplicateAction;
    QAction* mSelectAllAction;
    QAction* mDeselectAllAction;

    // Layout menu
    QAction* mAutoAdvanceAction; // select next cue after starting one

    // Window menu
    QAction* mSecondaryWindowAction;

    // About menu
    QAction* mAboutCueEngineAction;
    QAction* mAboutQtAction;
public:
    explicit QTUI(QWidget* parent);
    void start();
    void applyTheme(QString path);

    void closeEvent(QCloseEvent* event) override;

    void onNewAction();
    void onOpenAction();
    void onSaveAction();
    void onSaveAsAction();
    void onPreferencesAction();
    void onExitAction();
    
    void onUndoAction();
    void onRedoAction();
    void onCopyAction();
    void onCutAction();
    void onPasteAction();
    void onDuplicateAction();
    void onSelectAllAction();
    void onDeselectAllAction();

    void onAutoAdvanceFunction();

    void onSecondaryWindowAction();

    void onAboutCueEngineAction();
    void onAboutQtAction();
};


// Register QActions here that should appear in the settings' shortcuts page

// TODO probably a list/vector should be used as the order of shortcuts is arbitrary from QMap
class ShortcutManager : public QObject {
    Q_OBJECT
private:
    static QMap<QString, QAction*> actions;
public:
    static void registerAction(const QString& id, QAction* action);
    
    // also return an iterator maybe rather than the list itself?
    static const QMap<QString, QAction*>& getActions();
};