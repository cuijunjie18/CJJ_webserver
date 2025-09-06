#include <log/log.hpp>

Log::Log(){
    m_count = 0;
    m_is_async = false; // 默认同步记录
}

Log::~Log(){
    if (m_fp != NULL){
        fclose(m_fp);
    }
    delete m_log_queue;
    delete [] m_buf;
}

// C++11以后,使用局部变量懒汉不用加锁
Log* Log::GetInstance(){
    static Log instacne;
    return &instacne;
}

void* Log::flush_log_thread(void *args){
    Log::GetInstance()->async_write_log();
}

bool Log::init(
    const char *file_name, // 日志文件
    int close_log, // 是否开启日志
    int log_buf_size, // 日志缓冲区大小
    int split_lines, // 日志最大行数
    int max_queue_size // 阻塞队列长度
)
{
    // 设置了阻塞队列大小，启动异步
    if (max_queue_size > 0){
        m_is_async = true;
        m_log_queue = new BlockQueue(max_queue_size);
        pthread_t tid;
        //flush_log_thread为回调函数,这里表示创建线程异步写日志
        pthread_create(&tid, NULL, flush_log_thread, NULL);
    }

    // 初始化其他成员变量
    m_close_log = close_log;
    m_log_buf_size = log_buf_size;
    m_buf = new char[m_log_buf_size];
    memset(m_buf, '\0', m_log_buf_size);
    m_split_lines = split_lines;

    time_t t = time(nullptr);
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;

    const char *p = strrchr(file_name, '/');
    char log_full_name[256] = {0};

    if (p == nullptr){
        snprintf(log_full_name, 255, "%d_%02d_%02d_%s", 
            my_tm.tm_year + 1900, 
            my_tm.tm_mon + 1, 
            my_tm.tm_mday, 
            file_name
        );
    }
    else{
        strcpy(log_name, p + 1);
        strncpy(dir_name, file_name, p - file_name + 1);
        snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", 
            dir_name, 
            my_tm.tm_year + 1900, 
            my_tm.tm_mon + 1,
            my_tm.tm_mday, 
            log_name
        );
    }

    m_today = my_tm.tm_mday;
    
    m_fp = fopen(log_full_name, "a");
    if (m_fp == nullptr) return false;
    return true;
}

void Log::write_log(int level, const char *format, ...){
        struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;
    char s[16] = {0};
    switch (level)
    {
    case 0:
        strcpy(s, "[debug]:");
        break;
    case 1:
        strcpy(s, "[info]:");
        break;
    case 2:
        strcpy(s, "[warn]:");
        break;
    case 3:
        strcpy(s, "[erro]:");
        break;
    default:
        strcpy(s, "[info]:");
        break;
    }
    //写入一个log，对m_count++, m_split_lines最大行数
    m_mutex.lock();
    m_count++;

    if (m_today != my_tm.tm_mday || m_count % m_split_lines == 0){//everyday log
        
        char new_log[256] = {0};
        fflush(m_fp);
        fclose(m_fp);
        char tail[16] = {0};
       
        snprintf(tail, 16, "%d_%02d_%02d_", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday);
       
        if (m_today != my_tm.tm_mday)
        {
            snprintf(new_log, 255, "%s%s%s", dir_name, tail, log_name);
            m_today = my_tm.tm_mday;
            m_count = 0;
        }
        else
        {
            snprintf(new_log, 255, "%s%s%s.%lld", dir_name, tail, log_name, m_count / m_split_lines);
        }
        m_fp = fopen(new_log, "a");
    }
 
    m_mutex.unlock();

    va_list valst;
    va_start(valst, format);

    std::string log_str;
    m_mutex.lock();

    //写入的具体时间内容格式
    int n = snprintf(m_buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                     my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,
                     my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);
    
    int m = vsnprintf(m_buf + n, m_log_buf_size - n - 1, format, valst);
    m_buf[n + m] = '\n';
    m_buf[n + m + 1] = '\0';
    log_str = m_buf;

    m_mutex.unlock();

    if (m_is_async && !m_log_queue->full()){
        m_log_queue->push(log_str);
    }
    else{
        m_mutex.lock();
        fputs(log_str.c_str(), m_fp);
        m_mutex.unlock();
    }

    va_end(valst);
}

void Log::flush(){
    m_mutex.lock();
    fflush(m_fp); // 强制刷新写入流缓冲区
    m_mutex.unlock();
}

void* Log::async_write_log(){
    std::string single_log;
    //从阻塞队列中取出一个日志string，写入文件
    while (m_log_queue->pop(single_log))
    {
        m_mutex.lock();
        fputs(single_log.c_str(), m_fp);
        m_mutex.unlock();
    }
}
