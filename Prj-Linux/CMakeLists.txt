cmake_minimum_required(VERSION 3.10.2)
project(HyperLPR3-LinuxDemo)

set(CMAKE_CXX_STANDARD 11)

# use opencv
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# use hyperlpr3 sdk
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hyperlpr3/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/hyperlpr3/lib)

add_executable(PlateRecDemo ${CMAKE_CURRENT_SOURCE_DIR}/plate_rec_demo.cpp)
target_link_libraries(PlateRecDemo hyperlpr3 ${OpenCV_LIBS})