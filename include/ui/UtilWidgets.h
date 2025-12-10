#pragma once

#include <QDialog>
#include <QAction>

class AboutCueEngineWidget : public QDialog {
    Q_OBJECT
private:
    QAction* mCloseAction;
public:
    AboutCueEngineWidget(QWidget* parent);
};