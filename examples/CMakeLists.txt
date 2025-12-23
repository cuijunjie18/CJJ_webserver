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

# mysql_demo程序
add_executable(
    mysql_demo
    ${EXAMPLES_DIR}/mysql_demo.cpp
)
target_include_directories(mysql_demo PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(mysql_demo PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})

# client_demo程序
add_executable(
    client_demo
    ${EXAMPLES_DIR}/client_demo.cpp
    ${client_src}
)