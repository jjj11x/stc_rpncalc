cmake_minimum_required(VERSION 3.0)
# 3rd party tools
find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)

# Compiler warnings
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic)
  set(cpp_flags -std=c++11 -fsanitize=address,undefined)
  add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>")
endif()

# Directory with source code
add_subdirectory(src)
add_subdirectory(qt_gui)

