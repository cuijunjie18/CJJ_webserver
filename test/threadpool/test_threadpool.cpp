#include "threadpool/threadpool.hpp"
#include "http/http_conn.hpp"
#include <gtest/gtest.h>

class ThreadPoolTest : public testing::Test {
public:
    void SetUp() override {
        actor_model = Proactor_Mode;
        connPool = nullptr; // Assuming no DB connection pool is needed for this test
        thread_number = 4;
        max_requests = 100;
    }

    void TearDown() override {
        delete pool;
    }
public:
    ThreadPool<HttpConn>* pool;
    int actor_model;
    ConnectionPool *connPool; 
    int thread_number; 
    int max_requests;
};

TEST_F(ThreadPoolTest, BuildThreadPool) {
    EXPECT_NO_THROW({
        pool = new ThreadPool<HttpConn>(actor_model, connPool, thread_number, max_requests);
    });
    ASSERT_NE(pool, nullptr);
    delete pool;
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}