#include "http/http_conn.hpp"

// 静态类成员变量初始化
int HttpConn::m_epollfd{-1};
std::atomic<int> HttpConn::m_user_count{0};

// 定时器回调函数
void cb_func(client_data *user_data){
    epoll_ctl(Utils::u_epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    HttpConn::m_user_count--; // 关闭一个连接，客户总量-1
}

void HttpConn::init() {
    mysql = nullptr;
    bytes_to_send = 0;
    bytes_have_send = 0;
    m_check_state = CHECK_STATE_REQUESTLINE;
    m_linger = false;
    m_method = GET;
    m_url = nullptr;
    m_version = nullptr;
    m_content_length = 0;
    m_host = nullptr;
    m_start_line = 0;
    m_checked_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;
    cgi = 0;
    m_state = Read_State;
    timer_flag = 0;
    improv = Event_Processing;

    memset(m_read_buf, '\0', READ_BUFFER_SIZE);
    memset(m_write_buf, '\0', WRITE_BUFFER_SIZE);
    memset(m_real_file, '\0', FILENAME_LEN);
}

void HttpConn::init(int sockfd, 
    const sockaddr_in &addr, 
    char* root, int TRIGMode, int close_log, 
    std::string user, 
    std::string passwd, 
    std::string sqlname)
{
    m_sockfd = sockfd;
    m_address = addr;

    doc_root = root;
    m_TRIGMode = TRIGMode;
    m_close_log = close_log;

    addfd(m_epollfd, m_sockfd, false, m_TRIGMode);
    m_user_count++;

    strcpy(sql_user, user.c_str());
    strcpy(sql_passwd, passwd.c_str());
    strcpy(sql_name, sqlname.c_str());

    init();
}

void HttpConn::close_conn(bool real_close) {
    if (real_close && (m_sockfd != -1))
    {
        printf("close %d\n", m_sockfd);
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count--;
    }
}

bool HttpConn::read_once() {
    if (m_read_idx >= READ_BUFFER_SIZE) {
        return false;
    }
    int bytes_read = 0;

    //LT读取数据
    if (m_TRIGMode == LT_TRIGMODE) {
        bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
        m_read_idx += bytes_read;

        if (bytes_read <= 0) return false;
        return true;
    }
    //ET读数据(一次读完)
    else {
        while (true) {
            bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
            if (bytes_read == -1) {
                // 在非阻塞模式下，EAGAIN或EWOULDBLOCK表示没有更多数据可读
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                return false;
            }
            else if (bytes_read == 0) { // 客户端关闭连接
                return false;
            }
            m_read_idx += bytes_read;
        }
        return true;
    }
}

bool HttpConn::write() {

}

void HttpConn::process() {

}

// 解析http请求行，获得请求方法，目标url及http版本号
HTTP_CODE HttpConn::parse_request_line(char *text) {
    
}

HTTP_CODE parse_headers(char *text) {

}
HTTP_CODE parse_content(char *text) {

}
HTTP_CODE do_request() {

}
