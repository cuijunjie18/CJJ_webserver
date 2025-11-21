#ifndef HTTP_CONN_HPP
#define HTTP_CONN_HPP

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <map>
#include <atomic>

#include "CGImysql/sql_connection_pool.hpp"
#include "logger/logger.hpp"
#include "timer/lst_timer.hpp"
#include "utils/common.hpp"
#include "utils/epoll_utils.hpp"

// 请求方法
enum METHOD {
    GET = 0,
    POST,
    HEAD,
    PUT,
    DELETE,
    TRACE,
    OPTIONS,
    CONNECT,
    PATH
};

// 响应状态码
enum HTTP_CODE {
    NO_REQUEST = 0, // 请求不完整
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURCE,
    FORBIDDEN_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

// 主状态机状态
enum CHECK_STATE {
    CHECK_STATE_REQUESTLINE = 0, // 请求行
    CHECK_STATE_HEADER, // 请求头
    CHECK_STATE_CONTENT // 请求主体
};
// 从状态机的状态
enum LINE_STATUS {
    LINE_OK = 0,
    LINE_BAD,
    LINE_OPEN   // 行不完整
};

class HttpConn
{
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;

public:
    HttpConn() {}
    ~HttpConn() {}

public:
    void init(int sockfd, 
        const sockaddr_in &addr, 
        char *, int, int, 
        std::string user, 
        std::string passwd, 
        std::string sqlname);
    void close_conn(bool real_close = true);
    void process();
    bool read_once();
    bool write();
    sockaddr_in *get_address()
    {
        return &m_address;
    }
    int timer_flag; // 定时器是否需要删除
    int improv; // 任务完成标记


private:
    void init();
    HTTP_CODE process_read(); // 读请求
    bool process_write(HTTP_CODE ret);
    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();
    char *get_line() { return m_read_buf + m_start_line; };
    LINE_STATUS parse_line();
    void unmap();
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_type();
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

public:
    static int m_epollfd;
    static std::atomic<int> m_user_count; // 原子操作，线程安全
    MYSQL *mysql;
    int m_state;  //读为0, 写为1

private:
    int m_sockfd;
    sockaddr_in m_address;

    // 读缓存区
    char m_read_buf[READ_BUFFER_SIZE];
    long m_read_idx; // m_read_buf中下一个读取的位置
    long m_checked_idx; //用于解析字符串的下标
    int m_start_line; // 某行开始位置

    // 写缓存区
    char m_write_buf[WRITE_BUFFER_SIZE];
    int m_write_idx;

    // 主从状态机
    CHECK_STATE m_check_state;
    METHOD m_method;

    // 以下为解析请求报文中对应的6个变量
    char m_real_file[FILENAME_LEN]; // 存储读取文件的名称
    char *m_url;
    char *m_version;
    char *m_host;
    long m_content_length;
    bool m_linger; // 连接是否持久

    char *m_file_address;
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;
    int cgi;        // 是否启用的POST
    char *m_string; // 存储请求头数据
    int bytes_to_send;
    int bytes_have_send;
    char *doc_root;

    std::map<std::string, std::string> m_users;
    int m_TRIGMode;
    int m_close_log;

    // sql相关
    char sql_user[100];
    char sql_passwd[100];
    char sql_name[100];
};

void cb_func(client_data *user_data); // 定时器回调函数

#endif