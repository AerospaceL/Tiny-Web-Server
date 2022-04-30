# Tiny-Web-Server
Linux/C++下的微型服务器， 在学习网络编程后实践的一个小项目。
+ 使用**线程池** + **非阻塞socket** + **epoll边沿触发** + **Reactor**的并发模型
+ 使用**状态机**和**正则匹配**解析HTTP请求报文，支持解析**POST**和**GET**请求
+ 为了提升读写性能，使用自动增长的**缓冲区**
+ 使用**时间堆**处理定时任务，关闭非活动连接
+ 可以请求图片和视频文件
+ 经webbench测试可以实现**上万**的并发连接数据交换
+ 使用了简洁的**C++11**实现

框架
---
！图片

压力测试
---
```
./webbench-1.5/webbench -c 100 -t 10 http://ip:port/
./webbench-1.5/webbench -c 1000 -t 10 http://ip:port/
./webbench-1.5/webbench -c 10000 -t 10 http://ip:port/
```

运行方式
---
```
mkdir bin
make
cd bin
./tiny-web-server
```

细致解析
---
反应堆模型：参见[@qinguoyi](https://github.com/qinguoyi/TinyWebServer)的庖丁解牛  
时间堆：参见Linux高效能服务器中的实现  
线程池：参见[@mtrebi](https://github.com/mtrebi/thread-pool)的简洁实现  
关于各个模块的实现细节，本人会在后续更新  

待完善部分
---
+ 实现用户的登录和注册功能
+ 实现日志功能
+ 模拟Proactor模型的实现
+ HTTP协议的其他部分功能

致谢
---
Linux高性能服务器编程，游双著.   
[@qinguoyi](https://github.com/qinguoyi/TinyWebServer)   
[@markparticle](https://github.com/markparticle/WebServer/)  
[@mtrebi](https://github.com/mtrebi/thread-pool)  

