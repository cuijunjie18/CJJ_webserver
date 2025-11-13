#ifndef COMMON_HPP
#define COMMON_HPP

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

// http请求的状态
enum HttpRequestState{
    Read_State = 0,
    Write_State = 1,
};

// 管道的写入与读入端
enum PipeEnd{
    Read_End = 0,
    Write_End = 1,
};

#endif