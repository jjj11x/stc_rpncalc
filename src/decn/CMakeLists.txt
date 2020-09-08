add_library(Catch INTERFACE)
if(EXISTS /usr/include/catch/catch.hpp)
target_include_directories(Catch INTERFACE /usr/include/catch)
elseif(EXISTS /usr/include/catch2/catch.hpp)
target_include_directories(Catch INTERFACE /usr/include/catch2)
else()
endif()

#code coverage
add_library(coverage_config INTERFACE)
target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
target_link_libraries(coverage_config INTERFACE --coverage)

# decn library
add_library(decn decn.c ../utils.c)

# decn library with coverage
add_library(decn_cover decn.c)
target_link_libraries(decn_cover PUBLIC coverage_config)

add_executable(decn_test decn_test.c ../utils.c)
target_link_libraries(decn_test decn_cover coverage_config Catch)

add_executable(decn_tests catch_main.cpp decn_tests.cpp ../utils.c)
target_link_libraries(decn_tests decn_cover coverage_config mpfr Catch)

# decn prototyping
add_subdirectory(proto)
