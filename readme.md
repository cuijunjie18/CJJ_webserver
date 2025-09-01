# CJJ-WebSever

## 基本组件

- 线程同步包装类.
- 数据库连接池.
- 线程池.
- 定时器.
- 日志系统.
- http请求处理器.
- 注册登陆主件.
- 测试组件(预想)

## 环境配置

[MySQL数据库配置](https://github.com/cuijunjie18/my_notebook/blob/master/mysql)  

## 开发日志

- 2025.8.29
  - 完成线程同步机制的包装类，即lock功能包.
  - 初步搭建数据库连接池.(未实现具体函数)
  
- 2025.8.31
  - 完成数据库连接池搭建.(未添加日志系统、未测试)
  - 加上数据库连接池到main，测试编译成功.
  
- 2025.9.1
  - 完成线程池的搭建(未测试，大部分是copy from reference)
  - 学习i/o多路复用的相关理论，了解LT、ET事件触发模式.


## 收获

[线程同步](include/lock/readme.md)  
[数据库连接池](include/CGImysql/readme.md)  
[i/o多路复用](learning/Multiplexing/readme.md)  
[事件触发模式](learning/trigger_mode/readme.md)  

## 参考

baseline: https://github.com/qinguoyi/TinyWebServer.git  
mysql_cmake: https://github.com/nryeng/hello-mysql-world