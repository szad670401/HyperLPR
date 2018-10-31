//
//  CameraViewController.h
//  lpr
//
//  Created by baotim on 2018/10/26.
//  Copyright © 2018 lprSample. All rights reserved.
//

#ifdef __cplusplus
#import <opencv2/opencv.hpp>
#import <opencv2/imgcodecs/ios.h>
#endif

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CoreMedia/CoreMedia.h>
#endif

NS_ASSUME_NONNULL_BEGIN

typedef void(^ResultCallBack)(NSString *reuslt, UIImage *image);

@interface CameraViewController : UIViewController

@property(nonatomic, copy) ResultCallBack resultCB;

@end

NS_ASSUME_NONNULL_END
