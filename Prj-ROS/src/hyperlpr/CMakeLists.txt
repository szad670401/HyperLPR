cmake_minimum_required(VERSION 3.0.2)
project(hyperlpr)

set(SRC_DETECTION src/PlateDetection.cpp  src/util.h include/PlateDetection.h)

set(SRC_FINEMAPPING src/FineMapping.cpp )

set(SRC_FASTDESKEW src/FastDeskew.cpp )

set(SRC_SEGMENTATION src/PlateSegmentation.cpp )

set(SRC_RECOGNIZE src/Recognizer.cpp src/CNNRecognizer.cpp)

set(SRC_PIPLINE src/Pipeline.cpp)

set(SRC_SEGMENTATIONFREE src/SegmentationFreeRecognizer.cpp )


## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(OpenCV REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  cmake_modules
  roscpp
  rospy
  std_msgs
  sensor_msgs
  geometry_msgs
  cv_bridge
  image_transport
)

catkin_package(
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)


add_executable(${PROJECT_NAME}_node ${SRC_DETECTION} ${SRC_FINEMAPPING} ${SRC_FASTDESKEW} ${SRC_SEGMENTATION} ${SRC_RECOGNIZE} ${SRC_PIPLINE} ${SRC_SEGMENTATIONFREE} src/hyperlpr_node.cpp src/main.cpp)

target_link_libraries(${PROJECT_NAME}_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
