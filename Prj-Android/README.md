HyperLPR Android Demo集成指南

Prj-Android 是 HyperLPR的一部分，集成时需要引入HyperLPR中的依赖文件以及环境

主要依赖如下  a、模型文件  b、opencv依赖库  c、NDK

集成方法如下：

a、引入模型文件：请到HyperLRP中的Linux项目下拷贝模型model目录下的模型文件（https://github.com/zeusees/HyperLPR/tree/master/Prj-Linux/lpr/model），
   到Android项目下的asset/pr目录中（如果没有pr，请自行建立，如果拷贝不正确，可能会出现模型文件找不到的问题。
   
b、opencv，如果opencv没有配置好，请按如下方法配置：

   1、项目依赖opencv，版本为opencv3.4+，请自行下载，
   
   2、改完之后，请在cpp/Cmakelist.xml中把配置中opencv路径改为指向自己机器的路径。
   
   
   
   
   
   注意运行过程中可能出现权限问题，请开发者自行解决。
   
   
   
  c、NDK，项目依赖的NDK版本为ndk16、请开发者自行修改为自己的NDK路径
   

   
 
