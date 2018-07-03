
# Prj-PHP

HyperLPR 在PHP扩展程序中的实现，核心代码拷贝了 Prj-Linux 中的代码。

## CPP 依赖

+ Opencv 3.3
+ PHPCPP

## Linux/Mac 编译

```bash
mkdir build
cd build
cmake ../
make -j
```

## DEMO

```php
// 也可编译完后直接在tests目录直接运行 php platescan.php
$path = realpath("demo.png");   // 图片文件所在路径,realpath转为绝对路径
$model = realpath("model");     // 模型资源所在文件夹
$res = platescan($path, $model, 0.8);
var_dump($res); // string(9) "苏ED0N19"

```

最终实现上边这样的PHP调用

![image](./tests/demo.png)

## 测试备忘

经多次测试发现部分图片难以识别或者容易识别错误，经过筛选反推出如下规律：

### 识别精度高的图片

+ 图片中包含有整个车身
+ 图片清晰度较高，不模糊
+ 图片拍摄角度为正面拍摄，车牌在图片中没有较大幅度的倾斜
+ 图片中的车牌在整个图片中所占位置较小

### 识别精度低的图片

+ 图片模糊，程序未能裁切出正确的车牌，或者裁切后因图片太模糊而无法识别（此处应该可以通过训练更多样本解决）
+ 图片中的车牌在整个图片中占了1/3的位置甚至更高
+ 图片拍摄角度为俯角拍摄，车牌在图片中是倾斜着的
