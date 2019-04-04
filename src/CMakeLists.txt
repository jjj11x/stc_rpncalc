include_directories(${Qt5Widgets_INCLUDE_DIRS} ${QtQml_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS} ${QtQml_DEFINITIONS} ${${Qt5Quick_DEFINITIONS}})

add_library(decn decn/decn.c)

add_library(calc qt_main.cpp calc.c utils.c lcd_emulator.c key.c)
target_link_libraries(calc Qt5::Widgets)

add_executable(keytest key.c)
target_compile_definitions(keytest PRIVATE KEY_TEST_APP=1)

add_executable(decn_test decn/decn_test.c utils.c)
target_link_libraries(decn_test decn)
