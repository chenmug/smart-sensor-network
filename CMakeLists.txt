cmake_minimum_required(VERSION 3.16)
project(SmartSensorNetwork LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------
# Source files
# --------------------
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# --------------------
# Main library 
# --------------------
add_library(sensor_lib ${SOURCES})

target_include_directories(sensor_lib PUBLIC include)

# --------------------
# Main application
# --------------------
add_executable(sensor_app main.cpp)

target_link_libraries(sensor_app sensor_lib)

# --------------------
# GoogleTest
# --------------------
enable_testing()

find_package(GTest REQUIRED)

file(GLOB_RECURSE TEST_SOURCES
    tests/*.cpp
)

add_executable(run_tests ${TEST_SOURCES})

target_link_libraries(run_tests
    sensor_lib
    GTest::gtest
    GTest::gtest_main
    pthread
)

add_test(NAME SensorTests COMMAND run_tests)