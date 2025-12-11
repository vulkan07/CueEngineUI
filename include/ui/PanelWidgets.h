#pragma once


#include <QLabel>
#include <QFrame>
#include <QWidget>
#include <QLCDNumber>
#include <QTimer>
#include <QTabWidget>

#include "ui/PropertyTabWidgets.h"

class BPanel : public QFrame {
    Q_OBJECT
public:
    explicit BPanel(QWidget* parent);
};


class TestPanel : public BPanel {
    Q_OBJECT
public:
    explicit TestPanel(QWidget* parent);
};


class StatusPanel : public BPanel {
    Q_OBJECT
private:
    QLCDNumber* mLCD;    
    QLabel* mTitleLabel;
public:
    explicit StatusPanel(QWidget* parent);
    void updateTime();
    void updateTitleSize();
    void resizeEvent(QResizeEvent* event) override;
};


class PlayingPanel : public BPanel {
    Q_OBJECT
public:
    explicit PlayingPanel(QWidget* parent);
};


class CueListPanel : public BPanel {
    Q_OBJECT
public:
    explicit CueListPanel(QWidget* parent);
};


class PropertiesPanel : public BPanel {
    Q_OBJECT
private:
    std::vector<PropertyPage*> mPages;
    QTabWidget* mTabWidget;
public:
    explicit PropertiesPanel(QWidget* parent);
    void addPage(PropertyPage* page);
};


class MiscPanel : public BPanel {
    Q_OBJECT
public:
    explicit MiscPanel(QWidget* parent);
};