#include "ui/CoreWidgets.h"
#include "ui/Theme.h"

#include <QLabel>

#include <QFrame>
#include <QWidget>
#include <QVBoxLayout>
#include <QApplication>
#include <QString>

class QTUI : public QFrame {
    Q_OBJECT
public:
    explicit QTUI(QWidget* parent);
    void start();
    void applyTheme(QString path);
};