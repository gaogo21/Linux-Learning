cmake_minimum_required(VERSION 3.15)
project(test)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# 设置标准为C++11
set(CMAKE_CXX_STANDARD 11)
include_directories(${PROJECT_SOURCE_DIR}/include)

# 找到动态库的路径
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib1)

# 生成可执行程序
add_executable(app ${SRC_LIST})

# 链接动态库
target_link_libraries(app test1)

message("非常重要的信息")
message(STATUS "一般的信息")
message(FATAL_ERROR "非常严重的错误")
message(STATUS "一般的信息")