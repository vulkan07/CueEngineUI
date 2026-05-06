#pragma once
#include <QObject>
#include <QString>
#include <QAction>

// To add a new shortcut which will appear in the settings,
//  1. add an enum value for it in ShortcutId
//  2. create shortcut data for it in SHORTCUT_DATA
// (3. create a QAction with the widget that uses it, and register it via ShortcutManager::registerAction() )


enum class ShortcutId {
    OTHER = 0, // Not specified, won't occur in shortcut editor

    APP_EXIT,
    APP_NEW_SESSION,
    APP_OPEN_SESSION,
    APP_SAVE_SESSION,
    APP_OPEN_SETTINGS,
    APP_TOGGLE_SECONDARY_WINDOW,

    CUELIST_MOVE_UP,
    CUELIST_MOVE_DOWN,
    CUELIST_MOVE_HOME,
    CUELIST_MOVE_END,
    CUELIST_SELECT_CURRENT,
    CUELIST_SELECT_ALL,
    CUELIST_SELECT_UP,
    CUELIST_SELECT_DOWN,
    CUELIST_SELECT_HOME,
    CUELIST_SELECT_END,
    CUELIST_DESELECT_ALL,

    CUELIST_DELETE_SELECTED,
    CUELIST_COPY_SELECTED,
    CUELIST_CUT_SELECTED,
    CUELIST_DUPLICATE_SELECTED,
    CUELIST_SHIFT_UP_SELECTED,
    CUELIST_SHIFT_DOWN_SELECTED,

    CUELIST_PLAY_CURRENT_CUE,
    CUELIST_PAUSE_CURRENT_CUE,
    CUELIST_STOP_CURRENT_CUE,
};

enum class ShortcutCategory {
    NONE = 0,
    APPLICATION,
    NAVIGATION,
    SELECTION,
    EDITING,
    PLAYBACK,
    _COUNT_
};
static constexpr const char* ShortcutCategoryNames[] {
    "Misc",
    "Application",
    "Navigation",
    "Selection",
    "Editing",
    "Playback",
};

struct ShortcutData {
    const ShortcutId shortcutId; 
    const ShortcutCategory shortcutCategory; 
    const char* displayText;
    const char* defaultShortcut;
};

static constexpr ShortcutData SHORTCUT_DATA[] = {
    { ShortcutId::APP_EXIT, ShortcutCategory::APPLICATION, "Exit app", "Ctrl+Q" },
    { ShortcutId::APP_NEW_SESSION, ShortcutCategory::APPLICATION, "New session", "Ctrl+N" },
    { ShortcutId::APP_OPEN_SESSION, ShortcutCategory::APPLICATION, "Open session", "Ctrl+O" },
    { ShortcutId::APP_SAVE_SESSION, ShortcutCategory::APPLICATION, "Save session", "Ctrl+S" },
    { ShortcutId::APP_OPEN_SETTINGS, ShortcutCategory::APPLICATION, "Open settings", "Alt+S" },
    { ShortcutId::APP_TOGGLE_SECONDARY_WINDOW, ShortcutCategory::APPLICATION, "Toggle secondary window", "Alt+W" },

    { ShortcutId::CUELIST_MOVE_UP, ShortcutCategory::NAVIGATION, "Move up", "Up" },
    { ShortcutId::CUELIST_MOVE_DOWN, ShortcutCategory::NAVIGATION, "Move down", "Down" },
    { ShortcutId::CUELIST_MOVE_HOME, ShortcutCategory::NAVIGATION, "Move to home", "Home" },
    { ShortcutId::CUELIST_MOVE_END, ShortcutCategory::NAVIGATION, "Move to end", "End" },

    { ShortcutId::CUELIST_SELECT_CURRENT, ShortcutCategory::SELECTION, "Select current", "Left" },
    { ShortcutId::CUELIST_SELECT_ALL, ShortcutCategory::SELECTION, "Select all", "Ctrl+A" },
    { ShortcutId::CUELIST_SELECT_UP, ShortcutCategory::SELECTION, "Select up", "Shift+Up" },
    { ShortcutId::CUELIST_SELECT_DOWN, ShortcutCategory::SELECTION, "Select down", "Shift+Down" },
    { ShortcutId::CUELIST_SELECT_HOME, ShortcutCategory::SELECTION, "Select until home", "Shift+Home" },
    { ShortcutId::CUELIST_SELECT_END, ShortcutCategory::SELECTION, "Select until end", "Shift+End" },
    { ShortcutId::CUELIST_DESELECT_ALL, ShortcutCategory::SELECTION, "Deselect all", "Esc" },

    { ShortcutId::CUELIST_DELETE_SELECTED, ShortcutCategory::EDITING, "Delete selected cues", "Del" },
    { ShortcutId::CUELIST_COPY_SELECTED, ShortcutCategory::EDITING, "Copy selected cues", "Ctrl+C" },
    { ShortcutId::CUELIST_CUT_SELECTED, ShortcutCategory::EDITING, "Cut selected cues", "Ctrl+X" },
    { ShortcutId::CUELIST_DUPLICATE_SELECTED, ShortcutCategory::EDITING, "Duplicate selected cues", "Ctrl+D" },
    { ShortcutId::CUELIST_SHIFT_UP_SELECTED, ShortcutCategory::EDITING, "Shift selected cues up", "Alt+Up" },
    { ShortcutId::CUELIST_SHIFT_DOWN_SELECTED, ShortcutCategory::EDITING, "Shift selected cues down", "Alt+Down" },

    { ShortcutId::CUELIST_PLAY_CURRENT_CUE, ShortcutCategory::PLAYBACK, "Play cue", "Space" },
    { ShortcutId::CUELIST_PAUSE_CURRENT_CUE, ShortcutCategory::PLAYBACK, "Pause cue", "P" },
    { ShortcutId::CUELIST_STOP_CURRENT_CUE, ShortcutCategory::PLAYBACK, "Stop cue", "X" },
};

const ShortcutData* GetShortcutData(ShortcutId shortcutId);

// Register QActions here that should appear in the settings' shortcuts page
class ShortcutManager : public QObject {
    Q_OBJECT
private:
    static QMap<ShortcutId, QAction*> actions;
public:
    const static QString SETTINGS_PREFIX;
    static void registerAction(ShortcutId shortcutId, QAction* action);

    static QAction* const getAction(ShortcutId shortcutId);
    static const QMap<ShortcutId, QAction*>& getActions();

    static void loadShortcutsFromSettings(); // Call only after all QActions are created that should be loaded
    static void saveShortcutsToSettings();
};