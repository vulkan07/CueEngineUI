#pragma once


#include <QLabel>
#include <QFrame>
#include <QWidget>
#include <QLCDNumber>
#include <QTimer>
#include <QTabWidget>
#include <QSvgWidget>

#include <qcontainerfwd.h>
#include <vector>

#include "ui/PropertyTabWidgets.h"
#include "ui/CueListWidget.h"


class BPanel : public QFrame {
    Q_OBJECT
public:
    explicit BPanel(QWidget* parent);
};




class StatusPanel : public BPanel {
    Q_OBJECT
private:
    QLCDNumber* mLCD;    
    QLabel* mTitleLabel;
public:
    explicit StatusPanel(QWidget* parent);
    void updateTime();
    void resizeEvent(QResizeEvent* event) override;
};


class PlayingPanel : public BPanel {
    Q_OBJECT
public:
    explicit PlayingPanel(QWidget* parent);
};


class CueListPanel : public BPanel {
    Q_OBJECT
private:
    QScrollArea* mScrollWidget;
    CueListHeader* mHeaderWidget;
    CueListWidget* mCueListWidget;

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


class CueItemWidget : public QFrame {
    Q_OBJECT
public:
    explicit CueItemWidget(QString cueType, QString displayName, QWidget* parent=nullptr);

    void mousePressEvent(QMouseEvent* event) override;
private:
    QLabel* mLabel;
    QSvgWidget* mSvgWidget;

    QString mCueType;

};

// kk!
class CuePikkerWidget : public QFrame {
    Q_OBJECT
public:
    explicit CuePikkerWidget(QWidget* parent);
private:
    QLabel* mTitleLabel;
    QFrame* mCuesFrame;

    inline QWidget* constructCueCategory(QString name, std::vector<CueItemWidget*> widgets);
};

class MiscPanel : public BPanel {
    Q_OBJECT
public:
    explicit MiscPanel(QWidget* parent);
private:
    CuePikkerWidget* mCuePicker;
};
