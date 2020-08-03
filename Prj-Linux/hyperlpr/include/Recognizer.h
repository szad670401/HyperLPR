//
// Created by Jack Yu on 20/10/2017.
//

#ifndef HYPERPR_RECOGNIZER_H
#define HYPERPR_RECOGNIZER_H

#include "PlateInfo.h"
#include "opencv2/dnn.hpp"
namespace pr {
typedef cv::Mat label;
class GeneralRecognizer {
public:
  virtual label recognizeCharacter(cv::Mat character) = 0;
  //            virtual cv::Mat SegmentationFreeForSinglePlate(cv::Mat plate) =
  //            0;
  void SegmentBasedSequenceRecognition(PlateInfo &plateinfo);
  void SegmentationFreeSequenceRecognition(PlateInfo &plateInfo);
};

} // namespace pr
#endif // HYPERPR_RECOGNIZER_H
