#include <iostream>
#include <lock/locker.hpp>
#include <CGImysql/sql_connection_pool.hpp>
#include <threadpool/threadpool.hpp>

int main(int argc,char *argv[])
{
    ConnectionPool *p = ConnectionPool::GetInstance();
    std::cout << "Hello,World!" << std::endl;
    return 0;
}