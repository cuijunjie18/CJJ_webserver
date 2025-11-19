#include "lock/locker.hpp"
#include "CGImysql/sql_connection_pool.hpp"
#include "threadpool/threadpool.hpp"
#include "logger/logger.hpp"
#include "webserver.hpp"
#include "utils/config.hpp"

int main(int argc,char *argv[])
{
    WebConfig config;
    WebServer server;

    // 初始化
    server.init(
        config.port,
        config.user,
        config.passWord,
        config.databaseName,
        config.log_write,
        config.opt_linger,
        config.listen_trig_mode,
        config.conn_trig_mode,
        config.sql_num,
        config.thread_num,
        config.close_log,
        config.actor_model
    );

    //日志
    server.log_write();

    //数据库
    // server.sql_pool();

    //线程池
    // server.thread_pool();

    //监听
    // server.eventListen();

    //运行
    // server.eventLoop();
    return 0;
}