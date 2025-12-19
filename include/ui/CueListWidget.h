#pragma once

#include <QScrollArea>

#include <vector>
#include <array>

#include "ui/AnimationClock.h"

enum class CueListColumnTypes : int {
    INDEX,
    NAME,
    DESCRIPTION,
    PRE_WAIT,
    DURATION,
    POST_WAIT,
    NEXT_ACTION,
    _COUNT_
};

enum class ResizeMode : int {
    INTERACTIVE,
    FIXED,
    STRETCHING,
};

struct ColumnData {
    const char* name;
    int width;
    int textAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    ResizeMode ResizeMode;
};

static std::array<ColumnData, static_cast<int>(CueListColumnTypes::_COUNT_)> CueListColumns {{
    {"#", 28, Qt::AlignCenter, ResizeMode::FIXED},
    {"Name", 180},
    {"Description", 300, Qt::AlignVCenter | Qt::AlignLeft, ResizeMode::STRETCHING},
    {"Pre Wait", 100},
    {"Duration", 110},
    {"Post Wait", 100},
    {"", 28, Qt::AlignCenter, ResizeMode::FIXED},
}};


class CueListHeader : public QWidget {
    Q_OBJECT
private:
    std::vector<QWidget*> mWidgets;
public:
    explicit CueListHeader(QWidget* parent = nullptr);
    int getHeaderWidth(int index) const;
};

class CueListWidget : public QWidget {
    Q_OBJECT
private:
    float mCursorPos = 0;
    float mTargetCursorPos = 0;
    AnimationHandle* mAnimHandle = nullptr;

    int mStandbyIndex;
    const CueListHeader* const header; // only reference, not owned by this
public:
    static const int GAP_WIDTH = 2;
    static const int ROW_HEIGHT = 28;
    static const int CELL_PADDING = 4;
    static const int ROW_TOTAL_H = ROW_HEIGHT+GAP_WIDTH;

    explicit CueListWidget(CueListHeader* const header, QWidget* parent = nullptr);

    void onUpAction();
    void onDownAction();
    void onPlayAction();

    void setStandbyIndex(int index);
    int standbyIndex();

    void animationTick(float dt);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};