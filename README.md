# 车牌识别

### Pipeline

step1. 使用opencv 的 HAAR cascade 检测车牌大致位置 

step2. Extend 检测到的大致位置的矩形区域

step3. 使用类似于mser的方式的多级二值化+ransac拟合车牌的上下边界

step4. 使用CNN regression回归车牌左右边界

step5. 使用CNN滑动窗切割字符

step6. 使用CNN识别字符

### 简单测试方式

```python
from hyperlpr import  pipline as  pp
import cv2
image = cv2.imread("filename")
image,res  = pp.SimpleRecognizePlate(image)
```
### 测试样例

![image](./cache/demo1.png)
![image](./cache/demo2.png)
![image](./cache/demo3.png)
