#设置cmake最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(mprpc)

#生成debug版本，可以进行debug调试
set(CMAKE_BUILD_TYPE "Debug")

set(PROJECT_SOURCE_DIR /workspaces/cpp/mprpc/)
#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置项目库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#设置项目编译头文件搜索路径  -I
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
#include_directories(/usr/local/protobuf/include)


#设置项目库文件搜索路径   -L
link_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(/usr/local/protobuf/lib)

add_subdirectory(src)
add_subdirectory(example)