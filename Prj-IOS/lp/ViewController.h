//
//  ViewController.h
//  lp
//
//  Created by xiaojun on 2017/12/2.
//  Copyright © 2017年 xiaojun. All rights reserved.
//

//#import <UIKit/UIKit.h>

#ifdef __cplusplus
#import <opencv2/opencv.hpp>

//#import <opencv2/imgproc/types_c.h>
#import <opencv2/imgcodecs/ios.h>
#endif

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#endif

@interface ViewController : UIViewController<UIImagePickerControllerDelegate,
UINavigationControllerDelegate,UIToolbarDelegate> {
    
    cv::Mat source_image;
}

@property (nonatomic, retain) UILabel *textLabel;
@property (nonatomic, retain)  UIImageView *imageView;
@property (nonatomic, retain)  UIImageView *textView;
@property (nonatomic, retain)  UIToolbar *toolbar;

-(void)loadButtonPressed:(id)sender;
-(void)simpleRecognition:(cv::Mat&)src;
-(NSString *)getPath:(NSString *)fileName;

@end

