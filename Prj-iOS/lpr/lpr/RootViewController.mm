//
//  RootViewController.m
//  lpr
//
//  Created by baotim on 2018/10/26.
//  Copyright © 2018 lprSample. All rights reserved.
//

#import "RootViewController.h"
#import <Masonry/Masonry.h>
#import "Utility.h"
#import "Pipeline.h"
#import "CameraViewController.h"

@interface RootViewController () <UIImagePickerControllerDelegate, UINavigationControllerDelegate>

@property (nonatomic, strong) UIButton* albumButton;
@property (nonatomic, strong) UIButton* cameraButton;
@property (nonatomic, strong) UIImageView* imageView;
@property (nonatomic, strong) UILabel* resultLabel;

@end

@implementation RootViewController
{
    cv::Mat source_image;
}

#pragma mark - Lazy Initialize
- (UIButton *)albumButton
{
    if (!_albumButton) {
        _albumButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [_albumButton setTitle:@"打开相册" forState:UIControlStateNormal];
        [_albumButton setBackgroundColor:[UIColor redColor]];
        [_albumButton addTarget:self action:@selector(openAlbum) forControlEvents:UIControlEventTouchUpInside];
    }
    return _albumButton;
}

- (UIButton *)cameraButton
{
    if (!_cameraButton) {
        _cameraButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [_cameraButton setBackgroundColor:[UIColor redColor]];
        [_cameraButton setTitle:@"实时拍照" forState:UIControlStateNormal];
        [_cameraButton addTarget:self action:@selector(openCamera) forControlEvents:UIControlEventTouchUpInside];
    }
    return _cameraButton;
}

- (UIImageView *)imageView
{
    if (!_imageView) {
        _imageView = [[UIImageView alloc] init];
        _imageView.contentMode = UIViewContentModeScaleAspectFit;
    }
    return _imageView;
}

- (UILabel *)resultLabel
{
    if (!_resultLabel) {
        _resultLabel = [UILabel new];
        _resultLabel.textColor = [UIColor redColor];
        _resultLabel.textAlignment = NSTextAlignmentLeft;
        _resultLabel.font = [UIFont systemFontOfSize:15];
    }
    return _resultLabel;
}

#pragma mark - Life Cycle
- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.view.backgroundColor = [UIColor whiteColor];
    self.navigationItem.title = @"车牌识别Demo";
    
    [self.view addSubview:self.albumButton];
    [self.albumButton mas_makeConstraints:^(MASConstraintMaker* make) {
        make.centerX.equalTo(self.view).offset(-80);
        make.bottom.equalTo(self.mas_bottomLayoutGuideTop).offset(-20);
        make.width.mas_equalTo(100);
        make.height.mas_equalTo(50);
    }];
    
    [self.view addSubview:self.cameraButton];
    [self.cameraButton mas_makeConstraints:^(MASConstraintMaker* make) {
        make.centerX.equalTo(self.view).offset(80);
        make.bottom.equalTo(self.albumButton);
        make.width.mas_equalTo(100);
        make.height.mas_equalTo(50);
    }];
    
    [self.view addSubview:self.resultLabel];
    [self.resultLabel mas_makeConstraints:^(MASConstraintMaker* make) {
        make.left.right.equalTo(self.view);
        make.bottom.lessThanOrEqualTo(self.albumButton.mas_top);
    }];
    
    [self.view addSubview:self.imageView];
    [self.imageView mas_makeConstraints:^(MASConstraintMaker* make) {
        make.top.left.right.equalTo(self.view);
        make.bottom.lessThanOrEqualTo(self.resultLabel.mas_top);
    }];
}

#pragma mark - Actions
- (void)openAlbum
{
    UIImagePickerController* picker = [[UIImagePickerController alloc] init];
    picker.delegate = self;
    
    if (![UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary])
        return;
    
    picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    
    [self presentViewController:picker animated:YES completion:nil];
}

- (void)openCamera
{
    CameraViewController* video = [CameraViewController new];
    video.resultCB = ^(NSString* text, UIImage* image) {
        self.imageView.image = image;
        self.resultLabel.text = text;
    };
    [self presentViewController:video animated:YES completion:nil];
}

#pragma mark - Private Methods
- (NSString *)getPath:(NSString*)fileName
{
    NSString *bundlePath = [NSBundle mainBundle].bundlePath;
    NSString *path = [bundlePath stringByAppendingPathComponent:fileName];
    return path;
}

- (void)simpleRecognition:(cv::Mat&)src
{
    NSString *path_1 = [self getPath:@"cascade.xml"];
    NSString *path_2 = [self getPath:@"HorizonalFinemapping.prototxt"];
    NSString *path_3 = [self getPath:@"HorizonalFinemapping.caffemodel"];
    NSString *path_4 = [self getPath:@"Segmentation.prototxt"];
    NSString *path_5 = [self getPath:@"Segmentation.caffemodel"];
    NSString *path_6 = [self getPath:@"CharacterRecognization.prototxt"];
    NSString *path_7 = [self getPath:@"CharacterRecognization.caffemodel"];
    
    std::string *cpath_1 = new std::string([path_1 UTF8String]);
    std::string *cpath_2 = new std::string([path_2 UTF8String]);
    std::string *cpath_3 = new std::string([path_3 UTF8String]);
    std::string *cpath_4 = new std::string([path_4 UTF8String]);
    std::string *cpath_5 = new std::string([path_5 UTF8String]);
    std::string *cpath_6 = new std::string([path_6 UTF8String]);
    std::string *cpath_7 = new std::string([path_7 UTF8String]);
    
    
    pr::PipelinePR pr2 = pr::PipelinePR(*cpath_1, *cpath_2, *cpath_3, *cpath_4, *cpath_5, *cpath_6, *cpath_7);
    
    std::vector<pr::PlateInfo> list_res = pr2.RunPiplineAsImage(src);
    std::string concat_results = "";
    for(auto one:list_res) {
        if(one.confidence>0.7) {
            concat_results += one.getPlateName()+",";
        }
    }
    
    NSString *str = [NSString stringWithCString:concat_results.c_str() encoding:NSUTF8StringEncoding];
    if (str.length > 0) {
        str = [str substringToIndex:str.length-1];
        str = [NSString stringWithFormat:@"识别结果: %@",str];
    } else {
        str = [NSString stringWithFormat:@"识别结果: 未识别成功"];
    }
    
    [self.resultLabel setText:str];
}

#pragma mark - UIImagePickerControllerDelegate
- (void)imagePickerController:(UIImagePickerController*)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    [picker dismissViewControllerAnimated:YES completion:nil];
    
    UIImage* temp = [info objectForKey:@"UIImagePickerControllerOriginalImage"];
    UIImage *temp_image = [Utility scaleAndRotateImageBackCamera:temp];
    source_image = [Utility cvMatFromUIImage:temp_image];
    [self simpleRecognition:source_image];
    self.imageView.image = temp;
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissViewControllerAnimated:YES completion:nil];
}

@end
