#ifndef COMMON_HPP
#define COMMON_HPP

#include <set>

// 事件触发模式
enum EventTriggerMode{
    LT_TRIGMODE = 0,
    ET_TRIGMODE = 1,
};

// 事件处理模式
enum EventProcessMode{
    Proactor_Mode = 0,
    Reactor_Mode = 1,
};

// 事件完成情况
enum EventProcessState{
    Event_Processing = 0,
    Event_Finish = 1,
};

// http请求的状态
enum HttpRequestState{
    Read_State = 0,
    Write_State = 1,
};

// 日志等级
enum LogLevel{
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
};

// 日志关闭
enum LogClose{
    Log_Open = 0,
    Log_Close = 1,
};

// 日志同步异步
enum LogWriteMode{
    Log_Sync = 0,
    Log_Async = 1,
};

// 管道的写入与读入端
enum PipeEnd{
    Read_End = 0,
    Write_End = 1,
};

// 监听socket的优雅关闭选项
enum OptLinger{
    Linger_Close = 0,
    Linger_Open = 1,
};

// 是否销毁定时器
enum TimerState{
    TimerDestroy = 0,
    TimerRemain = 1,
};

#endif