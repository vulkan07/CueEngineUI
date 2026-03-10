#include "ui/CueListWidget.h"
#include "ui/Utils.h"
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


CueListWidget::CueListWidget(CueListHeader* const header, QScrollBar* const scrollBar, QWidget* parent) 
    : QWidget(parent), header(header), vScrollBar(scrollBar), mAnimHandle(new AnimationHandle) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(&AnimationClock::getInstance(), &AnimationClock::tick, this, &CueListWidget::animationTick);
    connect(this->header, &CueListHeader::userResized, this, [=]{this->update();}); // Update widths if user resizes headers

    this->setFixedHeight((backend.getLength()+2) * ROW_TOTAL_H + TOP_OFFSET); // TODO this should update if num of cues changes
}

void CueListWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setClipRegion(event->region());

    // Don't render rows that are outside viewport
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

    // Mouse "cursor"
    int w = 2;
    double whalf = w/2;
    QRectF rect = {(double)whalf, mCursorPos-whalf+TOP_OFFSET, (double)width()-w, (double)ROW_HEIGHT+w};
    QColor pen{200,200,255};
    p.setPen(QPen(pen,w));
    p.setBrush(QColor(110,110,255,30));
    p.drawRect(rect);
    
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

    /* TODO shadow below header (needs pos of scrollbar)
    for (int i = 0; i < 8; i++) {
        p.setPen({0,0,0,255*(255/8)});
        p.drawLine(0,i,width(),i);
    }
    */
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
        mAnimHandle->start();
    }
    this->scrollToStandbyIndex();
}

int CueListWidget::standbyIndex() { return mStandbyIndex; }


void CueListWidget::scrollToStandbyIndex() {
    static const int PADDING = 2*ROW_TOTAL_H;

    int i = this->standbyIndex();
    int cueY = i*ROW_TOTAL_H;
    int barPos = vScrollBar->value();
    int h = this->visibleRegion().boundingRect().height();

    int target = -1;
    if (cueY < barPos+PADDING) { // cue is above viewport
        qDebug() << "above";
        target = cueY-PADDING;
        if (target<0) target = 0;
    } else if (cueY+ROW_HEIGHT > barPos + h - PADDING) { // cue below viewport -> adjust to bottom
        qDebug() << "below";
        target = cueY+ROW_HEIGHT+PADDING-h;
    }

    if (target == -1) return; // no position update

    if (AnimationClock::getInstance().isAnimationsEnabled() == false) {
        vScrollBar->setValue(target);
    } else {
        mAnimHandle->start();
        mTargetScrollbarPos = target;
    }
}

void CueListWidget::animationTick(float dt) {
    if (!mAnimHandle->isRunning()) return;

    int barPos = vScrollBar->value();

    if ( mTargetScrollbarPos != -1) { // TODO this is stupid
        if(fabs(barPos-mTargetScrollbarPos) > PIXEL_SNAP_THERSHOLD) {
            vScrollBar->setValue(
                lerp(barPos, mTargetScrollbarPos, decayToLerpConstant(44, dt))
            );
        }
        else {
            mTargetScrollbarPos = -1;
        }
        
    } 

    float oldPos = mCursorPos;
    float dist = fabs(mCursorPos-mTargetCursorPos);
    float speed = 48;
    if (dist > ROW_TOTAL_H*3) speed = 90;
    mCursorPos = lerp(mCursorPos, mTargetCursorPos, decayToLerpConstant(speed, dt));
    
    if (dist < PIXEL_SNAP_THERSHOLD) {
        mCursorPos = mTargetCursorPos;
        mAnimHandle->stop();
    }
    
    this->repaint(
        QRect(0, mCursorPos - GAP_WIDTH + TOP_OFFSET, width(), ROW_TOTAL_H+GAP_WIDTH*2) | 
        QRect(0, oldPos - GAP_WIDTH + TOP_OFFSET, width(), ROW_TOTAL_H+GAP_WIDTH*2)
    );
}

void CueListWidget::onUpAction() {
    setStandbyIndex(standbyIndex()-1);
}

void CueListWidget::onDownAction() {
    setStandbyIndex(standbyIndex()+1);
}// azt nem tudom hogy a templomban az orgona az rendelkezik e python compilerrrel, mert klaviatura van rajta tehat gepelni lehet vele - Taki 2025

void CueListWidget::onPlayAction() {
    qDebug() << "PLAY";
}