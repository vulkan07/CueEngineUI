#include "ui/CueListModel.h"

// https://doc.qt.io/qt-6/qabstracttablemodel.html#subclassing


CueListModel::CueListModel(QObject* parent) : QAbstractTableModel(parent) {

}

int CueListModel::rowCount(const QModelIndex &parent = QModelIndex()) const {
    Q_UNUSED(parent);
    return backend.getLength();
}

int CueListModel::columnCount(const QModelIndex &parent = QModelIndex()) const {
    Q_UNUSED(parent);
    return 3; // TODO make this not hardcoded
}

QVariant CueListModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const {
    if (!index.isValid() || role != Qt::DisplayRole) return {};

    const Cue* cue = backend.getCue(index.row());

    if (cue) {
        switch (index.column()) {
            case CueListColumns::INDEX: return index.row()+1;
            case CueListColumns::NAME: return cue->mName;
            case CueListColumns::DESCRIPTION: return cue->mDescription;
        }
    }
    return {};
}

QVariant CueListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return "Index";
            case 1: return "Name";
            case 2: return "Note";
        }
    }
    return {};
}

Qt::ItemFlags CueListModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
