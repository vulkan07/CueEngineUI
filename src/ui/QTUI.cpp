#include "ui/QTUI.h"


#include <QVBoxLayout>
#include <QSplitter>
#include <QFile>
#include <QDir>

#include <iostream>

QTUI::QTUI(QWidget* parent)
    : QFrame(parent) {

}

void QTUI::start() {
    this->applyTheme("resources/themes/default.qss");

    auto* layout = new QVBoxLayout(this);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    QSplitter* lSplitter = new QSplitter(Qt::Vertical, this);
    QSplitter* rSplitter = new QSplitter(Qt::Vertical, this);

    BPanel* p1 = new BPanel(lSplitter);
    BPanel* p2 = new BPanel(lSplitter);
    BPanel* p3 = new StatusPanel(rSplitter);
    BPanel* p4 = new TestPanel(rSplitter);
    
    
    splitter->setHandleWidth(7);
    lSplitter->setHandleWidth(7);
    rSplitter->setHandleWidth(7);

    lSplitter->addWidget(p1);
    lSplitter->addWidget(p2);
    rSplitter->addWidget(p3);
    rSplitter->addWidget(p4);

    splitter->addWidget(lSplitter);
    splitter->addWidget(rSplitter);


    splitter->setContentsMargins(0,0,0,0);
    lSplitter->setContentsMargins(0,0,0,0);
    rSplitter->setContentsMargins(0,0,0,0);
    

    layout->addWidget(splitter);    
    this->setContentsMargins(3,4,3,4);
    layout->setContentsMargins(0,0,0,0);

    this->setWindowTitle("CueEngine");
    this->setLayout(layout);
    this->show();
}

void QTUI::applyTheme(QString path) {
    path.prepend("/").prepend(QDir::currentPath());
    qApp->setStyleSheet(loadTheme(path));
}



/*================== Qt C++ Wisdom ==================

(1) add "Q_OBJECT" macro to any qt inheriting class

(2) Qt objects on the heap should have a parent set to be deleted properly 


===================================================*/