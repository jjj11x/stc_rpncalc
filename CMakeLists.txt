cmake_minimum_required(VERSION 2.8)
# 3rd party tools
find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11 -fsanitize=address,undefined")

# Directory with source code
add_subdirectory(src)
add_subdirectory(qt_gui)

