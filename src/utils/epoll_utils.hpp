#ifndef EPOLL_UTILS_HPP
#define EPOLL_UTILS_HPP

#include "utils/common.hpp"
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

/*
cpp禁止在头文件中定义函数，使用inline避免编译错误
*/

//对文件描述符设置非阻塞
inline int setnonblocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
inline void addfd(int epollfd, int fd, bool one_shot, int TRIGMode){
    epoll_event event;
    event.data.fd = fd;

    if (TRIGMode == 1) // ET
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    else
        event.events = EPOLLIN | EPOLLRDHUP;

    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event); // 注册
    setnonblocking(fd);
}

//从内核时间表删除描述符
inline void removefd(int epollfd, int fd){
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

//将事件重置为EPOLLONESHOT
inline void modfd(int epollfd, int fd, int ev, int TRIGMode){
    epoll_event event;
    event.data.fd = fd;

    if (TRIGMode == ET_TRIGMODE)
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    else
        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event); // EPOLL_CTL_MOD为更新fd的监听状态
}

#endif