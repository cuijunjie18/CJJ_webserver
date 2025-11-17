#include <iostream>
#include <lock/locker.hpp>
#include <CGImysql/sql_connection_pool.hpp>
#include <threadpool/threadpool.hpp>
#include <unistd.h>
#include <logger/logger.hpp>

void Test_log(){
    
}

int main(int argc,char *argv[])
{
    // // Test sqlpool and threadpool
    // ConnectionPool *p = ConnectionPool::GetInstance();
    // // ThreadPool<int> q(0,p);
    // Test_log();
    // sleep(100);
    std::cout << "Hello,World!" << std::endl;
    return 0;
}