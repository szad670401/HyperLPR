#include "hyperlpr_node.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "hyperlpr_node");

  // 使用一个类完成订阅 识别 发布
  SubPuber plateRecognizer;

  // 获取模型路径
  ros::param::get("ModelPath", plateRecognizer.modelPath);
  // 发送路径，初始化图像处理管线
  plateRecognizer.SendPath();

  ros::spin();
  return 0;
}
