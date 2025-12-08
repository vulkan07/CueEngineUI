#pragma once

#include <QLabel>
#include <QFrame>
#include <QWidget>
#include <QLCDNumber>
#include <QTimer>

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
    QTimer* mClockTimer;
    QTimer* mSyncTimer;
    bool mSynced = false;
public:
    explicit StatusPanel(QWidget* parent);
    void updateTime();
    void updateTitleSize();
    void resizeEvent(QResizeEvent* event) override;
    void syncTimer();
};