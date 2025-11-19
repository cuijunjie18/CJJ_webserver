#include <http/http_conn.hpp>

// 静态类成员变量初始化
int HttpConn::m_epollfd = -1;
int HttpConn::m_user_count = -1;

void HttpConn::init(){
    mysql = nullptr;
    bytes_to_send = 0;
    bytes_have_send = 0;
    m_check_state = CHECK_STATE_REQUESTLINE;
    m_linger = false;
    m_method = GET;
    m_url = 0;
    m_version = 0;
    m_content_length = 0;
    m_host = 0;
    m_start_line = 0;
    m_checked_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;
    cgi = 0;
    m_state = 0;
    timer_flag = 0;
    improv = 0;

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

}

bool HttpConn::read_once(){
    if (m_read_idx >= READ_BUFFER_SIZE){
        return false;
    }
    int bytes_read = 0;

    //LT读取数据
    if (0 == m_TRIGMode){
        bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
        m_read_idx += bytes_read;

        if (bytes_read <= 0) return false;
        return true;
    }
    //ET读数据
    else{
        while (true){
            bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
            if (bytes_read == -1){
                // 在非阻塞模式下，EAGAIN或EWOULDBLOCK表示没有更多数据可读
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                return false;
            }
            else if (bytes_read == 0){ // 客户端关闭连接
                return false;
            }
            m_read_idx += bytes_read;
        }
        return true;
    }
}

HTTP_CODE HttpConn::process_read(){

}

void HttpConn::process(){
    
}
