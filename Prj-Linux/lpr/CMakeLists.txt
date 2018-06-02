cmake_minimum_required(VERSION 3.6)
project(SwiftPR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
find_package(OpenCV 3.3.0 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(include)

set(SRC_DETECTION src/PlateDetection.cpp  src/util.h include/PlateDetection.h)

set(SRC_FINEMAPPING src/FineMapping.cpp )

set(SRC_FASTDESKEW src/FastDeskew.cpp )

set(SRC_SEGMENTATION src/PlateSegmentation.cpp )

set(SRC_RECOGNIZE src/Recognizer.cpp src/CNNRecognizer.cpp)

set(SRC_PIPLINE src/Pipeline.cpp)

set(SRC_SEGMENTATIONFREE src/SegmentationFreeRecognizer.cpp )

#set(SOURCE_FILES main.cpp)
#add_executable(HyperLPR_cpp ${SOURCE_FILES})

#TEST_DETECTION
add_executable(TEST_Detection ${SRC_DETECTION} tests/test_detection.cpp)
target_link_libraries(TEST_Detection ${OpenCV_LIBS})

#TEST_FINEMAPPING
add_executable(TEST_FINEMAPPING ${SRC_FINEMAPPING} tests/test_finemapping.cpp)
target_link_libraries(TEST_FINEMAPPING ${OpenCV_LIBS})

#TEST_DESKEW

add_executable(TEST_FASTDESKEW ${SRC_FASTDESKEW} tests/test_fastdeskew.cpp)
target_link_libraries(TEST_FASTDESKEW ${OpenCV_LIBS})

#TEST_SEGMENTATION

add_executable(TEST_SEGMENTATION ${SRC_SEGMENTATION} ${SRC_RECOGNIZE} tests/test_segmentation.cpp)
target_link_libraries(TEST_SEGMENTATION ${OpenCV_LIBS})

#TEST_RECOGNIZATION

add_executable(TEST_RECOGNIZATION  ${SRC_RECOGNIZE} tests/test_recognization.cpp)
target_link_libraries(TEST_RECOGNIZATION ${OpenCV_LIBS})

#TEST_SEGMENTATIONFREE
add_executable(TEST_SEGMENTATIONFREE ${SRC_SEGMENTATIONFREE} tests/test_segmentationFree.cpp)
target_link_libraries(TEST_SEGMENTATIONFREE ${OpenCV_LIBS})

#TEST_PIPELINE

add_executable(TEST_PIPLINE ${SRC_DETECTION} ${SRC_FINEMAPPING} ${SRC_FASTDESKEW} ${SRC_SEGMENTATION} ${SRC_RECOGNIZE} ${SRC_PIPLINE} ${SRC_SEGMENTATIONFREE} tests/test_pipeline.cpp)
target_link_libraries(TEST_PIPLINE ${OpenCV_LIBS})
