find_package(GTest REQUIRED)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/test)

# GTest 测试定时器
add_executable(
    test_timer
    ${TEST_SRC}/test_timer.cpp
    ${TIMER_SRC}/lst_timer.cpp  
)
target_link_libraries(
    test_timer
    PRIVATE
    GTest::GTest
)

# =================================================================
# 测试程序
# add_executable(
#     test
#     ${TEST_SRC}/test.cpp
#     ${CGIMYSQL_SRC}/sql_connection_pool.cpp
#     ${LOG_SRC}/log.cpp ${LOG_SRC}/block_queue.cpp
# )
# target_include_directories(test PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
# target_link_libraries(test PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})