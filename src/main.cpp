#include <iostream>
#include <lock/locker.hpp>
#include <CGImysql/sql_connection_pool.hpp>
#include <threadpool/threadpool.hpp>
#include <unistd.h>
#include <log/log.hpp>

int main(int argc,char *argv[])
{
    ConnectionPool *p = ConnectionPool::GetInstance();
    ThreadPool<int> q(0,p);
    
    sleep(100);
    std::cout << "Hello,World!" << std::endl;
    return 0;
}