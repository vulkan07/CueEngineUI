cmake_minimum_required(VERSION 3.16)
project(CueEngine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# logging
find_package(spdlog REQUIRED)


#set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
# set(CMAKE_CXX_FLAGS_DEBUG
#     "-fsanitize=address -fno-omit-frame-pointer -g")
# set(CMAKE_LINKER_FLAGS_DEBUG
#     "-fsanitize=address")


# Note: Auto MOC doesn't work for some reason,
# so each header file needs to be added here as well
qt_add_executable(CueEngine
#    "resources.qrc"


    "include/ui/UtilWidgets.h"
    "src/ui/UtilWidgets.cpp"

    "include/ui/PanelWidgets.h"
    "src/ui/PanelWidgets.cpp"

    "include/ui/PropertyTabWidgets.h"
    "src/ui/PropertyTabWidgets.cpp"

    "include/ui/QTUI.h"
    "src/ui/QTUI.cpp"

    "include/ui/Theme.h"
    "src/ui/Theme.cpp"

    "include/backend/Cue.h"
    "src/backend/Cue.cpp"

    "include/ui/CueListWidget.h"
    "src/ui/CueListWidget.cpp"

    "include/ui/AnimationClock.h"
    "src/ui/AnimationClock.cpp"

    "include/backend/Backend.h"
    "src/backend/Backend.cpp"


    "src/main.cpp"

)

target_include_directories(CueEngine PRIVATE "include")

target_link_libraries(CueEngine PRIVATE Qt6::Core Qt6::Widgets spdlog::spdlog)


#FILE(GLOB Images assets/images/*) # doesnt work


qt_add_resources(CueEngine "resources"
    FILES
        assets/themes/default.qss
        assets/images/spin-up.png
        assets/images/spin-down.png
        assets/images/spin-up-disabled.png
        assets/images/spin-down-disabled.png
        assets/images/check-on.png
)