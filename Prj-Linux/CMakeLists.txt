cmake_minimum_required(VERSION 3.10.2)
project(HyperLPR3-LinuxDemo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin")

if (APPLE)
    set(PLAT darwin)
else()
    set(PLAT linux)
endif ()

# use opencv
set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty_hyper_inspire_op/opencv-4.5.1/${PLAT}/lib/cmake/opencv4)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# use hyperlpr3 sdk
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hyperlpr3/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/hyperlpr3/lib)

add_executable(PlateRecDemo ${CMAKE_CURRENT_SOURCE_DIR}/plate_rec_demo.cpp)
target_link_libraries(PlateRecDemo hyperlpr3 ${OpenCV_LIBS})