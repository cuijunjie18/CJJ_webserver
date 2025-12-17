# 设置cmake变量
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# message(WARNING "SRC_DIR = ${SRC_DIR}")

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

# 主程序
add_executable(
    main
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/webserver.cpp
    ${CGImysql_src}
    ${timer_src}
    ${logger_src}
    ${http_src}
)
target_include_directories(main PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})

# mysql_demo程序
add_executable(
    mysql_demo
    ${SRC_DIR}/mysql_demo.cpp
)
target_include_directories(mysql_demo PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(mysql_demo PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})