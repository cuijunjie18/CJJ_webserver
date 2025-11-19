#include <cstdio>
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

void WebServer::sql_pool() {
    // m_connPool = &ConnectionPool::GetInstance();
    // m_connPool->init("localhost", m_user, m_passWord, m_databaseName, m_sql_port, m_sql_num, m_close_log);

    // users->initmysql_result(m_connPool);
}

void WebServer::thread_pool() {
    // m_pool = new ThreadPool<HttpConn>(m_actormodel, m_connPool, m_thread_num);
}

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

void WebServer::set_signal_handler(){
    utils.addsig(SIGPIPE, SIG_IGN);
    utils.addsig(SIGALRM, utils.sig_handler, false);
    utils.addsig(SIGTERM, utils.sig_handler, false);
    alarm(TIMESLOT);
}

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
}

void WebServer::timer(int connfd, struct sockaddr_in client_address) {
    // users[connfd].init();
}

void WebServer::adjust_timer(UtilTimer *timer) {

}

void WebServer::deal_timer(UtilTimer *timer, int sockfd) {

}

void WebServer::dealwithread(int sockfd) {

}

void WebServer::dealwithwrite(int sockfd) {

}

bool WebServer::dealclientdata() {

}

bool WebServer::dealwithsignal(bool& timeout, bool& stop_server) {

}

void WebServer::eventLoop() {
    
}