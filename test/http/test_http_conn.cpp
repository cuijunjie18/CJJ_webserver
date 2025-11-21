#include <gtest/gtest.h>
#include "http/http_conn.hpp"

class HttpConnTest : public testing::Test {
public:
    void SetUp() override {

    }
    void TearDown() override {

    }
private:
    HttpConn http_conn;
};

TEST_F(HttpConnTest, Parse) {
    
}

int main(int argc,char *argv[]) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}