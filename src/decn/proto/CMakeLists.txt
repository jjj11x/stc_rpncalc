
add_executable(div div_mfp.cpp)
target_link_libraries(div gmp)

add_executable(ln ln_mfp.cpp)
target_link_libraries(ln mpfr)

add_executable(exp exp.cpp)
target_link_libraries(exp mpfr)
