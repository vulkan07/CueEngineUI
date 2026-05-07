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


    this->createActions();
    this->createMenus();

    ShortcutManager::loadShortcutsFromSettings();

    this->show();
    mSplashWidget->hide();
    mSplashWidget->deleteLater();
}

// Helpers for creating menus from ShortcutIds
static inline void addMenuAction(QMenu* menu, ShortcutId shortcut, QIcon icon) {
    QAction* action = ShortcutManager::getAction(shortcut);
    if (action) {
        action->setIcon(icon);
        menu->addAction(action);
    }
} 
static inline void addMenuAction(QMenu* menu, ShortcutId shortcut, QIcon icon, const char* menuTextOverride) {
    QAction* action = ShortcutManager::getAction(shortcut);
    if (action) {
        action->setIcon(icon);
        action->setText(menuTextOverride);
        menu->addAction(action);
    }
} 
inline void QTUI::createMenus() {

    addMenuAction(mFileMenu, ShortcutId::APP_NEW_SESSION, QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "New");
    addMenuAction(mFileMenu, ShortcutId::APP_OPEN_SESSION, QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "Open");
    addMenuAction(mFileMenu, ShortcutId::APP_SAVE_SESSION, QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "Save");
    addMenuAction(mFileMenu, ShortcutId::APP_SAVE_SESSION_AS, QIcon::fromTheme(QIcon::ThemeIcon::DocumentSaveAs), "Save as");
    mFileMenu->addSeparator();
    addMenuAction(mFileMenu, ShortcutId::APP_OPEN_SETTINGS, QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties), "Settings");
    addMenuAction(mFileMenu, ShortcutId::APP_EXIT, QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit), "Exit");
    
    addMenuAction(mEditMenu, ShortcutId::APP_UNDO, QIcon::fromTheme(QIcon::ThemeIcon::EditUndo));
    addMenuAction(mEditMenu, ShortcutId::APP_REDO, QIcon::fromTheme(QIcon::ThemeIcon::EditRedo));
    mEditMenu->addSeparator();
    addMenuAction(mEditMenu, ShortcutId::CUELIST_COPY_SELECTED, QIcon::fromTheme(QIcon::ThemeIcon::EditCopy), "Copy");
    addMenuAction(mEditMenu, ShortcutId::CUELIST_CUT_SELECTED, QIcon::fromTheme(QIcon::ThemeIcon::EditCut), "Cut");
    addMenuAction(mEditMenu, ShortcutId::CUELIST_PASTE_SELECTED, QIcon::fromTheme(QIcon::ThemeIcon::EditPaste), "Paste");
    addMenuAction(mEditMenu, ShortcutId::CUELIST_DUPLICATE_SELECTED, QIcon::fromTheme(QIcon::ThemeIcon::EditPaste), "Duplicate");
    addMenuAction(mEditMenu, ShortcutId::CUELIST_DELETE_SELECTED, QIcon::fromTheme(QIcon::ThemeIcon::EditDelete), "Delete");
    mEditMenu->addSeparator();
    addMenuAction(mEditMenu, ShortcutId::CUELIST_SELECT_ALL, QIcon::fromTheme(QIcon::ThemeIcon::EditSelectAll), "Select all");
    addMenuAction(mEditMenu, ShortcutId::CUELIST_DESELECT_ALL, QIcon::fromTheme(QIcon::ThemeIcon::EditSelectAll), "Deselect all");
    
    addMenuAction(mWindowMenu, ShortcutId::APP_TOGGLE_SECONDARY_WINDOW, QIcon::fromTheme(QIcon::ThemeIcon::WindowNew), "Secondary window");
    
    // Not registered shortcuts
    mAboutMenu->addActions({mAboutAppAction, mAboutQtAction});

}
inline void QTUI::createActions() {
    mNewAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "New", this);
    ShortcutManager::registerAction(ShortcutId::APP_NEW_SESSION, mNewAction);
    connect(mNewAction, &QAction::triggered, this, [=]{});

    mOpenAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "Open", this);
    ShortcutManager::registerAction(ShortcutId::APP_OPEN_SESSION, mOpenAction);
    connect(mOpenAction, &QAction::triggered, this, [=]{
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
    });

    mSaveAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "Save", this);
    ShortcutManager::registerAction(ShortcutId::APP_SAVE_SESSION, mSaveAction);
    connect(mSaveAction, &QAction::triggered, this, [=]{});

    mSaveAsAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSaveAs), "Save as", this);
    ShortcutManager::registerAction(ShortcutId::APP_SAVE_SESSION_AS, mSaveAsAction);
    connect(mSaveAsAction, &QAction::triggered, this, [=]{});

    mSettingsAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties), "Settings", this);
    ShortcutManager::registerAction(ShortcutId::APP_OPEN_SETTINGS, mSettingsAction);
    connect(mSettingsAction, &QAction::triggered, this, [=]{
        SettingsWidget(this).exec();
    });

    mExitAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit), "Exit", this);
    ShortcutManager::registerAction(ShortcutId::APP_EXIT, mExitAction);
    connect(mExitAction, &QAction::triggered, this, [=]{
        qApp->exit();
    });

    mSecondaryWindowAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::WindowNew), "Secondary window", this);
    ShortcutManager::registerAction(ShortcutId::APP_TOGGLE_SECONDARY_WINDOW, mSecondaryWindowAction);
    mSecondaryWindowAction->setCheckable(true);
    connect(mSecondaryWindowAction, &QAction::triggered, this, [=]{
        mSecondaryWindow->setVisible(mSecondaryWindowAction->isChecked());
    });

    mAboutAppAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), QString("About")+APP_NAME, this);
    connect(mAboutAppAction, &QAction::triggered, this, [=]{
        AboutAppWidget w(this);
        w.exec();
    });

    mAboutQtAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Qt", this);
    connect(mAboutQtAction, &QAction::triggered, this, [=]{
        qApp->aboutQt();
    });
}

void QTUI::closeEvent(QCloseEvent* event) {
    QFrame::closeEvent(event);
    // TODO exit handling here
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