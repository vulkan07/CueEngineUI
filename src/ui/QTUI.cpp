#include "ui/QTUI.h"

#include <QVBoxLayout>
#include <QFile>
#include <QDir>

#include <iostream>
#include <QVariant>


QTUI::QTUI(QWidget* parent)
    : QFrame(parent) {

}

void QTUI::start() {

    QCoreApplication::setOrganizationName("KFG Studio");
    QCoreApplication::setApplicationName("Cue Engine");

    this->applyTheme(":/assets/themes/default.qss");

    auto* layout = new QVBoxLayout(this);

    /*---------- Menubar ----------*/
    mMenubar = new QMenuBar(this);
    mMenubar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    mMenubar->setContextMenuPolicy(Qt::PreventContextMenu);

    mFileMenu = mMenubar->addMenu("File");
    mEditMenu = mMenubar->addMenu("Edit");
    mLayoutMenu = mMenubar->addMenu("Layout");
    mWindowMenu = mMenubar->addMenu("Window");
    mAboutMenu = mMenubar->addMenu("About");

    mFileMenu->setToolTipsVisible(true);
    mEditMenu->setToolTipsVisible(true);
    mLayoutMenu->setToolTipsVisible(true);
    mWindowMenu->setToolTipsVisible(true);
    mAboutMenu->setToolTipsVisible(true);

    mNewAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "New", this);
    mOpenAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "Open", this);
    mSaveAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "Save", this);
    mSaveAsAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSaveAs), "Save as", this);
    mPreferencesAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties), "Settings", this);
    mExitAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit), "Exit", this);
    //
    mUndoAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditUndo), "Undo", this);
    mRedoAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditRedo), "Redo", this);
    mCutAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCut), "Cut", this);
    mCopyAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy), "Copy", this);
    mPasteAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste), "Paste", this);
    mDuplicateAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste), "Duplicate", this);
    mSelectAllAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditSelectAll), "Select all", this);
    mDeselectAllAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditSelectAll), "Deselect all", this);
    //
    mAutoAdvanceAction = new QAction("Auto advance", this);
    mAutoAdvanceAction->setToolTip("Automatically select the next cue after starting one");
    mAutoAdvanceAction->setCheckable(true);
    mAutoAdvanceAction->setChecked(true);
    //
    mSecondaryWindowAction = new QAction("Secondary window", this);
    mSecondaryWindowAction->setCheckable(true);
    //
    mAboutCueEngineAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Cue Engine", this);
    mAboutQtAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Qt", this);


    mNewAction->setShortcuts(QKeySequence::New);
    mOpenAction->setShortcuts(QKeySequence::Open);
    mSaveAction->setShortcuts(QKeySequence::Save);
    mSaveAsAction->setShortcuts(QKeySequence::SaveAs);
    mPreferencesAction->setShortcut(QKeySequence("Alt+S"));
    mExitAction->setShortcuts(QKeySequence::Quit);
    //
    mUndoAction->setShortcuts(QKeySequence::Undo);
    mRedoAction->setShortcuts(QKeySequence::Redo);
    mCutAction->setShortcuts(QKeySequence::Cut);
    mCopyAction->setShortcuts(QKeySequence::Copy);
    mPasteAction->setShortcuts(QKeySequence::Paste);
    mDuplicateAction->setShortcut(QKeySequence("Ctrl+D"));
    mSelectAllAction->setShortcuts(QKeySequence::SelectAll);
    mDeselectAllAction->setShortcut(QKeySequence("Ctrl+Shift+A"));

    connect(mNewAction, &QAction::triggered, this, &QTUI::onNewAction);
    connect(mOpenAction, &QAction::triggered, this, &QTUI::onOpenAction);
    connect(mSaveAction, &QAction::triggered, this, &QTUI::onSaveAction);
    connect(mSaveAsAction, &QAction::triggered, this, &QTUI::onSaveAsAction);
    connect(mPreferencesAction, &QAction::triggered, this, &QTUI::onPreferencesAction);
    connect(mExitAction, &QAction::triggered, this, &QTUI::onExitAction);
    
    connect(mUndoAction, &QAction::triggered, this, &QTUI::onUndoAction);
    connect(mRedoAction, &QAction::triggered, this, &QTUI::onRedoAction);
    connect(mCopyAction, &QAction::triggered, this, &QTUI::onCopyAction);
    connect(mCutAction, &QAction::triggered, this, &QTUI::onCutAction);
    connect(mPasteAction, &QAction::triggered, this, &QTUI::onPasteAction);
    connect(mSelectAllAction, &QAction::triggered, this, &QTUI::onSelectAllAction);
    connect(mDeselectAllAction, &QAction::triggered, this, &QTUI::onDeselectAllAction);

    connect(mAutoAdvanceAction, &QAction::triggered, this, &QTUI::onAutoAdvanceFunction);

    connect(mSecondaryWindowAction, &QAction::triggered, this, &QTUI::onSecondaryWindowAction);

    connect(mAboutCueEngineAction, &QAction::triggered, this, &QTUI::onAboutCueEngineAction);
    connect(mAboutQtAction, &QAction::triggered, this, &QTUI::onAboutQtAction);


    mFileMenu->addAction(mNewAction);
    mFileMenu->addAction(mOpenAction);
    mFileMenu->addAction(mSaveAction);
    mFileMenu->addAction(mSaveAsAction);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mPreferencesAction);
    mFileMenu->addAction(mExitAction);
    //
    mEditMenu->addAction(mUndoAction);
    mEditMenu->addAction(mRedoAction);
    mEditMenu->addSeparator();
    mEditMenu->addAction(mCopyAction);
    mEditMenu->addAction(mCutAction);
    mEditMenu->addAction(mPasteAction);
    mEditMenu->addAction(mDuplicateAction);
    mEditMenu->addSeparator();
    mEditMenu->addAction(mSelectAllAction);
    mEditMenu->addAction(mDeselectAllAction);
    //
    mLayoutMenu->addAction(mAutoAdvanceAction);
    //
    mWindowMenu->addAction(mSecondaryWindowAction);
    //
    mAboutMenu->addAction(mAboutCueEngineAction);
    mAboutMenu->addAction(mAboutQtAction);

    ShortcutManager::registerAction("Toggle secondary window", mSecondaryWindowAction);
    ShortcutManager::registerAction("Open settings", mPreferencesAction);
    ShortcutManager::registerAction("Duplicate cues", mDuplicateAction);
    ShortcutManager::registerAction("Exit application", mExitAction);

    layout->addWidget(mMenubar);


    /*---------- Panels ----------*/
    mMainSplitter = new QSplitter(Qt::Horizontal, this);
    mLeftSplitter = new QSplitter(Qt::Vertical, this);
    mRightSplitter = new QSplitter(Qt::Vertical, this);

    BPanel* playingPanel = new PlayingPanel(mLeftSplitter);
    BPanel* cueListPanel = new CueListPanel(mLeftSplitter);
    BPanel* propertiesPanel = new PropertiesPanel(mLeftSplitter);
    BPanel* statusPanel = new StatusPanel(mRightSplitter);
    BPanel* miscPanel = new MiscPanel(mRightSplitter);
    
    mMainSplitter->setHandleWidth(6);
    mLeftSplitter->setHandleWidth(6);
    mRightSplitter->setHandleWidth(6);

    mLeftSplitter->addWidget(playingPanel);
    mLeftSplitter->addWidget(cueListPanel);
    mLeftSplitter->addWidget(propertiesPanel);
    mRightSplitter->addWidget(statusPanel);
    mRightSplitter->addWidget(miscPanel);

    mMainSplitter->addWidget(mLeftSplitter);
    mMainSplitter->addWidget(mRightSplitter);

    mMainSplitter->setContentsMargins(0,0,0,0);
    mLeftSplitter->setContentsMargins(0,0,0,0);
    mRightSplitter->setContentsMargins(0,0,0,0);

    layout->addWidget(mMainSplitter);    
    this->setContentsMargins(2,0,2,3); 
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    mMainSplitter->setSizes({width(), RIGHT_PANEL_WIDTH});  
    mLeftSplitter->setSizes({TOP_PANEL_HEIGHT, height(), BOTTOM_PANEL_HEIGHT});   

    this->setWindowTitle("Cue Engine");
    this->setLayout(layout);
    this->show();
    mSecondaryWindow = new SecondaryWindow(); // starts hidden
    connect(mSecondaryWindow, &SecondaryWindow::closed, this, [&]{mSecondaryWindowAction->setChecked(false);});

    ShortcutManager::loadShortcutsFromSettings();
}

void QTUI::closeEvent(QCloseEvent* event) {
    QFrame::closeEvent(event);
    this->onExitAction();
}

void QTUI::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    
    QList<int> sizes = mMainSplitter->sizes();

    if (sizes.length() != 2) return;

    if (width() < 900) {
        mMainSplitter->setSizes({width(), 0});
    } else {
        mMainSplitter->setSizes({width()-RIGHT_PANEL_WIDTH, RIGHT_PANEL_WIDTH});
    }

    if (height() < 800) {
        mLeftSplitter->setSizes({TOP_PANEL_HEIGHT, height()-TOP_PANEL_HEIGHT, 0});
    } else {
        mLeftSplitter->setSizes({TOP_PANEL_HEIGHT, height()-TOP_PANEL_HEIGHT-BOTTOM_PANEL_HEIGHT, BOTTOM_PANEL_HEIGHT});    
    }
    
}

void QTUI::applyTheme(QString path) {
    QIcon::setThemeName("breeze-dark"); // dark themed icons
    qApp->setStyleSheet(loadTheme(path));
}


void QTUI::onNewAction() {}
void QTUI::onOpenAction() {}
void QTUI::onSaveAction() {}
void QTUI::onSaveAsAction() {}
void QTUI::onPreferencesAction() {
    SettingsWidget(this).exec();
}
void QTUI::onExitAction() {
    qApp->exit(); 
}

void QTUI::onUndoAction() {}
void QTUI::onRedoAction() {}
void QTUI::onCopyAction() {}
void QTUI::onCutAction() {}
void QTUI::onPasteAction() {}
void QTUI::onDuplicateAction() {}
void QTUI::onSelectAllAction() {}
void QTUI::onDeselectAllAction() {}

void QTUI::onAutoAdvanceFunction() {}

void QTUI::onSecondaryWindowAction() {
    mSecondaryWindow->setVisible(mSecondaryWindowAction->isChecked());
}

void QTUI::onAboutCueEngineAction() {
    AboutCueEngineWidget* w = new AboutCueEngineWidget(this);
    w->open();
}
void QTUI::onAboutQtAction() {
    qApp->aboutQt();
}



SecondaryWindow::SecondaryWindow() : QFrame() {
    this->setLayout(new QVBoxLayout(this));
    this->setObjectName("SecondaryWindow");
    this->layout()->addWidget(new QLabel("balls+",this));
    this->setMinimumSize(400,400);
}
void SecondaryWindow::closeEvent(QCloseEvent* event) {
    emit closed();
}



const QString ShortcutManager::SETTINGS_PREFIX = "shortcuts";

QMap<QString, QAction*> ShortcutManager::actions = QMap<QString, QAction*>();

void ShortcutManager::registerAction(const QString& name, QAction* action) {
    ShortcutManager::actions[name] = action;
}

const QMap<QString, QAction*>& ShortcutManager::getActions() {
    return ShortcutManager::actions;
}

void ShortcutManager::loadShortcutsFromSettings() {
    QSettings settings;
    settings.beginGroup(SETTINGS_PREFIX);

    for (auto i = actions.cbegin(), end = actions.cend(); i != end; ++i) {
        
        auto stringList = settings.value(i.key()).toStringList();
        QList<QKeySequence> sequences; 
        
        for (auto sequence : stringList) {
            sequences.push_back(QKeySequence(sequence));
        }

        i.value()->setShortcuts(sequences);
    }
    settings.endGroup();
}

void ShortcutManager::saveShortcutsToSettings() {
    QSettings settings;
    settings.beginGroup(SETTINGS_PREFIX);
    for (auto i = actions.cbegin(), end = actions.cend(); i != end; ++i) {
        QStringList list;
        for (auto s : i.value()->shortcuts()) {
            list.push_back(s.toString());
        }
        settings.setValue(i.key(), list);
    }
    settings.endGroup();
}


/*================== Qt C++ Wisdom ==================

(1) add "Q_OBJECT" macro to any qt inheriting class
    + add the .h file to the CmakeLists qt_add_executable() as autoMOC doesn't work as of now  

(2) Qt objects on the heap should have a parent set to be deleted properly 

(3) Compile asset files in the CMakeLists to avoid file path issues 

(4) Both the widget, and its layout have content margins set bruh

===================================================*/