set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)
project(CJJ_webserver)

# 设置cmake变量
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(CGIMYSQL_SRC ${PROJECT_SOURCE_DIR}/src/CGImysql)
set(LOG_SRC ${PROJECT_SOURCE_DIR}/src/log)
set(TEST_SRC ${PROJECT_SOURCE_DIR}/test)

# message(WARNING "SRC_DIR = ${SRC_DIR}")

# ===================================================

# 第三方库

# mysql使用
include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

# ===================================================

# 添加头文件
include_directories(${PROJECT_SOURCE_DIR}/include)


# 主程序
add_executable(
    main
    ${SRC_DIR}/main.cpp
    ${CGIMYSQL_SRC}/sql_connection_pool.cpp
    ${LOG_SRC}/log.cpp ${LOG_SRC}/block_queue.cpp
)
target_include_directories(main PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})


# =================================================================
# 测试程序
add_executable(
    test
    ${TEST_SRC}/test.cpp
    ${CGIMYSQL_SRC}/sql_connection_pool.cpp
    ${LOG_SRC}/log.cpp ${LOG_SRC}/block_queue.cpp
)
target_include_directories(test PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(test PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})


# =====================================================================

# # demo程序
# add_executable(
#     demo
#     ${SRC_DIR}/mysql_demo.cpp
# )
# target_include_directories(demo PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
# target_link_libraries(demo PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})