#pragma once

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QHeaderView>

#include <backend/Backend.h>


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
