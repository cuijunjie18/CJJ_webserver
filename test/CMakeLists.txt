find_package(GTest REQUIRED)
# set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/test)

set (TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR})

# =================================================================================
# mysql使用
include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

if (LIBMYSQLCLIENT_FOUND)
    message(STATUS "Found MySQL client library:")
    message(STATUS "  Include dirs: ${LIBMYSQLCLIENT_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${LIBMYSQLCLIENT_LIBRARIES}")
else()
    message(WARN "MySQL client library not found")
    # 手动添加
    find_library(LIBMYSQLCLIENT_LIBRARIES
        NAMES mysqlclient
        PATHS /usr/lib/x86_64-linux-gnu
    )
    set (LIBMYSQLCLIENT_INCLUDE_DIRS /usr/include/mysql)
endif()
# ==================================================================================

# sql_connection
add_executable(
    test_sql_connection_pool
    ${TEST_SRC}/CGImysql/test_sql_connection_pool.cpp
    ${CGImysql_src}
)
target_link_libraries(test_sql_connection_pool PRIVATE 
    GTest::GTest
    ${LIBMYSQLCLIENT_LIBRARIES}
)

# logger
add_executable(
    test_logger
    ${TEST_SRC}/logger/test_logger.cpp
    ${logger_src}
)
target_link_libraries(test_logger PRIVATE 
    GTest::GTest
)

# threadpool
add_executable(
    test_threadpool
    ${TEST_SRC}/threadpool/test_threadpool.cpp
    ${http_src}
    ${CGImysql_src}
    ${logger_src}
    ${timer_src}
)
target_link_libraries(test_threadpool PRIVATE 
    GTest::GTest
    ${LIBMYSQLCLIENT_LIBRARIES}
)

# timer
add_executable(
    test_timer
    ${TEST_SRC}/timer/test_timer.cpp
    ${timer_src}
)
target_link_libraries(test_timer PRIVATE 
    GTest::GTest
)

# http
add_executable(
    test_http_conn
    ${TEST_SRC}/http/test_http_conn.cpp
    ${CGImysql_src}
    ${http_src}
    ${timer_src}
    ${logger_src}
)
target_link_libraries(test_http_conn PRIVATE 
    GTest::GTest
    ${LIBMYSQLCLIENT_LIBRARIES}
)