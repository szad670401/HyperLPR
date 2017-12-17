//
//  ViewController.m
//  lp
//
//  Created by xiaojun on 2017/12/2.
//  Copyright © 2017年 xiaojun. All rights reserved.
//



#import "ViewController.h"
#import "UIImageCVMatConverter.h"

#import "Pipeline.h"

using namespace pr;


@interface ViewController ()

@end

@implementation ViewController

@synthesize imageView;
@synthesize textView;
@synthesize toolbar;
@synthesize textLabel;

-(NSString *)getPath:(NSString*)fileName{
    NSString *bundlePath = [[NSBundle mainBundle].resourcePath stringByAppendingPathComponent:@"model.bundle"];
    NSString *path = [bundlePath stringByAppendingPathComponent:fileName];
    return path;
}

-(void)simpleRecognition:(cv::Mat&)src{
    
    NSString *path_1 = [self getPath:@"cascade.xml"];
    NSString *path_7 = [self getPath:@"CharacterRecognization.caffemodel"];
    NSString *path_6 = [self getPath:@"CharacterRecognization.prototxt"];
    NSString *path_3 = [self getPath:@"HorizonalFinemapping.caffemodel"];
    NSString *path_2 = [self getPath:@"HorizonalFinemapping.prototxt"];
    NSString *path_5 = [self getPath:@"Segmentation.caffemodel"];
    NSString *path_4 = [self getPath:@"Segmentation.prototxt"];
    
    std::string *cpath_1 = new std::string([path_1 UTF8String]);
    std::string *cpath_2 = new std::string([path_2 UTF8String]);
    std::string *cpath_3 = new std::string([path_3 UTF8String]);
    std::string *cpath_4 = new std::string([path_4 UTF8String]);
    std::string *cpath_5 = new std::string([path_5 UTF8String]);
    std::string *cpath_6 = new std::string([path_6 UTF8String]);
    std::string *cpath_7 = new std::string([path_7 UTF8String]);
    
    
    PipelinePR pr2 = PipelinePR::PipelinePR(*cpath_1,*cpath_2,*cpath_3,*cpath_4,*cpath_5,*cpath_6,*cpath_7);
    
    std::vector<pr::PlateInfo> list_res = pr2.RunPiplineAsImage(src);
    std::string concat_results="";
    for(auto one:list_res){
        if(one.confidence>0.7){
            concat_results += one.getPlateName()+",";
            //std::cout<<"1-----"+one.getPlateName()+"----1";
        }
    }
    //std::cout<<"2--"+concat_results+"--2";
    
    NSString *str=[NSString stringWithCString:concat_results.c_str() encoding:NSUTF8StringEncoding];
    if(str.length > 0){
        str = [str substringToIndex:str.length-1];
        str = [NSString stringWithFormat:@"result:%@",str];
    }else{
        str = [NSString stringWithFormat:@"result:null"];
    }
    
    [self.textLabel setText:str];
    
    
    
}


- (void)imagePickerController:(UIImagePickerController*)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    [picker dismissViewControllerAnimated:YES completion:nil];
    
    UIImage* temp = [info objectForKey:@"UIImagePickerControllerOriginalImage"];
    
    
    
    
    UIImage *temp_image=[UIImageCVMatConverter scaleAndRotateImageBackCamera:temp];
    source_image=[UIImageCVMatConverter cvMatFromUIImage:temp_image];
    [self simpleRecognition:source_image];
    imageView.image = temp;
    
    
    
    
    //cv::Mat cvImage,cv2;
    //UIImageToMat(temp, cvImage);
//    if(!cvImage.empty()){
//        cv::Mat gray;
//        // 将图像转换为灰度显示
//        cv::cvtColor(cvImage,gray,CV_RGB2GRAY);
//        // 应用高斯滤波器去除小的边缘
//        cv::GaussianBlur(gray, gray, cv::Size(5,5), 1.2,1.2);
//        // 计算与画布边缘
//        cv::Mat edges;
//        cv::Canny(gray, edges, 0, 50);
//        // 使用白色填充
//        cvImage.setTo(cv::Scalar::all(225));
//        // 修改边缘颜色
//        cvImage.setTo(cv::Scalar(0,128,255,255),edges);
//        // 将Mat转换为Xcode的UIImageView显示
//        self.imageView.image = MatToUIImage(cvImage);
//    }
    
    
    
    
    
}

-(void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissViewControllerAnimated:YES completion:nil];
}


- (void)loadButtonPressed:(id)sender
{
    
    UIImagePickerController* picker = [[UIImagePickerController alloc] init];
    picker.delegate = self;
    
    if (![UIImagePickerController isSourceTypeAvailable:
          UIImagePickerControllerSourceTypePhotoLibrary])
        return;
    
    picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    
    [self presentViewController:picker animated:YES completion:nil];
    
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    
    
    CGRect bounds = [UIScreen mainScreen].bounds;
    imageView = [[UIImageView alloc] init];
    imageView.frame = CGRectMake(0, 160, bounds.size.width, bounds.size.height-210);
    imageView.contentMode=UIViewContentModeScaleAspectFit;
    imageView.backgroundColor = [UIColor clearColor];
    [self.view addSubview:imageView];
    
    
    /* Add the fps Label */
    UILabel *fps = [[UILabel alloc] initWithFrame:CGRectMake(20, 120, 180, 20)];
    fps.font=[UIFont fontWithName:@"华文细黑" size:14.0f];
    fps.backgroundColor=[UIColor clearColor];
    fps.textColor=[UIColor redColor];
    fps.textAlignment=NSTextAlignmentLeft;
    // fps.transform = CGAffineTransformMakeRotation(90);
    fps.text=@"result";
    self.textLabel = fps;
    [self.view addSubview:self.textLabel];
    [self.view bringSubviewToFront:self.textLabel];
    
    toolbar=[[UIToolbar alloc] initWithFrame:CGRectMake(0, bounds.size.height- 44, bounds.size.width, 44)];
    [toolbar setBackgroundColor:[UIColor clearColor]];
    //   toolbar.barStyle=UIBarStyleDefault;
    toolbar.tintColor=[UIColor blackColor];
    toolbar.translucent=YES;
    //   [toolbar setTranslucent:YES];
    [self.toolbar setBackgroundImage:[UIImage new]
                  forToolbarPosition:UIBarPositionAny
                          barMetrics:UIBarMetricsDefault];
    toolbar.delegate=self;
    
  
    UIBarButtonItem*flexitem=[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];
    
    UIBarButtonItem*albumitem=[[UIBarButtonItem alloc]
                               
                               initWithTitle:@"相册"
                               style:UIBarButtonItemStylePlain
                               
                               target:self
                               
                               action:@selector(loadButtonPressed:)];
    
    
   
    
    [toolbar setItems:[NSArray arrayWithObjects:albumitem,flexitem,nil]];
    [self.view addSubview:toolbar];
    
    // Do any additional setup after loading the view, typically from a nib
    toolbar.autoresizingMask = UIViewAutoresizingNone;
    
    
    
    
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
