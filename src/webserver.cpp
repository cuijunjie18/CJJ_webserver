#include <cstdio>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <webserver.hpp>

WebServer::WebServer() {
    //http_conn类对象
    users = new HttpConn[MAX_FD];

    //root文件夹路径
    char server_path[200];
    getcwd(server_path, 200);
    char root[6] = "/root";
    m_root = (char *)malloc(strlen(server_path) + strlen(root) + 1);
    strcpy(m_root, server_path);
    strcat(m_root, root);

    //定时器
    users_timer = new client_data[MAX_FD];
}

WebServer::~WebServer() {
    close(m_epollfd);
    close(m_listenfd);
    close(m_pipefd[1]);
    close(m_pipefd[0]);
    delete[] users;
    delete[] users_timer;
    delete m_pool;
}

void WebServer::init(
    int port, 
    std::string user, 
    std::string passWord, 
    std::string databaseName, 
    int log_write, 
    int opt_linger, 
    int listen_trig_mode, 
    int conn_trig_mode,
    int sql_num, 
    int thread_num, 
    int close_log, 
    int actor_model
)
{
    m_user = user;
    m_passWord = passWord;
    m_databaseName = databaseName;
    m_port = port;
    m_log_write = log_write;
    m_optlinger = opt_linger;
    m_listen_trig_mode = listen_trig_mode;
    m_conn_trig_mode = conn_trig_mode;
    m_sql_num = sql_num;
    m_thread_num = thread_num;
    m_close_log = close_log;
    m_actormodel = actor_model;
}

// 初始化数据库用户结果
void WebServer::initmysql_result(ConnectionPool *connPool) {
    //先从连接池中取一个连接
    MYSQL *mysql = nullptr;
    ConnectionRAII mysqlcon(&mysql, connPool);

    //在user表中检索username，passwd数据，浏览器端输入
    if (mysql_query(mysql, "SELECT username,passwd FROM user"))
    {
        LOG_ERROR("SELECT error:%s\n", mysql_error(mysql));
    }

    //从表中检索完整的结果集
    MYSQL_RES *result = mysql_store_result(mysql);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while (MYSQL_ROW row = mysql_fetch_row(result))
    {
        std::string temp1(row[0]);
        std::string temp2(row[1]);
        users_info[temp1] = temp2;
    }
}

// 打印当前数据库中注册的用户数据
void WebServer::show_users_info() {
    std::cout << "Users info" << std::endl;
    for (auto user_info : users_info) {
        std::cout << user_info.first << " " << user_info.second << std::endl;
    }
    return;
}

// 数据库连接池初始化
void WebServer::sql_pool() {
    m_connPool = &ConnectionPool::GetInstance();
    m_connPool->init("localhost", m_user, m_passWord, m_databaseName, m_sql_port, m_sql_num, m_close_log);

    initmysql_result(m_connPool);
}

// Htpp线程池初始化
void WebServer::thread_pool() {
    m_pool = new ThreadPool<HttpConn>(m_actormodel, m_connPool, m_thread_num);
}

// 日志初始化
void WebServer::log_write() {
    if (m_close_log == Log_Close){
        return;
    }
    switch (m_log_write) {
    case Log_Sync:
        Logger::GetInstance().init("./web_server.log", m_close_log, 2000, 800000, 0);
        break;
    case Log_Async:
        Logger::GetInstance().init("./web_server.log", m_close_log, 2000, 800000, 800); // 异步启用阻塞队列
        break;
    default:
        fprintf(stderr, "No Log write mode code: %d\n",m_log_write);
        break;
    }
}

// 设置信号处理函数
void WebServer::set_signal_handler() {
    utils.addsig(SIGPIPE, SIG_IGN);
    utils.addsig(SIGALRM, utils.sig_handler, false);
    utils.addsig(SIGTERM, utils.sig_handler, false);
    alarm(TIMESLOT);
}

// 事件监听
void WebServer::eventListen() {
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenfd < 0){
        fprintf(stderr, "Create socket error!\n");
        return;
    }

    struct linger tmp = {0, 0};
    if (m_optlinger == Linger_Open){ // 优雅关闭连接
        tmp.l_onoff = 1;
        tmp.l_linger = 1; // 超时1s
    }
    setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(m_port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(m_listenfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        fprintf(stderr, "Bind Port %d error!\n",m_port);
        return;
    }

    if (listen(m_listenfd, 5) < 0){
        fprintf(stderr, "Listen error!\n");
        return;
    }
    utils.init(TIMESLOT);

    // 创建内核事件表
    epoll_event events[MAX_EVENT_NUMBER];
    m_epollfd = epoll_create(5);
    if (m_epollfd < 0){
        fprintf(stderr, "Epoll create error!\n");
        return;
    }
    utils.addfd(m_epollfd, m_listenfd, false, m_listen_trig_mode);
    HttpConn::m_epollfd = m_epollfd;

    // 生成socket对，用于本地通信
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, m_pipefd) < 0) {
        fprintf(stderr, "Generate socket pair error!\n");
        return;
    }
    utils.setnonblocking(m_pipefd[Write_End]);
    utils.addfd(m_epollfd, m_pipefd[Read_End], false, LT_TRIGMODE); 

    //工具类,信号和描述符基础操作
    Utils::u_pipefd = m_pipefd;
    Utils::u_epollfd = m_epollfd;

    // 设置信号处理
    set_signal_handler();
}

// 为一个http连接建立定时器
void WebServer::timer(int connfd, struct sockaddr_in client_address) {
    users[connfd].init(connfd, client_address, m_root, 
        m_conn_trig_mode, m_close_log,
        m_user, m_passWord, m_databaseName);
    
    // 初始化client_data数据
    // 创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
    users_timer[connfd].address = client_address;
    users_timer[connfd].sockfd = connfd;
    UtilTimer *timer = new UtilTimer;
    timer->user_data = &users_timer[connfd];
    timer->cb_func = cb_func;
    time_t cur = time(NULL);
    timer->expire = cur + 3 * TIMESLOT;
    users_timer[connfd].timer = timer;
    utils.m_timer_lst.add_timer(timer);
}

// 若有数据传输，则将定时器往后延迟3个单位
void WebServer::adjust_timer(UtilTimer *timer) {
    time_t cur = time(NULL);
    timer->expire = cur + 3 * TIMESLOT;
    utils.m_timer_lst.adjust_timer(timer);
}

// 超时处理
void WebServer::deal_timer(UtilTimer *timer, int sockfd) {
    timer->cb_func(&users_timer[sockfd]);
    utils.m_timer_lst.del_timer(timer);
}

// 读事件处理
void WebServer::dealwithread(int sockfd) {
    UtilTimer *timer = users_timer[sockfd].timer;

    // reactor
    if (m_actormodel == Reactor_Mode) {
        adjust_timer(timer);

        // 若监测到读事件，将该事件放入请求队列
        m_pool->append(users + sockfd, Read_State);

        while (true) {
            if (users[sockfd].improv == Event_Finish) {
                if (1 == users[sockfd].timer_flag) {
                    deal_timer(timer, sockfd);
                    users[sockfd].timer_flag = 0;
                }
                users[sockfd].improv = Event_Processing;
                break;
            }
        }
    } else { // proactor
        if (users[sockfd].read_once()) {
            LOG_INFO("deal with the client(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));

            // 若监测到读事件，将该事件放入请求队列
            m_pool->append_p(users + sockfd);
            adjust_timer(timer);
        }
        else {
            deal_timer(timer, sockfd);
        }
    }
}

// 写事件处理
void WebServer::dealwithwrite(int sockfd) {
    UtilTimer *timer = users_timer[sockfd].timer;

    // reactor
    if (m_actormodel == Reactor_Mode) {
        adjust_timer(timer);

        // 若监测到写事件，将该事件放入请求队列
        m_pool->append(users + sockfd, Write_State);

        while (true) {
            if (users[sockfd].improv == Event_Finish) {
                if (1 == users[sockfd].timer_flag) {
                    deal_timer(timer, sockfd);
                    users[sockfd].timer_flag = 0;
                }
                users[sockfd].improv = Event_Processing;
                break;
            }
        }
    } else { // proactor
        if (users[sockfd].write()) {
            LOG_INFO("deal with the client(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));
            adjust_timer(timer);
        }
        else {
            deal_timer(timer, sockfd);
        }
    }
}

// 处理用户连接请求
bool WebServer::dealclientdata() {
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    if (m_listen_trig_mode == LT_TRIGMODE) {
        int connfd = accept(m_listenfd, (struct sockaddr *)&client_address, &client_addrlength);
        if (connfd < 0) {
            LOG_ERROR("%s:errno is:%d", "accept error", errno);
            return false;
        }
        if (HttpConn::m_user_count >= MAX_FD) {
            utils.show_error(connfd, "Internal server busy");
            LOG_ERROR("%s", "Internal server busy");
            return false;
        }
        timer(connfd, client_address);
    }

    else { // ET触发一次读完
        while (true) {
            int connfd = accept(m_listenfd, (struct sockaddr *)&client_address, &client_addrlength);
            if (connfd < 0)
            {
                LOG_ERROR("%s:errno is:%d", "accept error", errno);
                break;
            }
            if (HttpConn::m_user_count >= MAX_FD)
            {
                utils.show_error(connfd, "Internal server busy");
                LOG_ERROR("%s", "Internal server busy");
                break;
            }
            timer(connfd, client_address);
        }
        return false;
    }
    return true;
}

// 本地服务器处理管道接收到的信号
bool WebServer::dealwithsignal(bool& timeout, bool& stop_server) {
    int ret = 0;
    int sig;
    char signals[1024];
    ret = recv(m_pipefd[Read_End], signals, sizeof(signals), 0);
    if (ret == -1) {
        return false;
    }else if (ret == 0) {
        return false;
    }else {
        for (int i = 0; i < ret; ++i) {
            switch (signals[i]) {
            case SIGALRM:
                timeout = true;
                break;
            case SIGTERM:
                stop_server = true;
                break;
            }
        }
    }
    return true;
}

// Webserver主循环
void WebServer::eventLoop() {
    bool timeout = false;
    bool stop_server = false;

    alarm(TIMESLOT); // 开启定时事件
    while (!stop_server) {
        int n = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        for (int i = 0; i < n; i++) {
            int act_fd = events[i].data.fd;
            int act_events = events[i].events;

            if (act_fd == m_listenfd) { // 处理新http连接请求
                bool ret = dealclientdata();
                if (ret == false) {
                    // TODO: Add log
                }
            } else if ((act_fd == m_pipefd[Read_End]) && (act_events & EPOLLIN)) { // 处理本地信号
                bool ret = dealwithsignal(timeout, stop_server);
                if (ret == false) {
                    // TODO: Add log
                }
            } else if (act_events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) { // 客户端关闭连接/错误
                UtilTimer* timer = users_timer[act_fd].timer;
                deal_timer(timer, act_fd);
            } else if (act_events & EPOLLIN) { // 客户端可读数据
                dealwithread(act_fd);
            } else if (act_events & EPOLLOUT) { // 客户端可写数据
                dealwithwrite(act_fd);
            }
        }
        if (timeout) {
            utils.timer_handler(); // 处理定时事件
            timeout = false;
            alarm(TIMESLOT); // 再次开启定时事件
        }
    }
}