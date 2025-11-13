#include "CGImysql/sql_connection_pool.hpp"
#include "gtest/gtest.h"

class CGImysqlTest : public testing::Test {
public:
    void SetUp() override {
        url = "localhost";
        User = "webdev";
        PassWord = "12345678";
        DataBaseName = "user_info_db";
        Port = 3306;
        MaxConn = 10;
        use_log = false;
    }

    void TearDown() override {
        ConnectionPool::GetInstance().DestroyPool();
    }

public:
    std::string url;
    std::string User;
    std::string PassWord;
    std::string DataBaseName;
    int Port;
    int MaxConn;
    bool use_log;
};

TEST_F(CGImysqlTest, Init){
    EXPECT_EQ(ConnectionPool::GetInstance().GetFreeConn(),0);
    bool init_result = ConnectionPool::GetInstance().init(
        url,User,PassWord,DataBaseName,Port,MaxConn,use_log
    );
    EXPECT_TRUE(init_result);
    EXPECT_EQ(ConnectionPool::GetInstance().GetFreeConn(), MaxConn);
};

TEST_F(CGImysqlTest, GetAndReleaseConnection){
    ConnectionPool::GetInstance().init(
        url,User,PassWord,DataBaseName,Port,MaxConn,use_log
    );
    MYSQL* conn = ConnectionPool::GetInstance().GetConnection();
    EXPECT_NE(conn, nullptr);
    EXPECT_EQ(ConnectionPool::GetInstance().GetFreeConn(), MaxConn - 1);

    bool release_result = ConnectionPool::GetInstance().ReleaseConnection(conn);
    EXPECT_TRUE(release_result);
    EXPECT_EQ(ConnectionPool::GetInstance().GetFreeConn(), MaxConn);
};

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

