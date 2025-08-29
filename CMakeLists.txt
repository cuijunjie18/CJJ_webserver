set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)
project(CJJ_webserver)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# message(WARNING "SRC_DIR = ${SRC_DIR}")

include_directories(${PROJECT_SOURCE_DIR}/include) # 添加头文件


# 主程序
add_executable(
    main
    ${SRC_DIR}/main.cpp
)

# mysql使用
include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)
# find_package(MySQL REQUIRED)

# demo程序
add_executable(
    demo
    ${SRC_DIR}/mysql_demo.cpp
)
target_include_directories(demo PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(demo PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})


# target_link_libraries(demo ${MYSQL_LIBRARIES})
# target_include_directories(demo PRIVATE ${MYSQL_INCLUDE_DIR})