#include "ui/QTUI.h"

#include <QVBoxLayout>
#include <QWindow>
#include <QFileDialog>
#include <QFile>
#include <QDir>

#include <iostream>
#include <QVariant>


QTUI::QTUI(QWidget* parent) : QFrame(parent) {

    // Minimal splash screen image (QSplashScreen adds startup delay while this does not)
    // Disable with --nosplash
    mSplashWidget = new QLabel();
    if (!QCoreApplication::arguments().contains("--nosplash")) {
        QPixmap pixmap(":/assets/images/splash.png");
        if (!pixmap.isNull()) {
            mSplashWidget->setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint);
            mSplashWidget->setPixmap(pixmap);
            mSplashWidget->setFixedSize(pixmap.size());
            mSplashWidget->show();
            mSplashWidget->repaint();
            mSplashWidget->raise();
            while (!mSplashWidget->windowHandle() || !mSplashWidget->windowHandle()->isExposed())
                QCoreApplication::processEvents(); // Make sure the splash window appears before any thread blocking
        }
    }
}

void QTUI::start() {
    this->setWindowTitle( QString("%1 (v%2)").arg(APP_NAME, APP_VERSION) );
    QApplication::setWindowIcon(QIcon(":/assets/images/app-icon.png"));

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
    mAboutAppAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Cue Engine", this);
    mAboutQtAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Qt", this);


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

    connect(mAboutAppAction, &QAction::triggered, this, &QTUI::onAboutAppAction);
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
    mAboutMenu->addAction(mAboutAppAction);
    mAboutMenu->addAction(mAboutQtAction);

    ShortcutManager::registerAction(ShortcutId::APP_TOGGLE_SECONDARY_WINDOW, mSecondaryWindowAction);
    ShortcutManager::registerAction(ShortcutId::APP_OPEN_SETTINGS, mPreferencesAction);
    ShortcutManager::registerAction(ShortcutId::APP_EXIT, mExitAction);
    //ShortcutManager::registerAction(ShortcutId::, mDuplicateAction);

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

    this->setLayout(layout);
    mSecondaryWindow = new SecondaryWindow(); // starts hidden
    connect(mSecondaryWindow, &SecondaryWindow::closed, this, [&]{mSecondaryWindowAction->setChecked(false);});

    ShortcutManager::loadShortcutsFromSettings();

    this->show();
    mSplashWidget->hide();
    mSplashWidget->deleteLater();
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
    Theme t(path);
    qApp->setStyleSheet(t.mStylesheet);
}


void QTUI::onNewAction() {}
void QTUI::onOpenAction() {
    // Mostly just a test for file dialogs
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Session");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("Session Files (*.cep)");
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec()) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            QString sessionPath = files.first();
            // TODO session load
        }
    }
}
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

void QTUI::onAboutAppAction() {
    AboutAppWidget* w = new AboutAppWidget(this);
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


/*================== Qt C++ Wisdom ==================

(1) add "Q_OBJECT" macro to any qt inheriting class
    + add the .h file to the CmakeLists qt_add_executable() as autoMOC doesn't work as of now  

(2) Qt objects on the heap should have a parent set to be deleted properly 

(3) Compile asset files in the CMakeLists to avoid file path issues 

(4) Both the widget, and its layout have content margins set bruh

(5) Q_OBJECTs don't support templates

===================================================*/