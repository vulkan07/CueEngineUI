#include "ui/IconManager.h"

#include <spdlog/spdlog.h>

const QMap<QString, QString> IconManager::cueTypeIconPaths = {
    {"audio","Audio"},
    {"video","Video"},
    {"image","Image"},
    {"text","Text2"},
    {"command","Command"},
    {"index","Go To"},
    {"group","Group"},
    {"stopall","Stop"},
    {"midi","MIDI"},
    {"osc","OSC"},
    {"light","Light"},
    {"timer","Clock"},
};

IconManager::IconManager() : QObject() {

}

// Resolves: ":/assets/icons/[???].svg"
QString IconManager::getIconPathForCueType(QString cueType) {
    if (!cueTypeIconPaths.contains(cueType)) {
        spdlog::warn(("[IconManager] No icon assigned in code for cue type: " + cueType).toStdString());
        return "";
    }
    return ":/assets/icons/"+cueTypeIconPaths[cueType]+".svg";
}