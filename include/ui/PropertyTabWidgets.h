#pragma once

#include <QTabWidget>
#include <QComboBox>
#include <QTimeEdit>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qsvgwidget.h>
#include <qtextedit.h>
#include <qwidget.h>

#include "backend/Cue.h"

using namespace BBackend;

class PropertyPage : public QWidget {
    Q_OBJECT
protected:
    PropertyPage(QWidget* parent);
public:
    virtual QString getPageName() = 0;    
    virtual bool supportsCue(Cue& cue) = 0;
};

class CueGeneralPage : public PropertyPage {
    Q_OBJECT
public:
    explicit CueGeneralPage(QWidget* parent);
    QString getPageName() override { return "Cue"; }
    bool supportsCue(Cue& cue) override;
private:
    QLabel* mCueIndexWidget;
    QSvgWidget* mCueIconWidget;
    QLineEdit* mCueNameWidget;
    QTextEdit* mCueDescriptionWidget;
    QComboBox* mCueNextActionWidget;
    QTimeEdit* mCuePreWaitWidget;
    QTimeEdit* mCuePostWaitWidget;

    // For layout
    QWidget* mTitleSplitter;
};

class TextCuePage : public PropertyPage {
    Q_OBJECT
public:
    explicit TextCuePage(QWidget* parent);
    QString getPageName() override { return "Text cue"; }
    bool supportsCue(Cue& cue) override;
};

class IdkRandomPage : public PropertyPage {
    Q_OBJECT
public:
    explicit IdkRandomPage(QWidget* parent);
    QString getPageName() override { return "Random stuff"; }
    bool supportsCue(Cue& cue) override { return true; }
};
