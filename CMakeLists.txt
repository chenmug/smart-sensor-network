cmake_minimum_required(VERSION 3.16)
project(SmartSensorNetwork LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------
# Core sources
# --------------------
file(GLOB_RECURSE CORE_SOURCES
    src/common/*.cpp
    src/network/*.cpp
    src/sensor/*.cpp
    src/gateway/*.cpp
    src/monitor/*.cpp
)

file(GLOB_RECURSE UI_SOURCES
    src/ui/*.cpp
)

add_library(sensor_lib ${CORE_SOURCES})
add_library(ui_lib ${UI_SOURCES})

target_include_directories(sensor_lib PUBLIC include)
target_include_directories(ui_lib PUBLIC include)


# --------------------
# Main app
# --------------------
add_executable(sensor_app
    main.cpp
    ${DEMO_SOURCES}
)

target_link_libraries(sensor_app
    sensor_lib
    ui_lib
)

# --------------------
# Tests
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

target_include_directories(run_tests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/tests
)

add_test(NAME SensorTests COMMAND run_tests)