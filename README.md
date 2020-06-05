![logo_t](./demo_images/logo.png)

## HyperLPR   也许是最好的车牌识别开源框架

#### [![1](https://badge.fury.io/py/hyperlpr.svg "title")](https://pypi.org/project/hyperlpr/)[![1](https://img.shields.io/pypi/pyversions/hyperlpr.svg "title")](https://pypi.org/project/hyperlpr/)

###### 支持python3,支持Windows  Mac Linux 树莓派等。

#### 快速上手
```cmd
cd $HYPERLPR_HOME
cd hyperlpr_pip_pkg
python demo.py
```
#### Q&A

Q：v2分支跟master分支的区别？

A：v2采用轻量化ssd车牌定位模型，辅以轻量化的车牌矫正和端到端识别模型，全部模型仅十几M，还支持双层黄牌的识别。

Q：车牌的训练数据来源？

A：开放较为大的数据集有[CCPD](https://github.com/detectRecog/CCPD)车牌数据集。

Q：效果不好？

A：不要拿开源代码跟商用产品比较，不公平，再问就是下次更新，次更新，更新，新。。。


### 更新

- 更新Linux实现，增加双层车牌支持（2020.6.2）


### TODO

- 轻量化模型安卓实现
- 新能源车牌支持优化
- 更多车牌类型支持

### 特性

- 速度快 720p,单核 Intel 2.2G CPU (MaBook Pro 2015)平均识别时间低于100ms
- 基于端到端的车牌识别无需进行字符分割
- 识别率高,卡口场景准确率在95%-97%左右
- 轻量,总代码量不超1k行

### 模型资源说明

- cascade_double.xml  检测模型 - 目前效果最好的cascade检测模型
- mininet_ssd_v1.caffemodel  轻量化车牌定位模型
- refinenet.caffemodel 车牌区域矫正模型
- HorizonalFinemapping.caffemodel 角度矫正模型
- SegmenationFree-Inception.caffemodel 基于CNN的序列识别模型

### 注意事项:

- Win工程中若需要使用静态库，需单独编译
- 本项目的C++实现和Python实现尽量一致，发现不一致的欢迎issue或者pr
- 在编译C++工程的时候必须要使用OpenCV 3.4.3以上版本 (DNN 库)，否则无法编译 
- 安卓工程编译ndk尽量采用14b版本

### Python 依赖

- OpenCV(>3.4.3.18)

### CPP 依赖

- Opencv 3.4.3 以上版本

### Linux 编译

- 仅需要的依赖OpenCV 3.4.3 (需要DNN框架)

```bash
cd Prj-Linux
mkdir build 
cd build
cmake ../
sudo make -j 
```

###  

### 可识别和待支持的车牌的类型

#### 支持
- [x] 单行蓝牌
- [x] 单行黄牌
#### 有限支持
- [x] 新能源车牌
- [x] 白色警用车牌
- [x] 使馆/港澳车牌
- [x] 教练车牌
- [x] 双层黄牌
#### 待支持
- [ ] 武警车牌
- [ ] 民航车牌
- [ ] 双层武警
- [ ] 双层军牌
- [ ] 双层农用车牌
- [ ] 双层个性化车牌

###### Note:由于训练的时候样本存在一些不均衡的问题,一些特殊车牌存在一定识别率低下的问题，会在后续的版本进行改进。

- HyperLPR讨论QQ群1: 673071218, 群2: 746123554(已满) ,加前请备注HyperLPR交流,高精度版本商务合作可在群内联系。

### 作者和贡献者信息：

##### 作者昵称不分前后

- Jack Yu 作者(jack-yu-business@foxmail.com / https://github.com/szad670401)
- lsy17096535 整理(https://github.com/lsy17096535)
- xiaojun123456 IOS贡献(https://github.com/xiaojun123456)
- sundyCoder Android第三方贡献(https://github.com/sundyCoder)
- coleflowers php贡献(@coleflowers)
- Free&Easy 资源贡献 
- 海豚嘎嘎 LBP cascade检测器训练
- Windows工程端到端模型 (https://github.com/SalamanderEyes)
- Android实时扫描实现 (https://github.com/lxhAndSmh)
