cmake_minimum_required(VERSION 3.14)
project(HyperLPR3-LinuxDemo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")

if (APPLE)
    set(PLAT darwin)
else()
    set(PLAT linux)
endif ()

# You need to link to the opencv library
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# You need to compile hyperlpr3 library to use it
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hyperlpr3/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/hyperlpr3/lib)

add_executable(PlateRecDemo ${CMAKE_CURRENT_SOURCE_DIR}/plate_rec_demo.cpp)
target_link_libraries(PlateRecDemo  hyperlpr3 ${OpenCV_LIBS})