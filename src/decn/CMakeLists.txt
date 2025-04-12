#code coverage
add_library(coverage_config INTERFACE)
target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
target_link_libraries(coverage_config INTERFACE --coverage)

# decn library
add_library(decn decn.c ../utils.c)

# decn library with coverage
add_library(decn_cover decn.c)
target_link_libraries(decn_cover PUBLIC coverage_config)

# old tests (compare output with reference "golden" output file)
add_executable(decn_test
	decn_test.c
	../utils.c
)
target_link_libraries(decn_test
	decn_cover
	coverage_config
)

# catch2 unit tests
find_package(Catch2 3 REQUIRED)
enable_testing()
set (BUILD_TESTING ON)
add_executable(decn_tests
	decn_tests.cpp
	decn_tests_div_sqrt.cpp
	decn_tests_transcendental.cpp
	decn_tests_trig.cpp
	../utils.c
)
target_link_libraries(decn_tests
	mpfr
	decn_cover
	coverage_config
	Catch2::Catch2WithMain
)
include(CTest)
include(Catch)
catch_discover_tests(decn_tests)

# decn prototyping
add_subdirectory(proto)

