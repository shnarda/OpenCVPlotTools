cmake_minimum_required(VERSION 3.13)

project(OpenCVPlotTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OPENCV package directives
find_package(OpenCV REQUIRED)


add_executable(Example Examples/Histogram_example.cpp
    inc/plotelementbase.h
    inc/histogram.h
    src/histogram.cpp
    inc/paintertools.h
    src/paintertools.cpp
    )
target_include_directories(Example PRIVATE ${OpenCV_INCLUDE_DIRS} inc)
target_link_libraries(Example PRIVATE ${OpenCV_LIBS})



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



add_library(OpenCVPlotToolsLib STATIC
    src/histogram.cpp
    src/paintertools.cpp
)
target_include_directories(OpenCVPlotToolsLib PRIVATE ${OpenCV_INCLUDE_DIRS} inc)
target_link_libraries(OpenCVPlotToolsLib PRIVATE ${OpenCV_LIBS})

# Test files
add_executable(testRunner
    Tests/TestHistogram.cpp
)
target_include_directories(testRunner PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    inc
)
target_link_libraries(testRunner
    OpenCVPlotToolsLib
    ${OpenCV_LIBS}
    gtest_main
    gmock_main)
