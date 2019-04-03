cmake_minimum_required(VERSION 2.8)
# 3rd party tools
find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)
# Directory with source code
add_subdirectory(src)
add_subdirectory(qt_gui)

