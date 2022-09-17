//
// Created by Jack Yu on 22/09/2017.
//

#ifndef HYPERPR_FINEMAPPING_H
#define HYPERPR_FINEMAPPING_H

#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>

#include <string>
namespace pr {
class FineMapping {
public:
  FineMapping();

  FineMapping(std::string prototxt, std::string caffemodel);
  static cv::Mat FineMappingVertical(cv::Mat InputProposal, int sliceNum = 15,
                                     int upper = 0, int lower = -50,
                                     int windows_size = 17);
  cv::Mat FineMappingHorizon(cv::Mat FinedVertical, int leftPadding,
                             int rightPadding);

private:
  cv::dnn::Net net;
};

} // namespace pr
#endif // HYPERPR_FINEMAPPING_H
