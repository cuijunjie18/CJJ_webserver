#include "http/http_conn.hpp"

// 静态类成员变量初始化
int HttpConn::m_epollfd{-1};
std::atomic<int> HttpConn::m_user_count{0};

//定义http响应的一些状态信息
const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file form this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the request file.\n";

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

// 读取http用户请求报文
bool HttpConn::read_once() {
    if (m_read_idx >= READ_BUFFER_SIZE) {
        return false;
    }
    int bytes_read = 0;

    // LT读取数据
    if (m_TRIGMode == LT_TRIGMODE) {
        bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
        m_read_idx += bytes_read;

        if (bytes_read <= 0) return false;
        return true;
    }
    // ET读数据(一次读完)
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

// 处理请求
HTTP_CODE HttpConn::do_request() {
    // strcpy(m_real_file, doc_root);
    // int len = strlen(doc_root);
    // const char *p = strrchr(m_url, '/');

    // //处理cgi
    // if (cgi == 1 && (*(p + 1) == '2' || *(p + 1) == '3')) {

    //     //根据标志判断是登录检测还是注册检测
    //     char flag = m_url[1];

    //     char *m_url_real = (char *)malloc(sizeof(char) * 200);
    //     strcpy(m_url_real, "/");
    //     strcat(m_url_real, m_url + 2);
    //     strncpy(m_real_file + len, m_url_real, FILENAME_LEN - len - 1);
    //     free(m_url_real);

    //     //将用户名和密码提取出来
    //     //user=123&passwd=123
    //     char name[100], password[100];
    //     int i;
    //     for (i = 5; m_string[i] != '&'; ++i)
    //         name[i - 5] = m_string[i];
    //     name[i - 5] = '\0';

    //     int j = 0;
    //     for (i = i + 10; m_string[i] != '\0'; ++i, ++j)
    //         password[j] = m_string[i];
    //     password[j] = '\0';

    //     if (*(p + 1) == '3')
    //     {
    //         //如果是注册，先检测数据库中是否有重名的
    //         //没有重名的，进行增加数据
    //         char *sql_insert = (char *)malloc(sizeof(char) * 200);
    //         strcpy(sql_insert, "INSERT INTO user(username, passwd) VALUES(");
    //         strcat(sql_insert, "'");
    //         strcat(sql_insert, name);
    //         strcat(sql_insert, "', '");
    //         strcat(sql_insert, password);
    //         strcat(sql_insert, "')");

    //         if (users.find(name) == users.end())
    //         {
    //             m_lock.lock();
    //             int res = mysql_query(mysql, sql_insert);
    //             users.insert(pair<string, string>(name, password));
    //             m_lock.unlock();

    //             if (!res)
    //                 strcpy(m_url, "/log.html");
    //             else
    //                 strcpy(m_url, "/registerError.html");
    //         }
    //         else
    //             strcpy(m_url, "/registerError.html");
    //     }
    //     //如果是登录，直接判断
    //     //若浏览器端输入的用户名和密码在表中可以查找到，返回1，否则返回0
    //     else if (*(p + 1) == '2')
    //     {
    //         if (users.find(name) != users.end() && users[name] == password)
    //             strcpy(m_url, "/welcome.html");
    //         else
    //             strcpy(m_url, "/logError.html");
    //     }
    // }

    // if (*(p + 1) == '0')
    // {
    //     char *m_url_real = (char *)malloc(sizeof(char) * 200);
    //     strcpy(m_url_real, "/register.html");
    //     strncpy(m_real_file + len, m_url_real, strlen(m_url_real));

    //     free(m_url_real);
    // }
    // else if (*(p + 1) == '1')
    // {
    //     char *m_url_real = (char *)malloc(sizeof(char) * 200);
    //     strcpy(m_url_real, "/log.html");
    //     strncpy(m_real_file + len, m_url_real, strlen(m_url_real));

    //     free(m_url_real);
    // }
    // else if (*(p + 1) == '5')
    // {
    //     char *m_url_real = (char *)malloc(sizeof(char) * 200);
    //     strcpy(m_url_real, "/picture.html");
    //     strncpy(m_real_file + len, m_url_real, strlen(m_url_real));

    //     free(m_url_real);
    // }
    // else if (*(p + 1) == '6')
    // {
    //     char *m_url_real = (char *)malloc(sizeof(char) * 200);
    //     strcpy(m_url_real, "/video.html");
    //     strncpy(m_real_file + len, m_url_real, strlen(m_url_real));

    //     free(m_url_real);
    // }
    // else if (*(p + 1) == '7')
    // {
    //     char *m_url_real = (char *)malloc(sizeof(char) * 200);
    //     strcpy(m_url_real, "/fans.html");
    //     strncpy(m_real_file + len, m_url_real, strlen(m_url_real));

    //     free(m_url_real);
    // }
    // else
    //     strncpy(m_real_file + len, m_url, FILENAME_LEN - len - 1);

    // if (stat(m_real_file, &m_file_stat) < 0)
    //     return NO_RESOURCE;

    // if (!(m_file_stat.st_mode & S_IROTH))
    //     return FORBIDDEN_REQUEST;

    // if (S_ISDIR(m_file_stat.st_mode))
    //     return BAD_REQUEST;

    // int fd = open(m_real_file, O_RDONLY);
    // m_file_address = (char *)mmap(0, m_file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    // close(fd);
    // return FILE_REQUEST;
}

// 读取请求的主状态机(报文解析)
HTTP_CODE HttpConn::process_read() {
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    char *text = 0;

    while ((m_check_state == CHECK_STATE_CONTENT && line_status == LINE_OK) || ((line_status = parse_line()) == LINE_OK))
    {
        text = get_line();
        m_start_line = m_checked_idx;
        LOG_INFO("%s", text);
        switch (m_check_state) {
            case CHECK_STATE_REQUESTLINE:
                ret = parse_request_line(text);
                if (ret == BAD_REQUEST) {
                    return BAD_REQUEST;
                }
                break;
            case CHECK_STATE_HEADER:
                ret = parse_headers(text);
                if (ret == BAD_REQUEST) {
                    return BAD_REQUEST;
                }else if (ret == GET_REQUEST) {
                    return do_request();
                }
                break;
            case CHECK_STATE_CONTENT:
                ret = parse_content(text);
                if (ret == GET_REQUEST) {
                    return do_request();
                }
                line_status = LINE_OPEN;
                break;
            default:
                return INTERNAL_ERROR;
        }
    }
    return NO_REQUEST;
}

// 写入请求的主状态机(报文响应)
bool HttpConn::process_write(HTTP_CODE ret) {
    switch (ret) {
        case INTERNAL_ERROR:
            add_status_line(500, error_500_title);
            add_headers(strlen(error_500_form));
            if (!add_content(error_500_form)) {
                return false;
            }
            break;
        case BAD_REQUEST:
        {
            add_status_line(404, error_404_title);
            add_headers(strlen(error_404_form));
            if (!add_content(error_404_form))
                return false;
            break;
        }
        case FORBIDDEN_REQUEST:
        {
            add_status_line(403, error_403_title);
            add_headers(strlen(error_403_form));
            if (!add_content(error_403_form))
                return false;
            break;
        }
        case FILE_REQUEST:
        {
            add_status_line(200, ok_200_title);
            if (m_file_stat.st_size != 0)
            {
                add_headers(m_file_stat.st_size);
                m_iv[0].iov_base = m_write_buf;
                m_iv[0].iov_len = m_write_idx;
                m_iv[1].iov_base = m_file_address;
                m_iv[1].iov_len = m_file_stat.st_size;
                m_iv_count = 2;
                bytes_to_send = m_write_idx + m_file_stat.st_size;
                return true;
            }
            else
            {
                const char *ok_string = "<html><body></body></html>";
                add_headers(strlen(ok_string));
                if (!add_content(ok_string))
                    return false;
            }
        }
        default:
            return false;
    }
    m_iv[0].iov_base = m_write_buf;
    m_iv[0].iov_len = m_write_idx;
    m_iv_count = 1;
    bytes_to_send = m_write_idx;
    return true;
}

// 工作线程调用
void HttpConn::process() {
    HTTP_CODE read_ret = process_read();
    if (read_ret == NO_REQUEST) {
        modfd(m_epollfd, m_sockfd, EPOLLIN, m_TRIGMode);
        return;
    }
    bool write_ret = process_write(read_ret);
    if (!write_ret) {
        close_conn();
    }
    modfd(m_epollfd, m_sockfd, EPOLLOUT, m_TRIGMode);
}