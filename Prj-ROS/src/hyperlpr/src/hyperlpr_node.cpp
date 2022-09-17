#include "hyperlpr_node.h"

void SubPuber::SendPath(){
  prc.initialize(modelPath+"/cascade.xml",
                 modelPath+"/HorizonalFinemapping.prototxt",
                 modelPath+"/HorizonalFinemapping.caffemodel",
                 modelPath+"/Segmentation.prototxt",
                 modelPath+"/Segmentation.caffemodel",
                 modelPath+"/CharacterRecognization.prototxt",
                 modelPath+"/CharacterRecognization.caffemodel",
                 modelPath+"/SegmenationFree-Inception.prototxt",
                 modelPath+"/SegmenationFree-Inception.caffemodel");
}

void SubPuber::PlateRecognitionCallback(const sensor_msgs::ImageConstPtr &cameraImage)
{
  cv::Mat img = cv_bridge::toCvShare(cameraImage, "bgr8")->image;

  //使用端到端模型模型进行识别 识别结果将会保存在res里面
  std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(img, pr::SEGMENTATION_FREE_METHOD);

  for(auto st:res) {
    if(st.confidence>0.75) {
      std_msgs::String recognitionResult;
      recognitionResult.data = st.getPlateName();

      std_msgs::Float32 recognitionConfidence;
      recognitionConfidence.data = st.confidence;

      // 使用ROS_INFO输出中文车牌与置信度
      setlocale(LC_CTYPE, "zh_CN.utf8");
      ROS_INFO("Plate found: %s, confidence is: %f", recognitionResult.data.c_str(), recognitionConfidence.data);

      // 获取车牌位置
      cv::Rect region = st.getPlateRect();
      // 框选出车牌位置
      cv::rectangle(img, cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),2);

      // 将识别结果作为一个topic发布
      recognitionResultPub.publish(recognitionResult);
      // 将识别置信度作为一个topic发布
      recognitionConfidencePub.publish(recognitionConfidence);
    }
  }
  // 将框选出当前帧所有车牌位置的图像作为一个topic发布
  sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", img).toImageMsg();
  recognizedImagePub.publish(*msg);
}