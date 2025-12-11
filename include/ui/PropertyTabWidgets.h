#pragma once

#include <QTabWidget>

#include "backend/Cue.h"

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
