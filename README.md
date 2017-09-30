# High Accuracy Chinese Plate Recognition Framework

### 介绍
This research aims at simply developping plate recognition project based on deep learning methods, with low complexity and high speed. This 
project has been used by some commercial corporations. Free and open source, deploying by Zeusee. 

HyperLPR是一个基于Python的使用深度学习针对对中文车牌识别的实现，与开源的[EasyPR](https://github.com/liuruoze/EasyPR)相比，它的检测速度和鲁棒性和多场景的适应性都要好于EasyPR。


### 特性

+ 单张720p图像，单核Intel 2.2G CPU 识别时间140ms左右，比EasyPR单核识别速度快近10倍的时间。
+ 识别率在EasyPR数据集上0-error达到70.2% 1-error识别率达到 89.6%
+ 检测方法在实时性、召回率、准确率上都优于MSER方法，检测recall和easyPR持平。
+ 代码框架轻量，总代码不到1k行。

### 依赖

+ Keras (>2.0.0)
+ Theano(>0.9) or Tensorflow(>1.1.x)
+ Numpy (>1.10)
+ Scipy (0.19.1)
+ OpenCV(>3.0)
+ scikit-image (0.13.0)

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
```
### 可识别和待支持的车牌的类型

- [x] 单行蓝牌
- [x] 单行黄牌
- [ ] 新能源车牌
- [ ] 双层黄牌
- [ ] 双层武警
- [ ] 双层军牌
- [ ] 农用车牌
- [ ] 白色警用车牌
- [ ] 使馆/港澳车牌
- [ ] 民航车牌
- [ ] 个性化车牌



### 测试样例

hyperlpr_test文件夹下

![image](./demo_images/test.png)


### 数据分享

车牌识别框架开发时使用的数据并不是很多，有意着可以为我们提供相关车牌数据。联系邮箱 455501914@qq.com。

### 获取帮助

+ HyperLPR讨论QQ群：673071218, 加前请备注HyperLPR交流。
