#include <iostream>
#include <QApplication>
#include "ui/QTUI.h"

#include <backend/Backend.h>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv); // QT init
    QTUI ui(nullptr); // UI Splash screen

    BBackend::Backend::getInstance(); // Singleton invokes backend init

    ui.start(); // actual UI

    return app.exec();
}