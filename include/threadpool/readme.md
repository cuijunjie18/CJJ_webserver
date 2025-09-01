# 线程池

## 亮点

- 使用范式编程，线程池支持不同任务类型的连接.

## 收获

### new 与 delete

注意： new后的内存不会自动释放，必须手动调用delete释放，否则会导致内存泄露.

```cpp
// 单个new
int *p = new int;
*p = 10;
delete p; // 释放指向的内存
p = nullptr; // 指空，防止误用

// 数组new
int *p = new int[10]; // 分配10个sizeof(int)的内存
delete[] p; // 删除方式
```

但是最好不要用new,用的话要记住：谁 new，谁 delete

更高级的c++版本可用智能指针等资源管理工具.

### pthread_detach()使用

可以避免使用pthread_join()等待线程结束回收资源，而是在自己对应的线程结束后自动释放.