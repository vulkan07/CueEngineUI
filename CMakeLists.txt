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




# Note: Auto MOC doesn't work for some reason,
# so each header file needs to be added here as well
qt_add_executable(CueEngine
#    "resources.qrc"


    "include/ui/UtilWidgets.h"
    "src/ui/UtilWidgets.cpp"

    "include/ui/PanelWidgets.h"
    "src/ui/PanelWidgets.cpp"

    "include/ui/QTUI.h"
    "src/ui/QTUI.cpp"

    "include/ui/Theme.h"
    "src/ui/Theme.cpp"


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