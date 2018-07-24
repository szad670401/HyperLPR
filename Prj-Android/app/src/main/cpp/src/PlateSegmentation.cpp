//
// Created by 庾金科 on 16/10/2017.
//

#include "../include/PlateSegmentation.h"
#include "../include/niBlackThreshold.h"


//#define DEBUG
namespace pr{

    PlateSegmentation::PlateSegmentation(std::string prototxt,std::string caffemodel) {
        net = cv::dnn::readNetFromCaffe(prototxt, caffemodel);
    }
    cv::Mat PlateSegmentation::classifyResponse(const cv::Mat &cropped){
        cv::Mat inputBlob = cv::dnn::blobFromImage(cropped, 1/255.0, cv::Size(22,22), cv::Scalar(0,0,0),false);
        net.setInput(inputBlob,"data");
        return net.forward();
    }

    void drawHist(float* seq,int size,const char* name){
        cv::Mat image(300,size,CV_8U);
        image.setTo(0);
        float* start =seq;
        float* end = seq+size;
        float l = *std::max_element(start,end);
        for(int i = 0;i<size;i++)
        {
            int p = int(float(seq[i])/l*300);
            cv::line(image,cv::Point(i,300),cv::Point(i,300-p),cv::Scalar(255,255,255));
        }
        cv::resize(image,image,cv::Size(600,100));
        cv::imshow(name,image);
    }

    inline void computeSafeMargin(int &val,const int &rows){
        val = std::min(val,rows);
        val = std::max(val,0);
    }

    cv::Rect boxFromCenter(const cv::Point center,int left,int right,int top,int bottom,cv::Size bdSize)
    {
        cv::Point p1(center.x -  left ,center.y - top);
        cv::Point p2( center.x + right, center.y + bottom);
        p1.x = std::max(0,p1.x);
        p1.y = std::max(0,p1.y);
        p2.x = std::min(p2.x,bdSize.width-1);
        p2.y = std::min(p2.y,bdSize.height-1);
        cv::Rect rect(p1,p2);
        return rect;
    }

    cv::Rect boxPadding(cv::Rect rect,int left,int right,int top,int bottom,cv::Size bdSize)
    {

        cv::Point center(rect.x+(rect.width>>1),rect.y + (rect.height>>1));
        int rebuildLeft  = (rect.width>>1 )+ left;
        int rebuildRight = (rect.width>>1 )+ right;
        int rebuildTop = (rect.height>>1 )+ top;
        int rebuildBottom = (rect.height>>1 )+ bottom;
        return boxFromCenter(center,rebuildLeft,rebuildRight,rebuildTop,rebuildBottom,bdSize);

    }



    void PlateSegmentation:: refineRegion(cv::Mat &plateImage,const std::vector<int> &candidatePts,const int padding,std::vector<cv::Rect> &rects){
        int w = candidatePts[5] - candidatePts[4];
        int cols = plateImage.cols;
        int rows = plateImage.rows;
        for(int i = 0; i < candidatePts.size()  ; i++)
        {
            int left = 0;
            int right = 0 ;

            if(i == 0 ){
                left= candidatePts[i];
                right = left+w+padding;
                }
            else {
                left = candidatePts[i] - padding;
                right = left + w + padding * 2;
            }

            computeSafeMargin(right,cols);
            computeSafeMargin(left,cols);
            cv::Mat roiImage;
//            plateImage.copyTo(roiImage);
            cv::Rect roi(left,0,right-left,rows-1);
            plateImage(roi).copyTo(roiImage);

            if (i>=1)
            {

                cv::Mat roi_thres;
//                cv::threshold(roiImage,roi_thres,0,255,cv::THRESH_OTSU|cv::THRESH_BINARY);

                niBlackThreshold(roiImage,roi_thres,255,cv::THRESH_BINARY,15,0.27,BINARIZATION_NIBLACK);

                std::vector<std::vector<cv::Point>> contours;
                cv::findContours(roi_thres,contours,cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);
                cv::Point boxCenter(roiImage.cols>>1,roiImage.rows>>1);

                cv::Rect final_bdbox;
                cv::Point final_center;
                int final_dist = INT_MAX;


                for(auto contour:contours)
                {
                    cv::Rect bdbox = cv::boundingRect(contour);
                    cv::Point center(bdbox.x+(bdbox.width>>1),bdbox.y + (bdbox.height>>1));
                    int dist = (center.x - boxCenter.x)*(center.x - boxCenter.x);
                    if(dist<final_dist and  bdbox.height > rows>>1)
                    {   final_dist =dist;
                        final_center = center;
                        final_bdbox = bdbox;
                    }
                }

                //rebuild box
                if(final_bdbox.height/ static_cast<float>(final_bdbox.width) > 3.5 && final_bdbox.width*final_bdbox.height<10)
                    final_bdbox = boxFromCenter(final_center,8,8,(rows>>1)-3 , (rows>>1) - 2,roiImage.size());
                else {
                    if(i == candidatePts.size()-1)
                        final_bdbox = boxPadding(final_bdbox, padding/2, padding, padding/2, padding/2, roiImage.size());
                    else
                        final_bdbox = boxPadding(final_bdbox, padding, padding, padding, padding, roiImage.size());


//                    std::cout<<final_bdbox<<std::endl;
//                    std::cout<<roiImage.size()<<std::endl;
#ifdef DEBUG
                    cv::imshow("char_thres",roi_thres);

                    cv::imshow("char",roiImage(final_bdbox));
                    cv::waitKey(0);
#endif


                }


                final_bdbox.x += left;

                rects.push_back(final_bdbox);
//

            }
            else
            {
                rects.push_back(roi);
            }

//            else
//            {
//
//            }

//            cv::GaussianBlur(roiImage,roiImage,cv::Size(7,7),3);
//
//            cv::imshow("image",roiImage);
//            cv::waitKey(0);


        }



    }
    void avgfilter(float *angle_list,int size,int windowsSize) {
        float *filterd = new float[size];
        for(int i = 0 ; i < size ; i++) filterd [i] = angle_list[i];
//        memcpy(filterd,angle_list,size);

        cv::Mat kernal_gaussian = cv::getGaussianKernel(windowsSize,3,CV_32F);
        float *kernal = (float*)kernal_gaussian.data;
//        kernal+=windowsSize;
        int r = windowsSize/2;




        for (int i = 0; i < size; i++) {
            float avg = 0.00f;
            for (int j = 0; j < windowsSize; j++) {
                if(i+j-r>0&&i+j+r<size-1)
                    avg += filterd[i + j-r]*kernal[j];
            }
//            avg = avg / windowsSize;
            angle_list[i] = avg;

        }

        delete[] filterd;
//        delete filterd;
    }

    void PlateSegmentation::templateMatchFinding(const cv::Mat &respones,int windowsWidth,std::pair<float,std::vector<int>> &candidatePts){
        int rows = respones.rows;
        int cols = respones.cols;



        float *data = (float*)respones.data;
        float *engNum_prob = data;
        float *false_prob = data+cols;
        float *ch_prob = data+cols*2;

        avgfilter(engNum_prob,cols,5);
        avgfilter(false_prob,cols,5);
//        avgfilter(ch_prob,cols,5);
        std::vector<int> candidate_pts(7);
#ifdef DEBUG
        drawHist(engNum_prob,cols,"engNum_prob");
        drawHist(false_prob,cols,"false_prob");
        drawHist(ch_prob,cols,"ch_prob");
                cv::waitKey(0);
#endif




        int cp_list[7];
        float loss_selected = -10;

        for(int start = 0 ; start < 20 ; start+=2)
            for(int  width = windowsWidth-5; width < windowsWidth+5 ; width++ ){
                for(int interval = windowsWidth/2; interval < windowsWidth; interval++)
                {
                    int cp1_ch  = start;
                    int cp2_p0 = cp1_ch+ width;
                    int cp3_p1 = cp2_p0+ width + interval;
                    int cp4_p2 = cp3_p1 + width;
                    int cp5_p3 = cp4_p2 + width+1;
                    int cp6_p4 = cp5_p3 + width+2;
                    int cp7_p5= cp6_p4+ width+2;

                    int md1 = (cp1_ch+cp2_p0)>>1;
                    int md2 = (cp2_p0+cp3_p1)>>1;
                    int md3 = (cp3_p1+cp4_p2)>>1;
                    int md4 = (cp4_p2+cp5_p3)>>1;
                    int md5 = (cp5_p3+cp6_p4)>>1;
                    int md6 = (cp6_p4+cp7_p5)>>1;




                    if(cp7_p5>=cols)
                        continue;
//                    float loss = ch_prob[cp1_ch]+
//                       engNum_prob[cp2_p0] +engNum_prob[cp3_p1]+engNum_prob[cp4_p2]+engNum_prob[cp5_p3]+engNum_prob[cp6_p4] +engNum_prob[cp7_p5]
//                    + (false_prob[md2]+false_prob[md3]+false_prob[md4]+false_prob[md5]+false_prob[md5] + false_prob[md6]);
                    float loss = ch_prob[cp1_ch]*3 -(false_prob[cp3_p1]+false_prob[cp4_p2]+false_prob[cp5_p3]+false_prob[cp6_p4]+false_prob[cp7_p5]);

                    if(loss>loss_selected)
                    {
                        loss_selected = loss;
                        cp_list[0]= cp1_ch;
                        cp_list[1]= cp2_p0;
                        cp_list[2]= cp3_p1;
                        cp_list[3]= cp4_p2;
                        cp_list[4]= cp5_p3;
                        cp_list[5]= cp6_p4;
                        cp_list[6]= cp7_p5;
                    }
                }
            }
        candidate_pts[0] = cp_list[0];
        candidate_pts[1] = cp_list[1];
        candidate_pts[2] = cp_list[2];
        candidate_pts[3] = cp_list[3];
        candidate_pts[4] = cp_list[4];
        candidate_pts[5] = cp_list[5];
        candidate_pts[6] = cp_list[6];

        candidatePts.first = loss_selected;
        candidatePts.second = candidate_pts;

    };


    void PlateSegmentation::segmentPlateBySlidingWindows(cv::Mat &plateImage,int windowsWidth,int stride,cv::Mat &respones){


//        cv::resize(plateImage,plateImage,cv::Size(136,36));

        cv::Mat plateImageGray;
        cv::cvtColor(plateImage,plateImageGray,cv::COLOR_BGR2GRAY);
        int padding  =  plateImage.cols-136 ;
//        int padding  =  0 ;
        int height = plateImage.rows - 1;
        int width = plateImage.cols - 1 - padding;
        for(int i = 0 ; i < width - windowsWidth +1 ; i +=stride)
        {
            cv::Rect roi(i,0,windowsWidth,height);
            cv::Mat roiImage = plateImageGray(roi);
            cv::Mat response = classifyResponse(roiImage);
            respones.push_back(response);
        }




        respones =  respones.t();
//        std::pair<float,std::vector<int>> images ;
//
//
//        std::cout<<images.first<<" ";
//        for(int i = 0 ; i < images.second.size() ; i++)
//        {
//            std::cout<<images.second[i]<<" ";
////            cv::line(plateImageGray,cv::Point(images.second[i],0),cv::Point(images.second[i],36),cv::Scalar(255,255,255),1); //DEBUG
//        }

//        int w = images.second[5] - images.second[4];

//        cv::line(plateImageGray,cv::Point(images.second[5]+w,0),cv::Point(images.second[5]+w,36),cv::Scalar(255,255,255),1); //DEBUG
//        cv::line(plateImageGray,cv::Point(images.second[5]+2*w,0),cv::Point(images.second[5]+2*w,36),cv::Scalar(255,255,255),1); //DEBUG


//        RefineRegion(plateImageGray,images.second,5);

//        std::cout<<w<<std::endl;

//        std::cout<<<<std::endl;

//        cv::resize(plateImageGray,plateImageGray,cv::Size(600,100));



    }

//    void filterGaussian(cv::Mat &respones,float sigma){
//
//    }


    void PlateSegmentation::segmentPlatePipline(PlateInfo &plateInfo,int stride,std::vector<cv::Rect> &Char_rects){
        cv::Mat plateImage = plateInfo.getPlateImage(); // get src image .
        cv::Mat plateImageGray;
        cv::cvtColor(plateImage,plateImageGray,cv::COLOR_BGR2GRAY);
        //do binarzation
        //
        std::pair<float,std::vector<int>> sections ; // segment points variables .

        cv::Mat respones; //three response of every sub region from origin image .
        segmentPlateBySlidingWindows(plateImage,DEFAULT_WIDTH,1,respones);
        templateMatchFinding(respones,DEFAULT_WIDTH/stride,sections);
        for(int i = 0; i < sections.second.size() ; i++)
        {
            sections.second[i]*=stride;

        }

//        std::cout<<sections<<std::endl;

        refineRegion(plateImageGray,sections.second,5,Char_rects);
#ifdef DEBUG
        for(int i = 0 ; i < sections.second.size() ; i++)
        {
            std::cout<<sections.second[i]<<" ";
            cv::line(plateImageGray,cv::Point(sections.second[i],0),cv::Point(sections.second[i],36),cv::Scalar(255,255,255),1); //DEBUG
        }
        cv::imshow("plate",plateImageGray);
        cv::waitKey(0);
#endif
//        cv::waitKey(0);

    }

    void PlateSegmentation::ExtractRegions(PlateInfo &plateInfo,std::vector<cv::Rect> &rects){
        cv::Mat plateImage = plateInfo.getPlateImage();
        for(int i = 0 ; i < rects.size() ; i++){
            cv::Mat charImage;
            plateImage(rects[i]).copyTo(charImage);
            if(charImage.channels())
                cv::cvtColor(charImage,charImage,cv::COLOR_BGR2GRAY);
//            cv::imshow("image",charImage);
//            cv::waitKey(0);
            cv::equalizeHist(charImage,charImage);
//

//


            std::pair<CharType,cv::Mat> char_instance;
            if(i == 0 ){

                char_instance.first = CHINESE;


            } else if(i == 1){
                char_instance.first = LETTER;
            }
            else{
                char_instance.first = LETTER_NUMS;
            }
            char_instance.second = charImage;
            plateInfo.appendPlateChar(char_instance);

        }

    }

}//namespace pr
