# 日志系统

基于生产者——消费者模型的异步日志系统，使用阻塞队列作为数据结构

具体实现参考文章： https://mp.weixin.qq.com/s/f-ujwFyCe1LZa3EB561ehA

## 收获

### 条件变量使用细节

对于多个消费者，注意假唤醒现象，使用while
```cpp
// 对于多消费者使用while，因为唤醒后可能条件立刻又不满足了，防止假唤醒
while (que.empty()){
    if (!b_cond.wait(b_lock.get())){ // 调用失败
        b_lock.unlock();
        return false;
    }
}
```

### 可接受多个参数的函数

在 C/C++ 函数声明中，... 表示这是一个可变参数函数（variadic function），即可以接受可变数量参数的函数

简单例子
```cpp
void write_log(int level, const char *format, ...);
// 基本调用（无额外参数）
write_log(INFO, "Application started");

// 带一个参数
write_log(WARNING, "File %s not found", filename);

// 带多个参数
write_log(ERROR, "Connection failed: %s:%d (code %d)", host, port, error_code);

```

### 可变参数宏__VA_ARGS__使用

如果想在宏定义中使用可变参数函数，需要使用__VA_ARGS__

**它是用于在可变参数宏（variadic macros） 中表示可变参数部分.**

```cpp
//最简单的定义
#define my_print1(...)  printf(__VA_ARGS__)

//搭配va_list的format使用
#define my_print2(format, ...) printf(format, __VA_ARGS__)  
#define my_print3(format, ...) printf(format, ##__VA_ARGS__)
```

__VA_ARGS__宏前面加上##的作用在于，当可变参数的个数为0时，这里printf参数列表中的的##会把前面多余的","去掉，否则会编译出错，建议使用后面这种，使得程序更加健壮.

**AI生成的使用示例**
```cpp
#include <stdio.h>

// 基本定义 - 直接传递所有参数
#define LOG_BASIC(...) printf(__VA_ARGS__)

// 带固定格式前缀 - 可能有尾部逗号问题
#define LOG_PREFIXED(format, ...) printf("[LOG] " format, __VA_ARGS__)

// 使用 ## 解决尾部逗号问题
#define LOG_SAFE(format, ...) printf("[LOG] " format, ##__VA_ARGS__)

// 更复杂的例子 - 带时间戳和日志级别
#define LOG_DEBUG(format, ...) printf("[DEBUG][%s] " format, __TIMESTAMP__, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) fprintf(stderr, "[ERROR][%s] " format, __TIMESTAMP__, ##__VA_ARGS__)

// 条件编译的日志宏
#ifdef DEBUG
    #define DEBUG_LOG(format, ...) printf("[DEBUG] " format, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(format, ...) ((void)0) // 空操作
#endif

int main() {
    int count = 5;
    char* name = "Alice";
    float value = 3.14f;
    
    // 基本使用
    LOG_BASIC("Basic log: Count = %d, Name = %s\n", count, name);
    
    // 带前缀的使用 - 注意这里需要至少一个参数
    LOG_PREFIXED("Prefixed: Value = %.2f\n", value);
    
    // 安全版本 - 可以处理无额外参数的情况
    LOG_SAFE("Safe log with no extra args\n");
    LOG_SAFE("Safe log with args: %d, %s\n", count, name);
    
    // 带时间戳的日志
    LOG_DEBUG("Debug message: %s has %d items\n", name, count);
    LOG_ERROR("Error occurred with value: %.2f\n", value);
    
    // 条件日志
    DEBUG_LOG("This will only print in debug mode: %s\n", name);
    
    return 0;
}
```

### strchr()与strrchr()使用

strchr()查找字符串中第一次一次出现的指定字符的位置  
strrchr()查找字符串中最后一次出现的指定字符的位置