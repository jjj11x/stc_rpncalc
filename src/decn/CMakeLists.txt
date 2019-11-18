
# decn library
add_library(decn decn.c)

add_executable(decn_test decn_test.c ../utils.c)
target_link_libraries(decn_test decn)

add_executable(decn_tests catch_main.cpp decn_tests.cpp ../utils.c)
target_link_libraries(decn_tests decn mpfr)

# decn prototyping
add_subdirectory(proto)
