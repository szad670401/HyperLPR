cmake_minimum_required(VERSION 3.6)
project(platescan)

set(OpenCV_DIR /usr/platescan/opencv-3.3.0/release/)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -std=c++11 -fpic -o")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 11)

set(PHP_CONFIG_DIR /etc/php.d/)
set(PHP_CONFIG php-config)
set(POST_COMPILE_COMMAND systemctl restart php-fpm)

find_package(OpenCV 3.3.0 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(include)


add_library( # Sets the name of the library.
    platescan

    # Sets the library as a shared library.
    SHARED

    # Provides a relative path to your source file(s).
    main.cpp 

    lpr/CNNRecognizer.cpp
    lpr/FastDeskew.cpp
    lpr/FineMapping.cpp
    lpr/Pipeline.cpp
    lpr/PlateDetection.cpp
    lpr/PlateSegmentation.cpp
    lpr/Recognizer.cpp
    lpr/SegmentationFreeRecognizer.cpp
)


target_link_libraries(
  platescan
  phpcpp
  ${OpenCV_LIBS}
)

execute_process(COMMAND ${PHP_CONFIG} --extension-dir
        OUTPUT_VARIABLE LIBRARY_DIR)

message("Test ${LIBRARY_DIR} failed.")

add_custom_command(TARGET platescan
        POST_BUILD
        COMMAND mv ./libplatescan.so ./platescan.so
        COMMAND cp -f ./platescan.so ${LIBRARY_DIR}
        COMMAND ${POST_COMPILE_COMMAND}
        )        
