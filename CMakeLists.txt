set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)
project(CJJ_webserver)

# 设置c++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译指令
add_compile_options(
    -g
    -Wall
    -Wextra
)

# 设置源码路径
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# 设置源文件
file(GLOB http_src ${SRC_DIR}/http/*.cpp)
file(GLOB logger_src ${SRC_DIR}/logger/*.cpp)
file(GLOB timer_src ${SRC_DIR}/timer/*.cpp)
file(GLOB CGImysql_src ${SRC_DIR}/CGImysql/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 是否启用测试
if (ENABLE_UT_TEST)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()