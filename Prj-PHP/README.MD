
# Prj-PHP

HyperLPR 在PHP扩展程序中的实现，核心代码拷贝了 Prj-Linux 中庾金科大牛的代码。我做的这部分工作主要是配置编译成PHP扩展程序。

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
$path = realpath("demo.png"); 
$res = platescan($path);
var_dump($res); // string(9) "苏ED0N19"

```

最终实现上边这样的PHP调用

![image](./tests/demo.png)
