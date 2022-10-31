#ifndef HYPERLPR_NODE_H
#define HYPERLPR_NODE_H

#include "Pipeline.h"
#include "ros/ros.h"
#include "sensor_msgs/CompressedImage.h"
#include "sensor_msgs/image_encodings.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <iostream>

class SubPuber
{
private:

  ros::NodeHandle nodeHandle;
  ros::Subscriber cameraImageSub;
  ros::Publisher recognitionResultPub;
  ros::Publisher recognitionConfidencePub;
  ros::Publisher recognizedImagePub;

  pr::PipelinePR prc;

public:

  // 模型路径，从launch文件中获得，用于寻找模型
  std::string modelPath;
  void SendPath();

  SubPuber(){
    // 获取相机话题
    std::string cameraTopic;
    ros::param::get("CameraTopic", cameraTopic);
    // 订阅相机话题，此处默认为realsense彩色图像的topic
    cameraImageSub = nodeHandle.subscribe(cameraTopic, 1, &SubPuber::PlateRecognitionCallback, this);
    // 发布识别结果
    recognitionResultPub = nodeHandle.advertise<std_msgs::String>("/recognition_result", 1);
    // 发布识别置信度
    recognitionConfidencePub = nodeHandle.advertise<std_msgs::Float32>("/recognition_confidence", 1);
    // 发布框选出车牌后的图像
    recognizedImagePub = nodeHandle.advertise<sensor_msgs::Image>("/recognized_image", 1);
  }

  void PlateRecognitionCallback(const sensor_msgs::ImageConstPtr &cameraImage);
};

#endif