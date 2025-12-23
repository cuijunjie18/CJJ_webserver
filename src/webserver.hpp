#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "CGImysql/sql_connection_pool.hpp"
#include "http/http_conn.hpp"
#include "threadpool/threadpool.hpp"
#include "timer/lst_timer.hpp"
#include "logger/logger.hpp"
#include "utils/common.hpp"

const int MAX_FD = 65536;           //最大文件描述符
const int MAX_EVENT_NUMBER = 10000; //最大事件数
const int TIMESLOT = 5;             //最小超时单位

class WebServer
{
public:
    WebServer();
    ~WebServer();

    void init(int port , std::string user, std::string passWord, std::string databaseName,
              int log_write , int opt_linger, int listen_trig_mode, int conn_trig_mode,
              int sql_num, int thread_num, int close_log, int actor_model, 
              std::string mysql_url = "localhost", int mysql_port = 3306);

    // 数据库
    void sql_pool();
    void initmysql_result(ConnectionPool *connPool);
    void show_users_info();

    void thread_pool();
    void log_write();
    void eventListen();
    void eventLoop();
    void set_signal_handler();
    void timer(int connfd, struct sockaddr_in client_address);
    void adjust_timer(UtilTimer *timer);
    void deal_timer(UtilTimer *timer, int sockfd);
    bool dealclientdata();
    bool dealwithsignal(bool& timeout, bool& stop_server);
    void dealwithread(int sockfd);
    void dealwithwrite(int sockfd);

    // 查看当前连接的ip
    void show_connection() {
        LOG_INFO("Current user nums: %d", HttpConn::m_user_count.load());
    }

public:
    //基础
    int m_port;
    char *m_root;
    int m_log_write;
    int m_close_log;
    int m_actormodel;

    int m_pipefd[2];
    int m_epollfd;
    HttpConn *users;
    std::set<int> active_user_fds; // 活跃的连接文件描述符

    // 数据库相关
    ConnectionPool *m_connPool;
    std::string m_mysql_url;    //MySQL服务器地址
    std::string m_user;         //登陆数据库用户名
    std::string m_passWord;     //登陆数据库密码
    std::string m_databaseName; //使用数据库名
    int m_sql_num;
    int m_sql_port;             //MySQL端口
 
    //线程池相关
    ThreadPool<HttpConn> *m_pool;
    int m_thread_num;

    //epoll_event相关
    epoll_event events[MAX_EVENT_NUMBER];

    int m_listenfd;
    int m_optlinger;
    int m_listen_trig_mode;
    int m_conn_trig_mode;

    //定时器相关
    client_data *users_timer;
    Utils utils;
};

#endif
