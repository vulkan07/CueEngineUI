#include "ui/Theme.h"
#include <QFile>
#include <QMap>
#include <QRegularExpression>
#include <iostream>
#include <spdlog/spdlog.h>

QString loadTheme(const QString& path) {
    QFile f(path);
    
    if (!f.open(QFile::ReadOnly)) {
        spdlog::error(std::string("(ui/Theme::loadTheme) Cannot read file: ")+path.toStdString());
        return {};
    }
    
    QTextStream in(&f);
    QStringList lines;
    QMap<QString,QString> vars;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        if (line.isEmpty()) continue;

        if (line.startsWith('$')) {
            int index = line.indexOf('=');
            
            if (index > 0) {
                QString name = line.left(index).trimmed();
                QString value = line.mid(index+1).trimmed();

                vars[name]=value;
                continue;
            }
        }
        lines << line;
    }

    if (lines.length() < 1)
        lines << "";

    QString s = lines.join("\n");

    for (auto it = vars.begin(); it != vars.end(); ++it) {
        s.replace(it.key(), it.value());
    }

    return s;
}