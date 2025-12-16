#include "ui/CueListModel.h"

// https://doc.qt.io/qt-6/qabstracttablemodel.html#subclassing

#include <QPainter>


CueListModel::CueListModel(QObject* parent) : QAbstractTableModel(parent) {

}

int CueListModel::rowCount(const QModelIndex &parent = QModelIndex()) const {
    Q_UNUSED(parent);
    return backend.getLength();
}

int CueListModel::columnCount(const QModelIndex &parent = QModelIndex()) const {
    Q_UNUSED(parent);
    return COLUMNS.size();
}

QVariant CueListModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const {
    if (!index.isValid() || role != Qt::DisplayRole) return {};

    const Cue* cue = backend.getCue(index.row());

    if (cue) {
        switch (index.column()) {
            case static_cast<int>(CueListColumns::INDEX): return index.row()+1;
            case static_cast<int>(CueListColumns::NAME): return cue->mName;
            case static_cast<int>(CueListColumns::DESCRIPTION): return cue->mDescription;
        }
    }
    return {};
}

QVariant CueListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section < COLUMNS.size()) {
            return COLUMNS[section].name;
        }
    }
    return {};
}

Qt::ItemFlags CueListModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void CueListItemDelegate::paint(QPainter* painter,
        const QStyleOptionViewItem& item,
        const QModelIndex& index) const {
    
    
    painter->save(); // needed to restore the previous state of the painter after this

    const QMargins textPadding = {4,4,4,4};
    const QMargins cellPadding = {1,1,1,1};

    const QRect rect = item.rect;

    bool isSelected = item.state & QStyle::State_Selected;
    

    painter->fillRect(rect, QColor("#202020")); //TODO make ts not hardcoded
    

    // Draw text with padding
    painter->drawText(
        rect-textPadding,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(Qt::DisplayRole).toString());


    painter->restore();
    
}

/*
QSize CueListItemDelegate::sizeHint(const QStyleOptionViewItem& option,
        const QModelIndex& index) const {
    return QSize(COLUMNS[index.column()].width, 30);
}
*/
