#pragma once

#include <QObject>
#include <QMap>

class IconManager : QObject {
    Q_OBJECT
private:
    static const QMap<QString, QString> cueTypeIconPaths;

    IconManager();
public:
    // Singleton //
    static IconManager& getInstance() {
        static IconManager im;
        return im;
    }
    IconManager(IconManager const&) = delete;
    void operator=(IconManager const&) = delete;

    static QString getIconPathForCueType(QString cueType);

};