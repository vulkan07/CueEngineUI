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

class QTUI : public QFrame {
    Q_OBJECT
private:
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