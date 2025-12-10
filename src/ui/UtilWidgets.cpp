#include "ui/UtilWidgets.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

AboutCueEngineWidget::AboutCueEngineWidget(QWidget* parent)
    : QDialog(parent) {

    auto* layout = new QVBoxLayout(this);
    this->setLayout(layout);
    
    QLabel* l = new QLabel("balls",this);
    QPushButton* btn = new QPushButton("OK",this);

    setFixedWidth(300);
    setFixedHeight(200);

    btn->setObjectName("AboutOkButton");

    layout->addWidget(l);
    layout->addWidget(btn);

    mCloseAction = new QAction(this);
    mCloseAction->setShortcut(QKeySequence::Close);


    connect(mCloseAction, &QAction::triggered, this, &AboutCueEngineWidget::done);
    connect(btn, &QPushButton::pressed, this, [=](){this->done(0);}); // hmm cursed

}
