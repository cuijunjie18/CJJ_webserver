#include <CGImysql/sql_connection_pool.hpp>
#include <iostream>

ConnectionPool::ConnectionPool(){
    max_conn = 0;
    free_conn = 0;
    cur_conn = 0;
}

ConnectionPool::~ConnectionPool(){
    this->DestroyPool();
}

/*c++11后，使用局部变量懒汉不用加锁*/
ConnectionPool* ConnectionPool::GetInstance(){
    static ConnectionPool conn_pool;
    return &conn_pool;
}

// 数据库连接池资源初始化
void ConnectionPool::init(
    std::string url, 
    std::string User, 
    std::string PassWord, 
    std::string DataBaseName, 
    int Port, int MaxConn, bool use_log)
{
    cp_url = url;
    cp_user = User;
    cp_passwd = PassWord;
    cp_dbname = DataBaseName;
    cp_port = Port;
    cp_use_log = use_log;
    max_conn = MaxConn;
    
    for (int i = 0; i < MaxConn; i++){
        MYSQL* temp = nullptr;

        temp = mysql_init(temp);
        if (temp == nullptr){
            std::cerr << "Failed to init mysql!" << std::endl;
            exit(1);
        }

        temp = mysql_real_connect(
                temp, 
                cp_url.c_str(), 
                cp_user.c_str(),
                cp_passwd.c_str(),
                cp_dbname.c_str(),
                cp_port,
                nullptr,
                0
            );
        if (temp == nullptr){
            std::cerr << "Failed to connect mysql!" << std::endl;
            exit(1);
        }

        conn_list.push_back(temp);
        free_conn++;
    }
    reserve = sem(max_conn);
}

// 当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL* ConnectionPool::GetConnection(){
    MYSQL* conn = nullptr;

    if (conn_list.size() == 0){
        return nullptr;
    }
    
    reserve.wait();
    lock.lock();
    
    conn = conn_list.front();
    conn_list.pop_front();
    free_conn--;
    cur_conn++;

    lock.unlock();

    return conn;
}

// 释放一个数据库连接
bool ConnectionPool::ReleaseConnection(MYSQL *conn){
    lock.lock();

    conn_list.push_back(conn);
    free_conn++;
    cur_conn--;

    lock.unlock();
    reserve.post();
    return true;
}

int ConnectionPool::GetFreeConn(){
    return this->free_conn;
}

void ConnectionPool::DestroyPool(){
    lock.lock();

    if (conn_list.size() > 0){
        std::list<MYSQL*>::iterator it;
        for (it = conn_list.begin(); it != conn_list.end(); it++){
            MYSQL* con = *it;
            mysql_close(con);
        }
        cur_conn = 0;
        free_conn = 0;
        conn_list.clear();
    }

    lock.unlock();
}

ConnectionRAII::ConnectionRAII(MYSQL **con, ConnectionPool *connPool){
    *con = connPool->GetConnection();
    conRAII = *con;
    poolRAII = connPool;
}

ConnectionRAII::~ConnectionRAII(){
    poolRAII->ReleaseConnection(conRAII);
}
