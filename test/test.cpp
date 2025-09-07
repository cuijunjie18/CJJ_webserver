#include <iostream>
#include <lock/locker.hpp>
#include <CGImysql/sql_connection_pool.hpp>
#include <threadpool/threadpool.hpp>
#include <unistd.h>
#include <log/log.hpp>

int m_close_log = 0;

void Test_log(){
    Log::GetInstance()->init("./test/demo.txt", m_close_log);
    LOG_ERROR("Hello,World!");
}

void Test_thread(){

}

int main(int argc,char *argv[])
{
    // Test sqlpool and threadpool
    // ConnectionPool *p = ConnectionPool::GetInstance();
    // ThreadPool<int> q(0,p); // 要测试要自定义一些类
    Test_log();
    sleep(100);
    std::cout << "Hello,World!" << std::endl;
    return 0;
}