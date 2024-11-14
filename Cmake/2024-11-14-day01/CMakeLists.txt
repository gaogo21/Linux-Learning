cmake_minimum_required(VERSION 3.15)
project(test)
set(SRC_LIST add.cpp div.cpp main.cpp mult.cpp sub.cpp)

# 设置标准为C++11
set(CMAKE_CXX_STANDARD 11)

#指定可执行程序的输出目录为/home/ubuntu/project/Linux-Learning/Cmake/aa/bb/cc
set(EXECUTABLE_OUTPUT_PATH /home/ubuntu/project/Linux-Learning/Cmake/aa/bb/cc)
add_executable(app ${SRC_LIST})