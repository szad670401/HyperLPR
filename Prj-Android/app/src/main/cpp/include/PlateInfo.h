//
// Created by 庾金科 on 20/09/2017.
//

#ifndef SWIFTPR_PLATEINFO_H
#define SWIFTPR_PLATEINFO_H
#include <opencv2/opencv.hpp>
namespace pr {

    typedef std::vector<cv::Mat> Character;

    enum PlateColor { BLUE, YELLOW, WHITE, GREEN, BLACK,UNKNOWN};
    enum CharType {CHINESE,LETTER,LETTER_NUMS,INVALID};


    class PlateInfo {
        public:
            std::vector<std::pair<CharType,cv::Mat>> plateChars;
            std::vector<std::pair<CharType,cv::Mat>> plateCoding;
            float confidence = 0;
            PlateInfo(const cv::Mat &plateData, std::string plateName, cv::Rect plateRect, PlateColor plateType) {
                    licensePlate = plateData;
                    name = plateName;
                    ROI = plateRect;
                    Type = plateType;
                }
            PlateInfo(const cv::Mat &plateData, cv::Rect plateRect, PlateColor plateType) {
                licensePlate = plateData;
                ROI = plateRect;
                Type = plateType;
            }
            PlateInfo(const cv::Mat &plateData, cv::Rect plateRect) {
                licensePlate = plateData;
                ROI = plateRect;
            }
            PlateInfo() {

            }

            cv::Mat getPlateImage() {
                return licensePlate;
            }

            void setPlateImage(cv::Mat plateImage){
                licensePlate = plateImage;
            }

            cv::Rect getPlateRect() {
                return ROI;
            }

            void setPlateRect(cv::Rect plateRect)   {
                ROI = plateRect;
            }
            cv::String getPlateName() {
                return name;

            }
            void setPlateName(cv::String plateName) {
                name = plateName;
            }
            int getPlateType() {
                return Type;
            }

            void appendPlateChar(const std::pair<CharType,cv::Mat> &plateChar)
            {
                plateChars.push_back(plateChar);
            }

            void appendPlateCoding(const std::pair<CharType,cv::Mat> &charProb){
                plateCoding.push_back(charProb);
            }

    //        cv::Mat getPlateChars(int id) {
    //            if(id<PlateChars.size())
    //                return PlateChars[id];
    //        }
            std::string decodePlateNormal(std::vector<std::string> mappingTable) {
                std::string decode;
                for(auto plate:plateCoding) {
                    float *prob = (float *)plate.second.data;
                    if(plate.first == CHINESE) {

                        decode += mappingTable[std::max_element(prob,prob+31) - prob];
                        confidence+=*std::max_element(prob,prob+31);


//                        std::cout<<*std::max_element(prob,prob+31)<<std::endl;

                    }

                    else if(plate.first == LETTER) {
                        decode += mappingTable[std::max_element(prob+41,prob+65)- prob];
                        confidence+=*std::max_element(prob+41,prob+65);
                    }

                    else if(plate.first == LETTER_NUMS) {
                        decode += mappingTable[std::max_element(prob+31,prob+65)- prob];
                        confidence+=*std::max_element(prob+31,prob+65);
//                        std::cout<<*std::max_element(prob+31,prob+65)<<std::endl;

                    }
                    else if(plate.first == INVALID)
                    {
                        decode+='*';
                    }

                }
                name = decode;

                confidence/=7;

                return decode;
            }

    private:
        cv::Mat licensePlate;
        cv::Rect ROI;
        std::string name ;
        PlateColor Type;
    };
}


#endif //SWIFTPR_PLATEINFO_H
