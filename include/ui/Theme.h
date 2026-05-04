#pragma once
#include <QString>
#include <QObject>
#include <QColor>

#include <vector>
#include <map>

// A new Theme parses a .qss file and stores the variables from the #PALETTE region,
// and substitutes them in the rest of the style sheet, for easier color management,
// plus the code can retrieve these variables for any need
class Theme {
private:
    std::map<QString, QString> mVars; // colors, border sizes etc..

    friend class ThemeManager;
public:
    QString mStylesheet;
    Theme(const QString& path);
};

// TODO this to swap themes and change colors live (maybe)
/*
class ThemeManager : QObject {
    Q_OBJECT
private:
    ThemeManager() : QObject() {}
    std::vector<std::unique_ptr<Theme>> mThemes;

public:
    // singleton //
    static ThemeManager& getInstance() {
        static ThemeManager themeManager;
        return themeManager;
    }
    // --------- //

    Theme* loadTheme(const QString& path);
    Theme* getCurrentTheme();
    void setTheme(Theme* theme);
    void updateGUIFromTheme();
};
*/