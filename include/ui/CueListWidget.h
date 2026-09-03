#pragma once

#include <QScrollArea>
#include <QScrollBar>

#include <vector>
#include <array>
#include <functional>

#include "ui/Animation.h"
#include "ui/Shortcut.h"

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
    INTERACTIVE, // can be resized by user
    FIXED,
    STRETCHING, // fills the remaining space
};

struct ColumnData {
    const char* name;
    int width;
    int minimumWidth;
    int textAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    ResizeMode resizeMode;
};

static std::array<ColumnData, static_cast<int>(CueListColumnTypes::_COUNT_)> CueListColumns {{
    {"#", 32, 32, Qt::AlignCenter, ResizeMode::FIXED},
    {"Name", 180, 100},
    {"Description", 300, 100, Qt::AlignVCenter | Qt::AlignLeft, ResizeMode::STRETCHING},
    {"Pre Wait", 85, 70, Qt::AlignCenter},
    {"Duration", 85, 70, Qt::AlignCenter},
    {"Post Wait", 85, 70, Qt::AlignCenter},
    {"", 28, 28, Qt::AlignCenter, ResizeMode::FIXED},
}};


class CueListHeader : public QWidget {
    Q_OBJECT
private:
    std::vector<QWidget*> mWidgets;
    int mGrabbedIndex = -1;
    int mGrabOrigin = 0;
public:
    static constexpr int GRAB_WIDTH = 12;
    explicit CueListHeader(QWidget* parent = nullptr);
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    int getHeaderWidth(int index) const;
signals:
    void userResized();
};

class CueListWidget : public QWidget {
    Q_OBJECT
private:
    float mCursorPos = 0;
    float mTargetCursorPos = 0;
    float mScrollbarPos = 0;
    float mTargetScrollbarPos = 0;

    AnimationHandle* mAnimHandle = nullptr;
    uint8_t mAnimMask = 0;
    static constexpr uint8_t ANIM_CURSOR = (1<<0);
    static constexpr uint8_t ANIM_SCROLL = (1<<1);

    int mStandbyIndex = 0;
    std::vector<bool> mSelectedCues = {};

    const CueListHeader* const header; // only reference, not owned by this
    QScrollBar* const vScrollBar; // only reference, this class handles scrolling to the cue when standby index changes 

    QAction* createKeyboardAction(ShortcutId shortcutId, std::function<void()> callback);
    
    // Playback
    QAction* mPlayAction;
    QAction* mPauseAction;
    QAction* mStopAction;

    // Navigation
    QAction* mHomeAction;
    QAction* mEndAction;
    QAction* mUpAction;
    QAction* mDownAction;

    // Selection
    QAction* mSelectAtCursorAction;
    QAction* mSelectCursorUpAction;
    QAction* mSelectCursorDownAction;
    QAction* mSelectCursorUntilHomeAction;
    QAction* mSelectCursorUntilEndAction;
    QAction* mSelectAllAction;
    QAction* mDeselectAllAction;

    // Editing
    QAction* mDeleteSelectedAction;
    QAction* mCopySelectedAction;
    QAction* mCutSelectedAction;
    QAction* mPasteSelectedAction;
    QAction* mDuplicateSelectedAction;
    QAction* mShiftUpSelectedAction;
    QAction* mShiftDownSelectedAction;

    struct SelectionRange {
        int start;
        int end;
    };
    std::vector<SelectionRange> mSelectionRanges = {};

public:
    static constexpr int GAP_WIDTH = 2;
    static constexpr int ROW_HEIGHT = 28;
    static constexpr int CELL_PADDING = 4;
    static constexpr int ROW_TOTAL_H = ROW_HEIGHT+GAP_WIDTH;
    static constexpr int TOP_OFFSET = GAP_WIDTH+1;

    explicit CueListWidget(CueListHeader* const header, QScrollBar* const scrollBar, QWidget* parent = nullptr);

    void setStandbyIndex(int index);
    int standbyIndex();

    void scrollToStandbyIndex();

    void animationTick(float dt);

    void repaintCue(int index);
    void repaintCueRange(int start, int end);

    void updateSelectionRanges();

    void selectAllCues(bool select=true);
    void selectCueRange(int start, int end, bool select=true);
    void selectCueAtCursor(bool select=true);
    void selectCueAtIndex(int index, bool select=true);

public slots:
    ;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void updateScroll();
};