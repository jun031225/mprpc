#set(SRC_LIST callUserService.cc ../user.pb.cc)
set(SRC_LIST callFriendService.cc ../friend.pb.cc)
#生成可执行文件
add_executable(consumer ${SRC_LIST})
#链接生成的动态库
target_link_libraries(consumer mprpc protobuf)
