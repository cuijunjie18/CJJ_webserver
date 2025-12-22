#include "timer/lst_timer.hpp"

/*SortTimerList类定义*/
SortTimerList::SortTimerList() : head(nullptr), tail(nullptr) {}

SortTimerList::~SortTimerList() {
    UtilTimer* tmp = head;
    while (tmp){
        UtilTimer* p = tmp;
        tmp = tmp->next;
        delete p;
    }
}

// 双向链表添加节点
void SortTimerList::add_timer(UtilTimer *timer) {
    if (head == nullptr) {
        head = tail = timer;
        return;
    }
    if (timer->expire < head->expire) {
        timer->next = head;
        head->prev = timer;
        head = timer;
        return;
    }
    UtilTimer *p = head->next;
    while (p) {
        if (timer->expire < p->expire) {
            timer->next = p;
            timer->prev = p->prev;
            p->prev->next = timer;
            p->prev = timer;
            break;
        }
        p = p->next;
    }
    if (p == nullptr) {
        timer->prev = tail;
        tail->next = timer;
        tail = timer;
    }
}

// 双向链表调整节点的位置
void SortTimerList::adjust_timer(UtilTimer *timer) {
    if (timer == tail) return; // 尾部不可能再调整

    UtilTimer *p = timer->next;
    if (!p || (timer->expire < p->expire)) {
        return;
    }
    if (timer == head) {
        head = head->next;
        head->prev = nullptr;
        add_timer(timer);
    }else {
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer(timer);
    }
}

// 双向链表删除节点
void SortTimerList::del_timer(UtilTimer *timer) {
    if (timer == head && timer == tail) {
        delete timer;
        head = nullptr;
        tail = nullptr;
        return;
    }
    if (timer == head) {
        head = head->next;
        head->prev = nullptr;
        delete timer;
        return;
    }
    if (timer == tail) {
        tail = tail->prev;
        tail->next = nullptr;
        delete timer;
        return;
    }
    UtilTimer* p = head->next;
    while (p) {
        if (p == timer) {
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            delete timer;
            return;
        }
        p = p->next;
    }
    std::fprintf(stdout,"utiltimer is not in sorttimerlist!\n");
}

// 遍历升序定时器，处理超时的节点
void SortTimerList::tick() {
    if (head == nullptr) return;
    time_t cur_stamp = time(nullptr);
    UtilTimer* p = head;
    while (p) {
        if (cur_stamp < p->expire) {
            break;
        }
        p->cb_func(p->user_data);
        head = p->next;
        delete p;
        p = head;
    }
}

UtilTimer* SortTimerList::get_head() {
    return head;
}
UtilTimer* SortTimerList::get_tail() {
    return tail;
}


/*下面是Utils类的定义*/

void Utils::init(int timeslot) {
    m_TIMESLOT = timeslot;
}

//对文件描述符设置非阻塞
int Utils::setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void Utils::addfd(int epollfd, int fd, bool one_shot, int TRIGMode) {
    epoll_event event;
    event.data.fd = fd;

    if (TRIGMode == ET_TRIGMODE)
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    else if (TRIGMode == LT_TRIGMODE)
        event.events = EPOLLIN | EPOLLRDHUP;

    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

//信号处理函数
void Utils::sig_handler(int sig) {
    //为保证函数的可重入性，保留原来的errno
    int save_errno = errno;
    int msg = sig;
    send(u_pipefd[Write_End], (char *)&msg, 1, 0);
    errno = save_errno;
}

//设置信号函数
void Utils::addsig(int sig, void(handler)(int), bool restart) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart)
        sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

//定时处理任务，重新定时以不断触发SIGALRM信号
void Utils::timer_handler() {
    m_timer_lst.tick();
    alarm(m_TIMESLOT);
}

void Utils::show_error(int connfd, const char *info) {
    send(connfd, info, strlen(info), 0);
    close(connfd);
}

int *Utils::u_pipefd = 0;
int Utils::u_epollfd = 0;

