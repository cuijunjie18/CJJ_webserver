set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)
project(CJJ_webserver)

# 设置c++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置源码路径
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 是否启用测试
if (ENABLE_UT_TEST)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()