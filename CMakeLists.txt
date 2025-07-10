cmake_minimum_required(VERSION 3.22)
project(CalcGTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(CalcLib ICalculator.h SimpleCalculator.h SimpleCalculator.cpp
                    IHistory.h InMemoryHistory.h InMemoryHistory.cpp)
target_compile_options(CalcLib PRIVATE -coverage)
target_link_options(CalcLib PRIVATE -coverage)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(unit_tests tests/tests.cpp)

target_compile_options(unit_tests PRIVATE -coverage)
target_link_options(unit_tests PRIVATE -coverage)

target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(unit_tests PRIVATE CalcLib GTest::gtest_main GTest::gmock)

include(GoogleTest)

gtest_discover_tests(unit_tests)
