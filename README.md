# High Accuracy Chinese Plate Recognition Framework

### 介绍
This research aims at simply developping plate recognition project based on deep learning methods, with low complexity and high speed. This 
project has been used by some commercial corporations. Free and open source, deploying by Zeusee. 


##### 更新热点:

- 添加的新的Python 序列模型-识别率大幅提高(尤其汉字)(2018.3.12)
- 新添加了HyperLPR Lite 只需要一个文件 160行代码即可完全整个车牌识别流程.
- 提供精确定位的车牌矩形框(2018.3.12)	

#### 相关资源 

+ [相关技术博客](http://blog.csdn.net/relocy/article/details/78705662)(技术文章会在接下来的几个月的时间内连续更新)。
+ [带UI界面的工程](https://pan.baidu.com/s/1cNWpK6)(感谢群内小伙伴的工作)。
+ [端到端(多标签分类)训练代码](https://github.com/LCorleone/hyperlpr-train_e2e)(感谢群内小伙伴的工作)。
+ [端到端(CTC)训练代码](https://github.com/armaab/hyperlpr-train)(感谢群内小伙伴工作)。

### 更新

+ [增加PHP车牌识别工程@coleflowers](https://github.com/zeusees/HyperLPR/tree/master/Prj-PHP) (2018.06.20)
+ 添加的新的Python 序列模型-识别率大幅提高(尤其汉字)(2018.3.12)
+ 添加了HyperLPR Lite 仅仅需160 行代码即可实现车牌识别(2018.3.12)
+ 提供精确定位的车牌矩形框(2018.3.12)


+ 增加了端到端模型的cpp实现(Linux)(2018.1.31)


+ 增加字符分割[训练代码和字符分割介绍](https://github.com/zeusees/HyperLPR-Training)(2018.1.)
+ 更新了Android实现，大幅提高准确率和速度 (骁龙835 (*720*x*1280*)  ~50ms )(2017.12.27)
+ 添加了IOS版本的实现（感谢[xiaojun123456](https://github.com/xiaojun123456)的工作）
+ 添加端到端的序列识别模型识别率大幅度提升,使得无需分割字符即可识别,识别速度提高20% (2017.11.17)
+ 新增的端到端模型可以识别新能源车牌、教练车牌、白色警用车牌、武警车牌 (2017.11.17)
+ 更新Windows版本的Visual Studio 2015 工程（2017.11.15）
+ 增加cpp版本,目前仅支持标准蓝牌(需要依赖OpenCV 3.3) (2017.10.28)

### TODO

+ 提供字符字符识别的训练代码
+ 改进精定位方法
+ C++版的端到端识别模型

### 特性

+ 速度快 720p ，单核 Intel 2.2G CPU (macbook Pro 2015)平均识别时间低于100ms
+ 基于端到端的车牌识别无需进行字符分割
+ 识别率高,仅仅针对车牌ROI在EasyPR数据集上，0-error达到 95.2%, 1-error识别率达到 97.4% (指在定位成功后的车牌识别率)
+ 轻量 总代码量不超1k行

### 模型资源说明

+ cascade.xml  检测模型 - 目前效果最好的cascade检测模型
+ cascade_lbp.xml  召回率效果较好，但其错检太多
+ char_chi_sim.h5 Keras模型-可识别34类数字和大写英文字  使用14W样本训练 
+ char_rec.h5 Keras模型-可识别34类数字和大写英文字  使用7W样本训练 
+ ocr_plate_all_w_rnn_2.h5 基于CNN的序列模型
+ ocr_plate_all_gru.h5 基于GRU的序列模型从OCR模型修改，效果目前最好但速度较慢，需要20ms。
+ plate_type.h5 用于车牌颜色判断的模型
+ model12.h5 左右边界回归模型



### 注意事项:

+ Win工程中若需要使用静态库，需单独编译
+ 本项目的C++实现和Python实现无任何关联，都为单独实现
+ 在编译C++工程的时候必须要使用OpenCV 3.3(DNN 库)，否则无法编译 

### Python 依赖

+ Keras (>2.0.0)
+ Theano(>0.9) or Tensorflow(>1.1.x)
+ Numpy (>1.10)
+ Scipy (0.19.1)
+ OpenCV(>3.0)
+ Scikit-image (0.13.0)
+ PIL

### CPP 依赖

+ Opencv 3.3

### 简单使用方式

推荐使用新更新的HyperLPR Lite，仅需一单独文件。

```python
import HyperLPRLite as pr
import cv2
import numpy as np
grr = cv2.imread("images_rec/demo1.jpg")
model = pr.LPR("model/cascade.xml","model/model12.h5","model/ocr_plate_all_gru.h5")

def drawRectBox(image,rect,addText):
    cv2.rectangle(image, (int(rect[0]), int(rect[1])), (int(rect[0] + rect[2]), int(rect[1] + rect[3])), (0,0, 255), 2,cv2.LINE_AA)
    cv2.rectangle(image, (int(rect[0]-1), int(rect[1])-16), (int(rect[0] + 115), int(rect[1])), (0, 0, 255), -1,
                  cv2.LINE_AA)
    img = Image.fromarray(image)
    draw = ImageDraw.Draw(img)
    draw.text((int(rect[0]+1), int(rect[1]-16)), addText.decode("utf-8"), (255, 255, 255), font=fontC)
    imagex = np.array(img)
    return imagex
    
for pstr,confidence,rect in model.SimpleRecognizePlateByE2E(grr):
        if confidence>0.7:
            image = drawRectBox(grr, rect, pstr+" "+str(round(confidence,3)))
            print("plate_str",pstr)
            print("plate_confidence",confidence)


cv2.imshow("image",image)
cv2.waitKey(0)

```
### Linux/Mac 编译

+ 仅需要的依赖OpenCV 3.3 (需要DNN框架)

```bash
cd cpp_implementation
mkdir build 
cd build
cmake ../
sudo make -j 
```

### CPP demo

```cpp
#include "../include/Pipeline.h"
int main(){
    pr::PipelinePR prc("model/cascade.xml",
                      "model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel",
                      "model/Segmentation.prototxt","model/Segmentation.caffemodel",
                      "model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel",
                       "model/SegmentationFree.prototxt","model/SegmentationFree.caffemodel"
                    );
  //定义模型文件

    cv::Mat image = cv::imread("/Users/yujinke/ClionProjects/cpp_ocr_demo/test.png");
    std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image,pr::SEGMENTATION_FREE_METHOD);
  //使用端到端模型模型进行识别 识别结果将会保存在res里面
 
    for(auto st:res) {
        if(st.confidence>0.75) {
            std::cout << st.getPlateName() << " " << st.confidence << std::endl;
          //输出识别结果 、识别置信度
            cv::Rect region = st.getPlateRect();
          //获取车牌位置
 cv::rectangle(image,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),2);
          //画出车牌位置
          
        }
    }

    cv::imshow("image",image);
    cv::waitKey(0);
    return 0 ;
}
```

### 

### 可识别和待支持的车牌的类型

- [x] 单行蓝牌
- [x] 单行黄牌
- [x] 新能源车牌
- [x] 白色警用车牌
- [x] 使馆/港澳车牌
- [x] 教练车牌
- [x] 武警车牌
- [ ] 民航车牌
- [ ] 双层黄牌
- [ ] 双层武警
- [ ] 双层军牌
- [ ] 双层农用车牌
- [ ] 双层个性化车牌


###### Note:由于训练的时候样本存在一些不均衡的问题,一些特殊车牌存在一定识别率低下的问题，如(使馆/港澳车牌)，会在后续的版本进行改进。


### 测试样例

![image](./demo_images/test.png)

![image](./demo_images/15.jpg)

#### Android示例

![android](./demo_images/android.png)

### 识别测试APP

- 体验 Android APP：[https://fir.im/HyperLPR](https://fir.im/HyperLPR) (根据图片尺寸调整程序中的尺度，提高准确率)
- 感谢 sundyCoder [Android 版本完善](https://github.com/sundyCoder/hyperlpr4Android) 

### 数据分享

车牌识别框架开发时使用的数据并不是很多，有意着可以为我们提供相关车牌数据。联系邮箱 jack-yu-business@foxmail.com。


#### 获取帮助

+ HyperLPR讨论QQ群1:673071218(已满), 群2: 746123554 ,加前请备注HyperLPR交流。


### 作者和贡献者信息：
##### 作者昵称不分前后 
+ Jack Yu 作者(jack-yu-business@foxmail.com / https://github.com/szad670401)
+ lsy17096535 整理(https://github.com/lsy17096535)
+ xiaojun123456 IOS贡献(https://github.com/xiaojun123456)
+ sundyCoder Android第三方贡献(https://github.com/sundyCoder)
+ coleflowers php贡献(@coleflowers)
+ Free&Easy 资源贡献 
+ 海豚嘎嘎 LBP cascade检测器训练
