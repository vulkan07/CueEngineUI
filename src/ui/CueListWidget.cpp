#include "ui/CueListWidget.h"
#include "backend/Backend.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>

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
    this->setMouseTracking(true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    this->setFixedHeight((backend.getLength()+2) * (ROW_HEIGHT+GAP_WIDTH)); // TODO this should update
}

void CueListWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setClipRegion(event->region());

    //top-left point of each cell
    int xBasis = 0;
    int yBasis;

    for (int i = 0; i < CueListColumns.size(); i++) {
        
        int width = header->getHeaderWidth(i);
        auto column = CueListColumns[i];
        yBasis = 0;

        for (int j = 0; j < backend.getLength(); j++) {
            
            // skip if outside viewport
            if (yBasis+ROW_HEIGHT < event->region().boundingRect().top()) { // above
                yBasis += ROW_HEIGHT + GAP_WIDTH;
                continue;
            }
            if (yBasis > event->region().boundingRect().bottom()) // below
                break;
            

            QRect rect {xBasis, yBasis, width, ROW_HEIGHT};
            
            p.fillRect(rect, QBrush(j%2 ? "#1d1d1f" : "#222224")); // TODO un-hardcode bg color

            rect = rect.marginsRemoved(QMargins()+CELL_PADDING); // TODO un-hardcode text padding

            switch (static_cast<CueListColumnTypes>(i)) {
                case CueListColumnTypes::INDEX:
                    p.drawText(rect, column.textAlignment, QString::number(j+1));
                    break;
                case CueListColumnTypes::NAME:
                    p.drawText(rect, column.textAlignment, backend.getCue(j)->mName);
                    break;
                case CueListColumnTypes::DESCRIPTION:
                    p.drawText(rect, column.textAlignment, backend.getCue(j)->mDescription);
                    break;
                case CueListColumnTypes::PRE_WAIT:
                    break;
                case CueListColumnTypes::DURATION:
                    break;
                case CueListColumnTypes::POST_WAIT:
                    break;
                case CueListColumnTypes::NEXT_ACTION:
                    break;
                default:; // suppress warning for _COUNT_
            }
            yBasis += ROW_HEIGHT + GAP_WIDTH;
        }
        xBasis += width + GAP_WIDTH;
    }
}