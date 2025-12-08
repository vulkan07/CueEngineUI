#include <iostream>
#include <QApplication>
#include "ui/QTUI.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTUI ui(nullptr);
    ui.start();

    return app.exec();
}