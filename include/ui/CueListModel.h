#pragma once

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QHeaderView>

#include <backend/Backend.h>

#include <array>


enum class CueListColumns : int {
    INDEX,
    NAME,
    DESCRIPTION,
    PRE_WAIT,
    DURATION,
    POST_WAIT,
    NEXT_ACTION,
    _COUNT_
};
struct ColumnData {
    const char* name;
    int width;
    QHeaderView::ResizeMode resizeMode = QHeaderView::Interactive;
};
static constexpr std::array<ColumnData, static_cast<int>(CueListColumns::_COUNT_)> COLUMNS {{
    {"#", 30, QHeaderView::Fixed},
    {"Name", 150},
    {"Description", 800, QHeaderView::Stretch},
    {"Pre Wait", 100},
    {"Duration", 150},
    {"Post Wait", 100},
    {"", 30},
}};

class CueListModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CueListModel(QObject* parent);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
};

class CueListItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;

    /*
    QSize sizeHint(const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;
    */
};
