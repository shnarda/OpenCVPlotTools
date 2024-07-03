cmake_minimum_required(VERSION 3.13)

project(OpenCVPlotTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenCV package directives
find_package(OpenCV REQUIRED)

################## Library #######################

add_library(OpenCVPlotTools STATIC
    src/histogram.cpp
    src/plotelementbase.cpp
)

target_include_directories(OpenCVPlotTools PRIVATE ${OpenCV_INCLUDE_DIRS} inc)
target_link_libraries(OpenCVPlotTools PRIVATE ${OpenCV_LIBS})

################# Examples #####################

add_executable(Example Examples/Histogram_example.cpp)
target_include_directories(Example PRIVATE ${OpenCV_INCLUDE_DIRS} inc)
target_link_libraries(Example PRIVATE
    ${OpenCV_LIBS}
    OpenCVPlotTools
)


################ Tests #########################

# GTest package directives
include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG release-1.11.0)
FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BUILD_DIR})
endif()

# Test files
add_executable(testRunner
    Tests/TestHistogram.cpp
)
target_include_directories(testRunner PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    inc
)
target_link_libraries(testRunner
    OpenCVPlotTools
    ${OpenCV_LIBS}
    gtest_main
    gmock_main)
