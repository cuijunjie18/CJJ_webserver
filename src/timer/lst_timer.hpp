#ifndef LST_TIMER_HPP
#define LST_TIMER_HPP

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
#include <iostream>
#include <time.h>

#include "logger/logger.hpp"
#include "utils/common.hpp"

class UtilTimer;

struct client_data {
    sockaddr_in address;
    int sockfd;
    UtilTimer *timer;
};

// 定时器类
class UtilTimer {
public:
    UtilTimer() : prev(nullptr), next(nullptr) {}

public:
    time_t expire;
    
    void (* cb_func)(client_data *);
    client_data *user_data;
    UtilTimer *prev;
    UtilTimer *next;
};

// 定时器容器类
class SortTimerList {
public:
    SortTimerList();
    ~SortTimerList();

    void add_timer(UtilTimer *timer);
    void adjust_timer(UtilTimer *timer);
    void del_timer(UtilTimer *timer);
    void tick();
    UtilTimer* get_head();
    UtilTimer* get_tail();

private:
    UtilTimer *head;
    UtilTimer *tail;
};

class Utils {
public:
    Utils() {}
    ~Utils() {}

    void init(int timeslot);

    //对文件描述符设置非阻塞
    int setnonblocking(int fd);

    //将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
    void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);

    //信号处理函数
    static void sig_handler(int sig);

    //设置信号函数
    void addsig(int sig, void(handler)(int), bool restart = true);

    //定时处理任务，重新定时以不断触发SIGALRM信号
    void timer_handler();

    void show_error(int connfd, const char *info);

    void show_timer_list();

public:
    static int *u_pipefd; // 使用管道传递信号
    static int u_epollfd;
    SortTimerList m_timer_lst;
    int m_TIMESLOT;
};

#endif
