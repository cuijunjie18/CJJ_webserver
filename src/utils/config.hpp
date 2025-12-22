#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <cstdlib>
#include "utils/common.hpp"

class WebConfig
{
public:
    WebConfig() {
        // 从环境变量读取MySQL主机地址，默认为localhost
        const char* mysql_host = std::getenv("MYSQL_HOST");
        if (mysql_host != nullptr) {
            mysql_url = mysql_host;
        }
        
        // 从环境变量读取MySQL端口，默认为3306
        const char* mysql_port_env = std::getenv("MYSQL_PORT");
        if (mysql_port_env != nullptr) {
            mysql_port = std::atoi(mysql_port_env);
        }
    }
    
    // 数据库配置
    std::string mysql_url = "localhost"; // MySQL服务器地址
    std::string user = "webdev"; 
    std::string passWord = "12345678"; 
    std::string databaseName = "user_info_db";
    int mysql_port = 3306; // MySQL端口

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
