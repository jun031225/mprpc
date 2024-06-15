#aux_source_directory(. SRC_LIST)

set(SRC_LIST 
    mprpcApplication.cc 
    mprpcConfig.cc 
    rpcHeader.pb.cc 
    rpcProvider.cc 
    mprpcChannel.cc
   )
#生成动态库文件
add_library(mprpc ${SRC_LIST})

target_link_libraries(mprpc muduo_net muduo_base pthread)