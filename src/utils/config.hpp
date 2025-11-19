#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include "utils/common.hpp"

class WebConfig
{
public:
    // 数据库配置
    std::string user = "webdev"; 
    std::string passWord = "12345678"; 
    std::string databaseName = "user_info_db";

    int port = 50001; // 服务器端口 

    int log_write = Log_Sync;  // 同步日志
    int opt_linger = Linger_Open; // 开启优雅关闭socket
    int listen_trig_mode = LT_TRIGMODE; 
    int conn_trig_mode = LT_TRIGMODE;
    int sql_num = 8; 
    int thread_num = 8; 
    int close_log = Log_Open; // 日志开启状态 
    int actor_model = Proactor_Mode; // 服务器并发模式
};

#endif