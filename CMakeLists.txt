set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)
project(CJJ_webserver)

# 设置c++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置cmake变量
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(CGIMYSQL_SRC ${PROJECT_SOURCE_DIR}/src/CGImysql)
set(LOG_SRC ${PROJECT_SOURCE_DIR}/src/log)
set(TIMER_SRC ${PROJECT_SOURCE_DIR}/src/timer)
set(TEST_SRC ${PROJECT_SOURCE_DIR}/test)

# message(WARNING "SRC_DIR = ${SRC_DIR}")

# ===================================================

# 第三方库

# mysql使用
# include(FindPkgConfig)
# pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

find_library(LIBMYSQLCLIENT
    NAMES mysqlclient
    PATHS /usr/lib/x86_64-linux-gnu
)

# ===================================================

# 添加头文件
include_directories(${PROJECT_SOURCE_DIR}/src)


# 主程序
add_executable(
    main
    ${SRC_DIR}/main.cpp
    ${CGIMYSQL_SRC}/sql_connection_pool.cpp
    ${LOG_SRC}/log.cpp ${LOG_SRC}/block_queue.cpp
)
# target_include_directories(main PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
# target_link_libraries(main PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})
target_link_libraries(main PRIVATE ${LIBMYSQLCLIENT})


# 是否启用测试
if (ENABLE_UT_TEST)
  add_subdirectory(test)
endif()