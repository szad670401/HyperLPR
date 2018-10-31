//
//  Utility.h
//  lpr
//
//  Created by baotim on 2018/10/26.
//  Copyright © 2018 lprSample. All rights reserved.
//
#ifdef __cplusplus
#import <opencv2/opencv.hpp>
#endif
#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#endif
using namespace cv;

NS_ASSUME_NONNULL_BEGIN

@interface Utility : NSObject

+ (cv::Mat)cvMatFromUIImage:(UIImage *)image;
+ (UIImage *)UIImageFromCVMat:(cv::Mat)image;
+ (UIImage *)scaleAndRotateImageFrontCamera:(UIImage *)image;
+ (UIImage *)scaleAndRotateImageBackCamera:(UIImage *)image;
+ (UIImage *)imageWithMat:(const cv::Mat&) image andImageOrientation: (UIImageOrientation) orientation;
+ (UIImage *)imageWithMat:(const cv::Mat&) image andDeviceOrientation: (UIDeviceOrientation) orientation;

@end

NS_ASSUME_NONNULL_END
