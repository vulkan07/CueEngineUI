#include "ui/CueListWidget.h"
#include "ui/Utils.h"
#include "ui/QTUI.h"
#include "backend/Backend.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QTime>

#include <algorithm>

CueListHeader::CueListHeader(QWidget* parent) : QWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,11,0); // 11 = scrollbar width
    layout->setSpacing(CueListWidget::GAP_WIDTH);
    this->setLayout(layout);
    this->setFixedHeight(CueListWidget::ROW_HEIGHT+2);

    for (int i = 0; i < CueListColumns.size(); i++) {
    
        auto widget = new QLabel(CueListColumns[i].name, this);
        if (CueListColumns[i].ResizeMode != ResizeMode::STRETCHING) {
            widget->setFixedWidth(CueListColumns[i].width);
        }
        widget->setAutoFillBackground(true);
        widget->setAlignment(Qt::AlignCenter);
        widget->setMouseTracking(true);
        mWidgets.push_back(widget);
        layout->addWidget(widget);
    }

    this->setMouseTracking(true);
}

void CueListHeader::mousePressEvent(QMouseEvent* event) {
    int mouseX = event->pos().x();

    int x = 0;
    for (int i = 0; i < CueListColumns.size(); i++) {
        if ((mouseX > x-CueListHeader::GRAB_WIDTH) && (mouseX < x+CueListHeader::GRAB_WIDTH)) {
            mGrabbedIndex = i;
            mGrabOrigin = x;
            break;
        }
        x += mWidgets[i]->width() + CueListWidget::GAP_WIDTH;
    }
    QGuiApplication::setOverrideCursor(Qt::SizeHorCursor); // TODO 
    QGuiApplication::restoreOverrideCursor();
}

void CueListHeader::mouseReleaseEvent(QMouseEvent* event) {
    mGrabbedIndex = -1;
}

void CueListHeader::mouseMoveEvent(QMouseEvent* event) {
    if (mGrabbedIndex < 1) 
        return;

    if (CueListColumns[mGrabbedIndex-1].ResizeMode == ResizeMode::FIXED)
        return;

    int mouseX = event->pos().x();

    int dx = mouseX - mGrabOrigin;
    QWidget* w1 = mWidgets[mGrabbedIndex-1];
    QWidget* w2 = mWidgets[mGrabbedIndex];

    int size1 = w1->width()+dx;
    int size2 = w2->width()-dx;
    int min1 = CueListColumns[mGrabbedIndex-1].minimumWidth;
    int min2 = CueListColumns[mGrabbedIndex].minimumWidth;

    
    // enforce minimum widths
    if (size1 < min1) {
        int d = size1-min1;
        dx -= d;
        size2 += d;
        size1 = min1;
    }
    if (size2 < min2) {
        int d = size2-min2;
        dx -= d;
        size1 += d;
        size2 = min2;
    }


    if (CueListColumns[mGrabbedIndex-1].ResizeMode != ResizeMode::STRETCHING)
        w1->setFixedWidth(size1);
    if (CueListColumns[mGrabbedIndex].ResizeMode != ResizeMode::STRETCHING)
        w2->setFixedWidth(size2);

    mGrabOrigin += dx;
    emit userResized(); // send update to cue list table
}

int CueListHeader::getHeaderWidth(int index) const {
    return mWidgets[index]->width();
}

QAction* CueListWidget::createKeyboardShortcut(const char* name, QKeySequence shortcut, std::function<void()> callback) {
    QAction* a = new QAction(this);
    a->setShortcut(shortcut);
    a->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(a, &QAction::triggered, this, callback);
    this->addAction(a);
    ShortcutManager::registerAction(name, a);
    return a;
}

CueListWidget::CueListWidget(CueListHeader* const header, QScrollBar* const scrollBar, QWidget* parent) 
    : QWidget(parent), header(header), vScrollBar(scrollBar), mAnimHandle(new AnimationHandle) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setFocusPolicy(Qt::StrongFocus);

    // Keyboard shortcuts specific to this widget
    mHomeAction = createKeyboardShortcut("Move to top", Qt::Key_Home, [=]{
        this->setStandbyIndex(0);
    });
    mEndAction = createKeyboardShortcut("Move to bottom", Qt::Key_End, [=]{
        this->setStandbyIndex(backend.getLength()-1);
    });
    mUpAction = createKeyboardShortcut("Move up", Qt::Key_Up, [=]{
        this->setStandbyIndex(this->standbyIndex()-1);
    });
    mDownAction = createKeyboardShortcut("Move down", Qt::Key_Down, [=]{
        this->setStandbyIndex(this->standbyIndex()+1);
    });
    mPlayAction = createKeyboardShortcut("Play cue", Qt::Key_Space, [=]{
        // TODO cue action here
        this->setStandbyIndex(this->standbyIndex()+1);
    });
    mSelectAtCursorAction = createKeyboardShortcut("Select current cue", Qt::Key_Left, [=]{
        this->selectCueAtCursor(!mSelectedCues[this->standbyIndex()]);
    });
    mSelectAllAction = createKeyboardShortcut("Select all cues", Qt::Key_Control | Qt::Key_A, [=]{
        this->selectAllCues(true);
    });
    mDeselectAllAction = createKeyboardShortcut("Deselect all cues", Qt::Key_Escape, [=]{
        this->selectAllCues(false);
    });
    mSelectCursorUpAction = createKeyboardShortcut("Select up", Qt::Key_Shift | Qt::Key_Up, [=]{
        int index = this->standbyIndex();
        this->selectCuesInRange(index-1, index, true);
        this->setStandbyIndex(index-1);
    });
    mSelectCursorDownAction = createKeyboardShortcut("Select down", Qt::Key_Shift | Qt::Key_Down, [=]{
        int index = this->standbyIndex();
        this->selectCuesInRange(index, index+1, true);
        this->setStandbyIndex(index+1);
    });
    mSelectCursorUntilHomeAction = createKeyboardShortcut("Select until home", Qt::Key_Shift | Qt::Key_Home, [=]{
        int index = this->standbyIndex();
        this->selectCuesInRange(0, index, true);
        this->setStandbyIndex(0);
    });
    mSelectCursorUntilEndAction = createKeyboardShortcut("Select until end", Qt::Key_Shift | Qt::Key_End, [=]{
        this->selectCuesInRange(this->standbyIndex(), backend.getLength()-1, true);
        this->setStandbyIndex(backend.getLength()-1);
    });

    // Optimizations for high FPS rendering
    // Skip clearing the widget before repaints (in theory)
    this->setAttribute(Qt::WA_OpaquePaintEvent); 
    this->setAttribute(Qt::WA_StaticContents); 
    this->setAutoFillBackground(false);

    connect(&AnimationClock::getInstance(), &AnimationClock::tick, this, &CueListWidget::animationTick);
    connect(this->header, &CueListHeader::userResized, this, [=]{this->update();}); // Update widths if user resizes headers

    //// TODO implement an update callback function when cues change in backend  -->
    this->setFixedHeight((backend.getLength()+2) * ROW_TOTAL_H + TOP_OFFSET); 
    this->mSelectedCues.resize(backend.getLength(), false);
    ////
}

// NOTE: performance could be (in theory) significantly increased further by using QStaticText instead of QPainter::drawText()
// but that requires caching every text entry and updating them when any cue property or the cue list changes
void CueListWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setClipRegion(event->region());

    // Don't render rows that are outside viewport (full repaint) 
    // or dirty region (partial repaint when cursor is moved)
    int startRow = event->region().boundingRect().top() / ROW_TOTAL_H;
    int endRow = event->region().boundingRect().bottom() / ROW_TOTAL_H +1;
    
    //top-left point of each cell
    int xBasis = 0;
    int yBasis;

    static QFont monoFont("monospace");

    Cue* cue = nullptr;

    for (int i = 0; i < CueListColumns.size(); i++) {
        
        int width = header->getHeaderWidth(i);
        auto column = CueListColumns[i];
        yBasis = startRow*ROW_TOTAL_H + TOP_OFFSET;

        for (int j = startRow; j < std::min(backend.getLength(), (size_t)endRow); j++) {
            
            cue = backend.getCue(j);
            if (!cue) continue; // Just in case

            // Background fill
            QRect rect {xBasis, yBasis, width, ROW_HEIGHT}; // the whole cell
            p.fillRect(rect, QBrush(j%2 ? "#1d1d1f" : "#222224")); // TODO un-hardcode bg color

            //if (mSelectedCues[j])
            //    p.fillRect(rect, QBrush(QColor(100,150,200,130))); // TODO un-hardcode color

            // Text
            QRect paddedRect = rect.marginsRemoved(QMargins()+CELL_PADDING); // TODO un-hardcode text padding
            switch (static_cast<CueListColumnTypes>(i)) {
                case CueListColumnTypes::INDEX:
                    p.drawText(paddedRect, column.textAlignment, QString::number(j+1));
                    break;
                case CueListColumnTypes::NAME:
                    p.drawText(paddedRect, column.textAlignment, cue->mName);
                    break;
                case CueListColumnTypes::DESCRIPTION:
                    p.drawText(paddedRect, column.textAlignment, cue->mDescription);
                    break;
                case CueListColumnTypes::PRE_WAIT:
                    break;
                case CueListColumnTypes::DURATION:
                    if (cue->getType() == CueType::MediaCue) {
                        p.setFont(monoFont);
                        p.drawText(paddedRect, column.textAlignment, stringFromDuration(static_cast<MediaCue*>(cue)->mDuration));
                        //p.fillRect(rect.adjusted(0,0, -width*(j*0.01) ,0), QBrush("#604040c0")); // TODO un-hardcode bg color    
                    }
                    break;
                case CueListColumnTypes::POST_WAIT:
                    p.setFont(monoFont);
                    break;
                case CueListColumnTypes::NEXT_ACTION:
                    p.setFont(monoFont);
                    break;
                default:; // suppress warning for _COUNT_
            }
            yBasis += ROW_TOTAL_H;
        }
        xBasis += width + GAP_WIDTH;
    }

    // "Cursor" at standby index
    int w = 2;
    double whalf = w/2;
    QRectF rect = {whalf, mCursorPos-whalf+TOP_OFFSET, (double)width()-w, (double)ROW_HEIGHT+whalf};
    QColor pen{180,180,255};
    p.setPen(QPen(pen,w));
    p.setBrush(QColor(100,100,255,30));
    p.drawRect(rect);

    // Selection ranges 
    w = 1;
    p.setRenderHint(QPainter::Antialiasing, false); // sharper edges for selection rects
    p.setPen(QPen(QColor({220,220,250}),w));
    p.setBrush(QColor(250,250,255,25));
    for (auto range : mSelectionRanges) {
        p.drawRect(
                1,
                range.start*ROW_TOTAL_H+TOP_OFFSET-w,
                width()-(w*2),
                (range.end-range.start+1)*ROW_TOTAL_H-(w*2)
        );
    }
    p.setRenderHint(QPainter::Antialiasing, true);
    
    // Mouse cursor arrow thing
    QPainterPath path;
    qreal top = mCursorPos+TOP_OFFSET;
    QPointF p1{2, top+1};
    QPointF p2{6, top+ROW_HEIGHT/2};
    QPointF p3{2, top+ROW_HEIGHT-1};
    path.moveTo(p1);
    path.lineTo(p2);
    path.moveTo(p2);
    path.lineTo(p3);
    path.lineTo(p3);
    path.lineTo(p1);
    p.setPen(Qt::NoPen);
    p.fillPath(path, pen);

    // Tiny 'shadow' below header
    // only shows up when it cuts throught text, improve readability
    int barPos = vScrollBar->value();
    constexpr int size = 5;
    if ((barPos+2) % ROW_TOTAL_H > 3) {
        for (int i = 0; i < size; i++) {
            p.setPen({22,22,22,(190/size)*(size-i)});
            p.drawLine(0, i+barPos, width(), i+barPos);
        }
    }
    
}


void CueListWidget::mousePressEvent(QMouseEvent* event) {
    this->setStandbyIndex( (event->pos().y() - GAP_WIDTH - TOP_OFFSET) / ROW_TOTAL_H );
}

//TODO remake this in a less ass way
void CueListWidget::setStandbyIndex(int index) {
    int oldIndex = mStandbyIndex;
    mStandbyIndex = index;
    if (mStandbyIndex >= backend.getLength() || mStandbyIndex < 0) {
        mStandbyIndex = oldIndex;
        return;
    }

    mTargetCursorPos = mStandbyIndex*ROW_TOTAL_H;
    if (AnimationClock::getInstance().isAnimationsEnabled() == false) {
        float prevPos = mCursorPos;
        mCursorPos = mTargetCursorPos; 
        this->repaint(
                QRect(0, mCursorPos - GAP_WIDTH + TOP_OFFSET, width(), ROW_TOTAL_H+GAP_WIDTH*2) | 
                QRect(0, prevPos - GAP_WIDTH + TOP_OFFSET, width(), ROW_TOTAL_H+GAP_WIDTH*2)
            );
    }
    else {
        mAnimMask |= ANIM_CURSOR;
        mAnimHandle->start();
    }
    this->scrollToStandbyIndex();
}

int CueListWidget::standbyIndex() {
    return mStandbyIndex;
}


void CueListWidget::scrollToStandbyIndex() {
    constexpr int PADDING = 1*ROW_TOTAL_H;

    int i = this->standbyIndex();
    int cueY = i*ROW_TOTAL_H;
    int barPos = vScrollBar->value();
    int h = this->visibleRegion().boundingRect().height();

    int target = -1;
    if (cueY < barPos+PADDING) { // cue is above viewport
        target = cueY-PADDING;
        if (target<0) target = 0;
    } else if (cueY+ROW_HEIGHT > barPos + h - PADDING) { // cue below viewport -> adjust to bottom
        target = cueY+ROW_HEIGHT+PADDING-h+6; // Adjust last visible cues bottom (not idea why 6px)
    }

    if (target == -1) return; // no position update

    if (AnimationClock::getInstance().isAnimationsEnabled() == false) {
        vScrollBar->setValue(target);
    } else {
        mAnimMask |= ANIM_SCROLL;
        mTargetScrollbarPos = target;
        mAnimHandle->start();
    }
}

void CueListWidget::animationTick(float dt) {
    if (!mAnimHandle->isRunning()) return;

    int barPos = vScrollBar->value();
    // Qt uses an int for the scrollbar, but animation needs float for accurate and smooth motion
    // This ensures that the float value updates along together with Qt when the user scrolls the scrollbar
    if (fabs(barPos - mScrollbarPos) >= 1)
        mScrollbarPos = barPos;

    if (mAnimMask & ANIM_SCROLL) {
        int dist = fabs(barPos-mTargetScrollbarPos);

        if(dist <= 1) {
            mAnimMask &= ~(ANIM_SCROLL); // end scroll bar animation
        } else {
            int speed = dist < 6 ? 30 : 44;
            mScrollbarPos = lerp(mScrollbarPos, mTargetScrollbarPos, decayToLerpConstant(speed, dt));
            vScrollBar->setValue((int)mScrollbarPos);
        }
    } 

    if (mAnimMask & ANIM_CURSOR) {
        float oldPos = mCursorPos;
        float dist = fabs(mCursorPos-mTargetCursorPos);
        float speed = 48;
        if (dist > ROW_TOTAL_H*3) speed = 90;
        mCursorPos = lerp(mCursorPos, mTargetCursorPos, decayToLerpConstant(speed, dt));
        
        if (dist < PIXEL_SNAP_THERSHOLD) {
            mCursorPos = mTargetCursorPos;
            mAnimMask &= ~(ANIM_CURSOR); // end cursor animation
        }
        
        // Repaint the area around cursor only
        // Note: scroll animation triggers a total repaint so this is won't be called redundantly
        if (!(mAnimMask & ANIM_SCROLL)) {
            this->repaint(
                QRect(0, mCursorPos - GAP_WIDTH + TOP_OFFSET, width(), ROW_TOTAL_H+GAP_WIDTH*2) | 
                QRect(0, oldPos - GAP_WIDTH + TOP_OFFSET, width(), ROW_TOTAL_H+GAP_WIDTH*2)
            );
        }
    }

    if (!mAnimMask)
        mAnimHandle->stop();
}

void CueListWidget::selectCueAtCursor(bool selected) {
    if (mSelectedCues[mStandbyIndex] == selected) return;
    mSelectedCues[mStandbyIndex] = selected;
    this->updateSelectionRanges();
    this->repaintCue(mStandbyIndex);
}

void CueListWidget::selectAllCues(bool select) {
    std::fill(mSelectedCues.begin(), mSelectedCues.end(), select);
    this->updateSelectionRanges();
    this->repaint();
}

void CueListWidget::selectCuesInRange(int start, int end, bool select) {
    if (start < 0) start = 0;
    if (end >= backend.getLength()) end = backend.getLength() - 1;
    if (start > end) return;

    std::fill(mSelectedCues.begin() + start, mSelectedCues.begin() + end + 1, select);
    this->updateSelectionRanges();
    this->repaint(QRect(
        0, 
        start*ROW_TOTAL_H + TOP_OFFSET - GAP_WIDTH-1, 
        width(),
        (end - start + 1) * ROW_TOTAL_H + 2 * GAP_WIDTH + 2)
    );
}

void CueListWidget::repaintCue(int index) {
    if (index < 0 || index >= backend.getLength()) return;
    this->repaint(QRect(0, index*ROW_TOTAL_H + TOP_OFFSET - GAP_WIDTH-1, width(), ROW_TOTAL_H+1));
}

// Split the selected cues into continuous ranges for rendering selection highlights
void CueListWidget::updateSelectionRanges() {
    int start = -1;
    mSelectionRanges.clear();
    //qDebug("---------------------");
    if (mSelectedCues[0]) start = 0;

    for (int i = 0; i < mSelectedCues.size()-1; i++) {
        if (!mSelectedCues[i] && mSelectedCues[i+1]) {
            start = i+1;
        } else if (mSelectedCues[i] && !mSelectedCues[i+1]) {
            if (start != -1) {
                //qDebug() << ">" << start << "-" << i;
                mSelectionRanges.push_back({start, i});
                start = -1;
            }   
        }
    }
    if (start != -1 && mSelectedCues.back())
        mSelectionRanges.push_back({start, (int)mSelectedCues.size()-1});
}