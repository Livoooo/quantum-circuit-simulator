cmake_minimum_required(VERSION 3.20)
project(qsim CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qsim src/statevector.cpp)
target_include_directories(qsim PUBLIC include)
target_compile_options(qsim PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_executable(test_statevector tests/test_statevector.cpp)
target_link_libraries(test_statevector qsim)
add_test(NAME statevector COMMAND test_statevector)