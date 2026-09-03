#include <QSettings>

#include "ui/Shortcut.h"

#include <spdlog/spdlog.h>

const ShortcutData* GetShortcutData(ShortcutId shortcutId) {
    for (const ShortcutData& e : SHORTCUT_DATA)
        if (e.shortcutId == shortcutId)
            return &e;

    if (shortcutId != ShortcutId::OTHER)
        spdlog::error("[ui/Shortcut::GetShortcutData()] Shortcut with ID {} was not defined in SHORTCUT_DATA" , static_cast<int>(shortcutId));
    return nullptr;
}

const QString ShortcutManager::SETTINGS_PREFIX = "Shortcuts";

QMap<ShortcutId, QAction*> ShortcutManager::actions = QMap<ShortcutId, QAction*>();

void ShortcutManager::registerAction(ShortcutId shortcutId, QAction* action) {
    if (action && shortcutId != ShortcutId::OTHER) {
        ShortcutManager::actions[shortcutId] = action;
        action->setText( GetShortcutData(shortcutId)->displayText );
    }
}

QAction* const ShortcutManager::getAction(ShortcutId shortcutId) {
    if (!ShortcutManager::actions.contains(shortcutId)) return nullptr;
    return ShortcutManager::actions[shortcutId];
}

// TODO iterator instead maybe
const QMap<ShortcutId, QAction*>& ShortcutManager::getActions() {
    return ShortcutManager::actions;
}

void ShortcutManager::loadShortcutsFromSettings() {
    QSettings settings;
    settings.beginGroup(SETTINGS_PREFIX);

    for (auto i = actions.cbegin(), end = actions.cend(); i != end; ++i) {
        // load
        auto value = settings.value(std::to_string(static_cast<int>(i.key())));

        if (value.isNull()) {
            i.value()->setShortcut( QKeySequence( GetShortcutData(i.key())->defaultShortcut) );
        } else {
            auto stringList = value.toString().split(", ");
            QList<QKeySequence> sequences; 
            
            for (auto sequence : stringList) {
                sequences.push_back(QKeySequence(sequence));
            }

            i.value()->setShortcuts(sequences);
        }
    }
    settings.endGroup();
}

void ShortcutManager::saveShortcutsToSettings() {
    QSettings settings;
    settings.beginGroup(SETTINGS_PREFIX);
    for (auto i = actions.cbegin(), end = actions.cend(); i != end; ++i) {
        QStringList list;
        for (auto s : i.value()->shortcuts()) {
            list.push_back(s.toString());
        }
        settings.setValue(std::to_string(static_cast<int>(i.key())), list);
    }
    settings.endGroup();
}
