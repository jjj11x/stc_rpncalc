cmake_minimum_required(VERSION 3.1)
project(stc_rpncalc C CXX)
# 3rd party tools
find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (for tests debug make sense)")

# Compiler warnings
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

# Directory with source code
add_subdirectory(src)
add_subdirectory(qt_gui)

