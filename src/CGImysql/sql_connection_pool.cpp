#include <CGImysql/sql_connection_pool.hpp>

ConnectionPool::ConnectionPool(){
    max_conn = 0;
    free_conn = 0;
    cur_conn = 0;
}

ConnectionPool* ConnectionPool::GetInstance(){
    static ConnectionPool conn_pool;
    return &conn_pool;
}

void ConnectionPool::init(
    std::string url, 
    std::string User, 
    std::string PassWord, 
    std::string DataBaseName, 
    int Port, int MaxConn, bool use_log)
{
    
}