qt版本6.7.2
本项目基于OpenCV和dlib
使用C/S架构 网络通信使用UDP
采用C++ QML联合编程
数据库使用MySQL
本项目代码量比较少 主要是我用来学习qml的

!!!
主要依靠dlib的68点人脸特征提取模型实现 不是深度学习 所以鲁棒性不强,准确度不高！光照表情脸部遮挡等条件都会对结果产生影响！
没有活体检测功能
只实现了基本的功能 还有很多地方可以优化

!!!
需要自行配置OpenCV和dlib 自行下载dlib模型
自行解决MySQL驱动问题
另外还有一个qsb文件如果出现问题需要自行编译
