# High Accuracy Chinese Plate Recognition Framework

### 介绍
This research aims at simply developping plate recognition project based on deep learning methods, with low complexity and high speed. This 
project has been used by some commercial corporations. Free and open source, deploying by Zeusee. 

HyperLPR是一个使用深度学习针对对中文车牌识别的实现，与较为流行的开源的[EasyPR](https://github.com/liuruoze/EasyPR)相比，它的检测速度和鲁棒性和多场景的适应性都要好于目前开源的EasyPR。[在线测试地址](http://sftera.vicp.io:8000/uploader)。

[相关技术博客](http://blog.csdn.net/relocy/article/details/78705662)(技术文章会在接下来的几个月的时间内连续更新)。


### 更新

+ 添加端到端的序列识别模型识别率大幅度提升,使得无需分割字符即可识别,识别速度提高20% (2017.11.17)
+ 新增的端到端模型可以识别新能源车牌、教练车牌、白色警用车牌、武警车牌 (2017.11.17)
+ 更新Windows版本的Visual Studio 2015 工程（2017.11.15）
+ 增加cpp版本,目前仅支持标准蓝牌(需要依赖OpenCV 3.3) (2017.10.28)
+ 添加了简单的Android实现 (骁龙835 (*720*x*1280*)  200ms )(2017.10.28)

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

### 设计流程

> step1. 使用opencv 的 HAAR Cascade 检测车牌大致位置 
>
> step2. Extend 检测到的大致位置的矩形区域
>
> step3. 使用类似于MSER的方式的 多级二值化 + RANSAC 拟合车牌的上下边界
>
> step4. 使用CNN Regression回归车牌左右边界
>
> step5. 使用基于纹理场的算法进行车牌校正倾斜
>
> step6. 使用CNN滑动窗切割字符
>
> step7. 使用CNN识别字符

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



### 测试样例

![image](./demo_images/test.png)

![image](./demo_images/15.jpg)

#### Android示例

![android](./demo_images/android.png)

### 识别测试APP

- 体验 Android APP：[https://fir.im/HyperLPR](https://fir.im/HyperLPR)
- 感谢 sundyCoder [Android 版本完善](https://github.com/sundyCoder/hyperlpr4Android) 

### 数据分享

车牌识别框架开发时使用的数据并不是很多，有意着可以为我们提供相关车牌数据。联系邮箱 455501914@qq.com。

### 获取帮助

+ HyperLPR讨论QQ群：673071218, 加前请备注HyperLPR交流。
