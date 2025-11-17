#include "logger/logger.hpp"
#include <gtest/gtest.h>

class LoggerTest : public testing::Test{
public:
    void SetUp() override {
        
    }
    void TearDown() override {
        
    }
public:
    std::string log_file = "test.log";
};

TEST_F(LoggerTest,InitLogger_sync_mode) {
    bool result = Logger::GetInstance().init(
        log_file.c_str(),
        0,          // close_log
        8192,       // log_buf_size
        5000000,    // split_lines
        0        // max_queue_size
    );
    EXPECT_TRUE(result);
}

TEST_F(LoggerTest,InitLogger_async_mode) {
    bool result = Logger::GetInstance().init(
        log_file.c_str(),
        0,          // close_log
        8192,       // log_buf_size
        5000000,    // split_lines
        1000        // max_queue_size
    );
    EXPECT_TRUE(result);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}