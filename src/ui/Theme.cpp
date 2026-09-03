#include "ui/Theme.h"
#include <QFile>
#include <QMap>
#include <QTextStream>
#include <spdlog/spdlog.h>
#include <iostream>

Theme::Theme(const QString& path) {
    QFile f(path);
    
    if (!f.open(QFile::ReadOnly)) {
        spdlog::error(std::string("[ui/Theme::Theme] Cannot read file: ")+path.toStdString());
        return;
    }
    
    QTextStream in(&f);
    QStringList lines;
    int paletteLine = -1;
    int paletteLineEnd = -1;

    int i = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        
        if (line.isEmpty()) continue;

        // Remove texts after //
        int index = line.indexOf("//");
        if (index == 0) continue;
        else if (index > 0) {
            line = line.left(line.length()-index).trimmed();
        }

        if (line == "#PALETTE") {
            if (paletteLine != -1) {
                spdlog::error("[ui/Theme::Theme] #PALETTE appears more than once, when loading " + path.toStdString() );
                paletteLine = -2;
            }
            paletteLine = i;
            continue;
        }
        if (line == "#ENDPALETTE") {
            if (paletteLine < 0) {
                spdlog::error("[ui/Theme::Theme] #ENDPALETTE appears before #PALETTE, when loading " + path.toStdString() );
                paletteLine = -2;
            }
            paletteLineEnd = i;
            continue;
        }

        if (paletteLine >= 0 && paletteLineEnd == -1) {

            int separator = line.indexOf(':');
            
            if (separator > 0) {
                QString name = line.left(separator).trimmed();
                QString value = line.mid(separator+1).trimmed();

                this->mVars[name] = value;
                continue;
            }
        } else
            lines.append(line);
        i++;
    }

    mStylesheet = lines.join("\n");

    for (auto const& c : mVars) {
        mStylesheet.replace("$"+c.first+"$", c.second);
    }

}