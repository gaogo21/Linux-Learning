# cmake_minimum_required(VERSION 3.15)
# project(test)
# # set(SRC_LIST add.cpp div.cpp main.cpp mult.cpp sub.cpp)

# # 搜索CMakeLists.txt目录下的所有源文件，将值存入到SRC_LIST中
# # aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)

# file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}*.cpp)
# # 设置标准为C++11
# set(CMAKE_CXX_STANDARD 11)

# #指定可执行程序的输出目录为/home/ubuntu/project/Linux-Learning/Cmake/aa/bb/cc
# # set(EXECUTABLE_OUTPUT_PATH /home/ubuntu/project/Linux-Learning/Cmake/aa/bb/cc)

# #包含头文件  -->头文件在CMakeLists.txt文件目录下的include目录
# include_directories(${PROJECT_SOURCE_DIR}/include)
# #将SRC_LIST的值取出，生成可执行程序app
# # add_executable(app ${SRC_LIST})

# # 制作动态库
# add_library(test1 SHARED ${SRC_LIST})

# # 制作静态库
# add_library(test2 STATIC ${SRC_LIST})

cmake_minimum_required(VERSION 3.15)
project(test)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# 设置标准为C++11
set(CMAKE_CXX_STANDARD 11)
include_directories(${PROJECT_SOURCE_DIR}/include)
 
# 链接静态库
link_libraries(test2)

# 指定静态库的路径
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib2)

# 生成可执行程序
add_executable(app ${SRC_LIST})