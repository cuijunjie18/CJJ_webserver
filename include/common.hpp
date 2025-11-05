#ifndef COMMON_HPP
#define COMMON_HPP

// 事件触发模式
enum{
    LT_TRIGMODE = 0,
    ET_TRIGMODE = 1,
};

// 事件处理模式
enum{
    Proactor_Mode = 0,
    Reactor__Mode = 1,
};

// http请求的状态
enum{
    Read_State = 0,
    Write_State = 1,
};

// 管道的写入与读入端
enum{
    Read_End = 0,
    Write_End = 1,
};

#endif