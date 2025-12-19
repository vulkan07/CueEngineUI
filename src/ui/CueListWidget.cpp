#include "ui/CueListWidget.h"
#include "backend/Backend.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>

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
        mWidgets.push_back(widget);
        layout->addWidget(widget);
    }
}

int CueListHeader::getHeaderWidth(int index) const {
    return mWidgets[index]->width();
}



CueListWidget::CueListWidget(CueListHeader* const header, QWidget* parent) : QWidget(parent), header(header) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(&AnimationClock::getInstance(), &AnimationClock::tick, this, &CueListWidget::animationTick);

    this->setFixedHeight((backend.getLength()+2) * ROW_TOTAL_H); // TODO this should update
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


    for (int i = 0; i < CueListColumns.size(); i++) {
        
        int width = header->getHeaderWidth(i);
        auto column = CueListColumns[i];
        yBasis = startRow*ROW_TOTAL_H;

        for (int j = startRow; j < std::min(backend.getLength(), (size_t)endRow); j++) {
            
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
                    p.drawText(paddedRect, column.textAlignment, backend.getCue(j)->mName);
                    break;
                case CueListColumnTypes::DESCRIPTION:
                    p.drawText(paddedRect, column.textAlignment, backend.getCue(j)->mDescription);
                    break;
                case CueListColumnTypes::PRE_WAIT:
                    break;
                case CueListColumnTypes::DURATION:
                    p.fillRect(rect.adjusted(0,0, -width*(j*0.01) ,0), QBrush("#604040c0")); // TODO un-hardcode bg color
                    break;
                case CueListColumnTypes::POST_WAIT:
                    break;
                case CueListColumnTypes::NEXT_ACTION:
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
    QRectF rect = {(double)whalf, mCursorPos-whalf , (double)width()-w, (double)ROW_HEIGHT+w};
    p.setPen(QPen(QColor(200,200,255),w));
    p.setBrush(QColor(110,110,255,30));
    p.drawRect(rect);
}


void CueListWidget::mousePressEvent(QMouseEvent* event) {
    this->setStandbyIndex( (event->pos().y() - GAP_WIDTH) / ROW_TOTAL_H );
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
    if (AnimationClock::getInstance().isAnimationsEnabled() == false) {float prevPos = mCursorPos;
        mCursorPos = mTargetCursorPos; 
        this->repaint(
                QRect(0, mCursorPos - GAP_WIDTH, width(), ROW_TOTAL_H+GAP_WIDTH*2) | 
                QRect(0, prevPos - GAP_WIDTH, width(), ROW_TOTAL_H+GAP_WIDTH*2)
            );
    }
    else if (!mAnimHandle) {
        mAnimHandle = AnimationClock::getInstance().resumeAnimation();
    }
}

int CueListWidget::standbyIndex() { return mStandbyIndex; }

void CueListWidget::animationTick(float dt) {
    if (!mAnimHandle) return;

    float oldPos = mCursorPos;
    float dist = fabs(mCursorPos-mTargetCursorPos);
    mCursorPos = lerp(mCursorPos, mTargetCursorPos, decayToLerpConstant(lerpSpeedBoost(60,dist,60), dt));
    
    if (dist < PIXEL_SNAP_THERSHOLD) {
        mCursorPos = mTargetCursorPos;
        mAnimHandle->done();
        mAnimHandle = nullptr; // TODO THIS IS ASS!
    }
    
    this->repaint(
        QRect(0, mCursorPos - GAP_WIDTH, width(), ROW_TOTAL_H+GAP_WIDTH*2) | 
        QRect(0, oldPos - GAP_WIDTH, width(), ROW_TOTAL_H+GAP_WIDTH*2)
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