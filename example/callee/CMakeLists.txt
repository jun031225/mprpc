#set(SRC_LIST userService.cc ../user.pb.cc )
set(SRC_LIST friendService.cc ../friend.pb.cc)
#生成可执行文件
add_executable(provider ${SRC_LIST})
#链接生成的动态库
target_link_libraries(provider mprpc protobuf)
