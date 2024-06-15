
#include <iostream>

#include "mprpcApplication.h"
#include "mprpcChannel.h"
#include "friend.pb.h"

int main(int argc, char **argv)
{
    // 初始化，解析配置文件
    MprpcApplication::Init(argc, argv);

    fixbug::friendServiceRpc_Stub stub(new MprpcChannel()); // 调用callMethod

    // 填写request
    fixbug::GetFriendsRequest get_request;

    get_request.set_id(111);

    fixbug::GetFriendsResponse get_respone;

    // rpc方法调用，实质是调用RpcChannel的CallMethod方法(序列化->发送->等待响应->接收->反序列化)  socket编程 + protobuf
     stub.GetFriends(nullptr, &get_request, &get_respone, nullptr); // RpcChannel调用CallMethod

    
    //channel_->CallMethod(descriptor()->method(0),controller, request, response, done);

    

    // 对响应处理
    fixbug::ResultCode *rc = get_respone.mutable_rc();
    if(rc->errcode() != "0")
    {
        std::cout << rc->errmsg() << " errorcode: " << rc->errcode() << std::endl;
    }
    else
    {
        
        int size = get_respone.friend__size();
        std::cout <<"friend get success: " << std::endl;
        
        for(int i = 0; i < size; i++)
        {
            std::cout << "name: " << get_respone.mutable_friend_(i)->name() << ", email: " << get_respone.mutable_friend_(i)->email() << std::endl;
        }
    }
    
    return 0;
}