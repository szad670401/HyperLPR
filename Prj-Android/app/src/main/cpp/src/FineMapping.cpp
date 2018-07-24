//
// Created by 庾金科 on 22/09/2017.
//

#include "../include/FineMapping.h"
namespace pr{

    const int FINEMAPPING_H = 60 ;
    const int FINEMAPPING_W = 140;
    const int PADDING_UP_DOWN = 30;
    void drawRect(cv::Mat image,cv::Rect rect)
    {
        cv::Point p1(rect.x,rect.y);
        cv::Point p2(rect.x+rect.width,rect.y+rect.height);
        cv::rectangle(image,p1,p2,cv::Scalar(0,255,0),1);
    }


    FineMapping::FineMapping(std::string prototxt,std::string caffemodel) {
         net = cv::dnn::readNetFromCaffe(prototxt, caffemodel);

    }

    cv::Mat FineMapping::FineMappingHorizon(cv::Mat FinedVertical,int leftPadding,int rightPadding)
    {

//        if(FinedVertical.channels()==1)
//            cv::cvtColor(FinedVertical,FinedVertical,cv::COLOR_GRAY2BGR);
        cv::Mat inputBlob = cv::dnn::blobFromImage(FinedVertical, 1/255.0, cv::Size(66,16),
                                      cv::Scalar(0,0,0),false);

        net.setInput(inputBlob,"data");
        cv::Mat prob = net.forward();
        int front = static_cast<int>(prob.at<float>(0,0)*FinedVertical.cols);
        int back = static_cast<int>(prob.at<float>(0,1)*FinedVertical.cols);
        front -= leftPadding ;
        if(front<0) front = 0;
        back +=rightPadding;
        if(back>FinedVertical.cols-1) back=FinedVertical.cols - 1;
        cv::Mat cropped  = FinedVertical.colRange(front,back).clone();
        return  cropped;


    }
    std::pair<int,int> FitLineRansac(std::vector<cv::Point> pts,int zeroadd = 0 )
    {
        std::pair<int,int> res;
        if(pts.size()>2)
        {
            cv::Vec4f line;
            cv::fitLine(pts,line,cv::DIST_HUBER,0,0.01,0.01);
            float vx = line[0];
            float vy = line[1];
            float x = line[2];
            float y = line[3];
            int lefty = static_cast<int>((-x * vy / vx) + y);
            int righty = static_cast<int>(((136- x) * vy / vx) + y);
            res.first = lefty+PADDING_UP_DOWN+zeroadd;
            res.second = righty+PADDING_UP_DOWN+zeroadd;
            return res;
        }
        res.first = zeroadd;
        res.second = zeroadd;
        return res;
    }

    cv::Mat FineMapping::FineMappingVertical(cv::Mat InputProposal,int sliceNum,int upper,int lower,int windows_size){


        cv::Mat PreInputProposal;
        cv::Mat proposal;

        cv::resize(InputProposal,PreInputProposal,cv::Size(FINEMAPPING_W,FINEMAPPING_H));
//        cv::imwrite("res/cache/finemapping.jpg",PreInputProposal);

        if(InputProposal.channels() == 3)
            cv::cvtColor(PreInputProposal,proposal,cv::COLOR_BGR2GRAY);
        else
            PreInputProposal.copyTo(proposal);

//            proposal = PreInputProposal;

        // this will improve some sen
        cv::Mat kernal = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(1,3));
//        cv::erode(proposal,proposal,kernal);


        float diff = static_cast<float>(upper-lower);
        diff/=static_cast<float>(sliceNum-1);
        cv::Mat binary_adaptive;
        std::vector<cv::Point> line_upper;
        std::vector<cv::Point> line_lower;
        int contours_nums=0;

        for(int i = 0 ; i < sliceNum ; i++)
        {
            std::vector<std::vector<cv::Point> > contours;
            float k =lower + i*diff;
            cv::adaptiveThreshold(proposal,binary_adaptive,255,cv::ADAPTIVE_THRESH_MEAN_C,cv::THRESH_BINARY,windows_size,k);
            cv::Mat draw;
            binary_adaptive.copyTo(draw);
            cv::findContours(binary_adaptive,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
            for(auto contour: contours)
            {
                cv::Rect bdbox =cv::boundingRect(contour);
                float lwRatio = bdbox.height/static_cast<float>(bdbox.width);
                int  bdboxAera = bdbox.width*bdbox.height;
                if ((   lwRatio>0.7&&bdbox.width*bdbox.height>100 && bdboxAera<300)
                    || (lwRatio>3.0 && bdboxAera<100 && bdboxAera>10))
                {
                    cv::Point p1(bdbox.x, bdbox.y);
                    cv::Point p2(bdbox.x + bdbox.width, bdbox.y + bdbox.height);
                    line_upper.push_back(p1);
                    line_lower.push_back(p2);
                    contours_nums+=1;
                }
            }
        }


        if(contours_nums<41)
        {
            cv::bitwise_not(InputProposal,InputProposal);
            cv::Mat kernal = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(1,5));
            cv::Mat bak;
            cv::resize(InputProposal,bak,cv::Size(FINEMAPPING_W,FINEMAPPING_H));
            cv::erode(bak,bak,kernal);
            if(InputProposal.channels() == 3)
                cv::cvtColor(bak,proposal,cv::COLOR_BGR2GRAY);
            else
                proposal = bak;
            int contours_nums=0;

            for(int i = 0 ; i < sliceNum ; i++)
            {
                std::vector<std::vector<cv::Point> > contours;
                float k =lower + i*diff;
                cv::adaptiveThreshold(proposal,binary_adaptive,255,cv::ADAPTIVE_THRESH_MEAN_C,cv::THRESH_BINARY,windows_size,k);
//                cv::imshow("image",binary_adaptive);
//            cv::waitKey(0);
                cv::Mat draw;
                binary_adaptive.copyTo(draw);
                cv::findContours(binary_adaptive,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
                for(auto contour: contours)
                {
                    cv::Rect bdbox =cv::boundingRect(contour);
                    float lwRatio = bdbox.height/static_cast<float>(bdbox.width);
                    int  bdboxAera = bdbox.width*bdbox.height;
                    if ((   lwRatio>0.7&&bdbox.width*bdbox.height>120 && bdboxAera<300)
                        || (lwRatio>3.0 && bdboxAera<100 && bdboxAera>10))
                    {

                        cv::Point p1(bdbox.x, bdbox.y);
                        cv::Point p2(bdbox.x + bdbox.width, bdbox.y + bdbox.height);
                        line_upper.push_back(p1);
                        line_lower.push_back(p2);
                        contours_nums+=1;
                    }
                }
            }
//            std:: cout<<"contours_nums "<<contours_nums<<std::endl;
        }

            cv::Mat rgb;
            cv::copyMakeBorder(PreInputProposal, rgb, PADDING_UP_DOWN, PADDING_UP_DOWN, 0, 0, cv::BORDER_REPLICATE);
//        cv::imshow("rgb",rgb);
//        cv::waitKey(0);
//


            std::pair<int, int> A;
            std::pair<int, int> B;
            A = FitLineRansac(line_upper, -1);
            B = FitLineRansac(line_lower, 1);
            int leftyB = A.first;
            int rightyB = A.second;
            int leftyA = B.first;
            int rightyA = B.second;
            int cols = rgb.cols;
            int rows = rgb.rows;
//        pts_map1  = np.float32([[cols - 1, rightyA], [0, leftyA],[cols - 1, rightyB], [0, leftyB]])
//        pts_map2 = np.float32([[136,36],[0,36],[136,0],[0,0]])
//        mat = cv2.getPerspectiveTransform(pts_map1,pts_map2)
//        image = cv2.warpPerspective(rgb,mat,(136,36),flags=cv2.INTER_CUBIC)
            std::vector<cv::Point2f> corners(4);
            corners[0] = cv::Point2f(cols - 1, rightyA);
            corners[1] = cv::Point2f(0, leftyA);
            corners[2] = cv::Point2f(cols - 1, rightyB);
            corners[3] = cv::Point2f(0, leftyB);
            std::vector<cv::Point2f> corners_trans(4);
            corners_trans[0] = cv::Point2f(136, 36);
            corners_trans[1] = cv::Point2f(0, 36);
            corners_trans[2] = cv::Point2f(136, 0);
            corners_trans[3] = cv::Point2f(0, 0);
            cv::Mat transform = cv::getPerspectiveTransform(corners, corners_trans);
            cv::Mat quad = cv::Mat::zeros(36, 136, CV_8UC3);
            cv::warpPerspective(rgb, quad, transform, quad.size());
        return quad;

    }


}


