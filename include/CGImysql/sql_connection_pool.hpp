#ifndef SQL_CONNECTION_POOL_HPP
#define SQL_CONNECTION_POOL_HPP

#include <mysql/mysql.h>
#include <string>
#include <string.h>
#include <list>

#include "lock/locker.hpp"

class ConnectionPool
{
public:
    MYSQL *GetConnection();				 // 获取数据库连接
	bool ReleaseConnection(MYSQL *conn); // 释放连接
	int GetFreeConn();					 // 获取可用连接数
	void DestroyPool();					 // 销毁所有连接

	//单例模式
	static ConnectionPool *GetInstance();

	void init(std::string url, 
            std::string User, 
            std::string PassWord, 
            std::string DataBaseName, 
            int Port, 
            int MaxConn, 
            bool use_log); 

private:
    ConnectionPool();
    ~ConnectionPool();

    // 连接池流量控制
    int max_conn;
    int cur_conn;
    int free_conn;
    locker lock;
    std::list<MYSQL*> conn_list;
    sem reserve;

public:
    /*cp 为 connection pool的缩写*/

    // Mysql连接需要的参数
    std::string cp_url;
    std::string cp_user;
    std::string cp_dbname;
    std::string cp_passwd;
    int cp_port;

    bool cp_use_log; // 日志开关
};

class ConnectionRAII
{
public:
    ConnectionRAII(MYSQL **con, ConnectionPool *connPool);
	~ConnectionRAII();
private:
    MYSQL *conRAII;
    ConnectionPool *poolRAII;
};

#endif