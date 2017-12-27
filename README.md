# High Accuracy Chinese Plate Recognition Framework

### 介绍
This research aims at simply developping plate recognition project based on deep learning methods, with low complexity and high speed. This 
project has been used by some commercial corporations. Free and open source, deploying by Zeusee. 

HyperLPR是一个使用深度学习针对对中文车牌识别的实现，与较为流行的开源的[EasyPR](https://github.com/liuruoze/EasyPR)相比，它的检测速度和鲁棒性和多场景的适应性都要好于目前开源的EasyPR。[在线测试地址](http://sftera.vicp.io:8000/uploader)。

[相关技术博客](http://blog.csdn.net/relocy/article/details/78705662)(技术文章会在接下来的几个月的时间内连续更新)。

### 更新

+ 更新了Android实现，大幅提高准确率和速度 (骁龙835 (*720*x*1280*)  ~50ms )(2017.12.27)
+ 添加了IOS版本的实现（感谢[xiaojun123456](https://github.com/xiaojun123456)的工作）
+ 添加端到端的序列识别模型识别率大幅度提升,使得无需分割字符即可识别,识别速度提高20% (2017.11.17)
+ 新增的端到端模型可以识别新能源车牌、教练车牌、白色警用车牌、武警车牌 (2017.11.17)
+ 更新Windows版本的Visual Studio 2015 工程（2017.11.15）
+ 增加cpp版本,目前仅支持标准蓝牌(需要依赖OpenCV 3.3) (2017.10.28)

### TODO

+ 提供字符分割、字符识别的训练代码
+ 改进精定位方法
+ C++版的端到端识别模型

### 特性

+ 速度快 720p ，单核 Intel 2.2G CPU (macbook Pro 2015)平均识别时间低于100ms
+ 基于端到端的车牌识别无需进行字符分割
+ 识别率高,仅仅针对车牌ROI在EasyPR数据集上，0-error达到 95.2%, 1-error识别率达到 97.4% (指在定位成功后的车牌识别率)
+ 轻量 总代码量不超1k行

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

```python
from hyperlpr import  pipline as  pp
import cv2
image = cv2.imread("filename")
image,res  = pp.SimpleRecognizePlate(image)
print(res)
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

车牌识别框架开发时使用的数据并不是很多，有意着可以为我们提供相关车牌数据。联系邮箱 455501914@qq.com。

### 捐赠我们

***如果您愿意支持我们持续对这个框架的开发，可以通过下面的链接来对我们捐赠。***

**[支付宝/微信](http://chuantu.biz/t6/178/1513525003x-1404758184.png)**

#### 获取帮助

+ HyperLPR讨论QQ群：673071218, 加前请备注HyperLPR交流。

