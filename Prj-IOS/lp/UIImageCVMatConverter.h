//
//  UIImageCVMatConverter.h
//  lp
//
//  Created by xiaojun on 2017/12/2.
//  Copyright © 2017年 xiaojun. All rights reserved.
//

#ifndef UIImageCVMatConverter_h
#define UIImageCVMatConverter_h

#ifdef __cplusplus
#import <opencv2/opencv.hpp>
#endif
#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#endif
using namespace cv;

@interface UIImageCVMatConverter : NSObject

+ (cv::Mat)cvMatFromUIImage:(UIImage *)image;
+ (UIImage *)UIImageFromCVMat:(cv::Mat)image;
+ (UIImage *)scaleAndRotateImageFrontCamera:(UIImage *)image;
+ (UIImage *)scaleAndRotateImageBackCamera:(UIImage *)image;
+(UIImage*) imageWithMat:(const cv::Mat&) image andImageOrientation: (UIImageOrientation) orientation;
+(UIImage*) imageWithMat:(const cv::Mat&) image andDeviceOrientation: (UIDeviceOrientation) orientation;

@end

#endif /* UIImageCVMatConverter_h */
