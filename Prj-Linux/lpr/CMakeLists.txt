cmake_minimum_required(VERSION 3.6)
project(SwiftPR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
find_package(OpenCV 3.4.3 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(include)

set(SRC_DETECTION src/PlateDetection.cpp)
set(SRC_FINETUNE src/FineTune.cpp)
set(SRC_RECOGNIZE src/PlateRecognation.cpp)
set(SRC_PIPLINE src/Pipeline.cpp)

#TEST_PIPELINE
add_executable(TEST_PIPLINE ${SRC_DETECTION} ${SRC_FINETUNE} ${SRC_RECOGNIZE} ${SRC_PIPLINE} tests/testPipeLine.cpp)
target_link_libraries(TEST_PIPLINE ${OpenCV_LIBS})
