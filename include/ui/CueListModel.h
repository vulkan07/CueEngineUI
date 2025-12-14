#pragma once

#include <QAbstractTableModel>
#include <QModelIndex>
#include <backend/Backend.h>


enum CueListColumns {
    INDEX,
    NAME,
    DESCRIPTION,
};

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