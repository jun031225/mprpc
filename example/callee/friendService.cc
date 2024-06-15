#include <iostream>
#include <string>

#include "friend.pb.h"
#include "mprpcApplication.h"
#include "rpcProvider.h"

class FriendService : public fixbug::friendServiceRpc
{
public:
    bool GetFriends(uint32_t id)
    {
        
        if (!id || id <= 0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    void GetFriends(::google::protobuf::RpcController *controller,
                    const ::fixbug::GetFriendsRequest *request,
                    ::fixbug::GetFriendsResponse *response,
                    ::google::protobuf::Closure *done)
    {
        uint32_t id = request->id();
        bool result = GetFriends(id);

        fixbug::ResultCode *rc = response->mutable_rc();

        if(!result)
        {
            rc->set_errcode("1");
            rc->set_errmsg("GetFriends() error!");
        }

        rc->set_errcode("0");
        rc->set_errmsg("");
        fixbug::Friend *friend_1 = response->add_friend_();
        friend_1->set_id(1);
        friend_1->set_name("DingZhen");
        friend_1->set_email("DingZhen@Weixin.sc");
        
        fixbug::Friend *friend_2 = response->add_friend_();
        friend_2->set_id(2);
        friend_2->set_name("WangYuan");
        friend_2->set_email("WangYuan@Shoujiu.cq");

        fixbug::Friend *friend_3 = response->add_friend_();
        friend_3->set_name("KunKun");
        friend_3->set_email("KunKun@basketball.kk");

        done->Run();

    }

private:

};

int main(int argc, char **argv)
    {
        //初始化，解析配置文件
        MprpcApplication::Init(argc, argv);

        //发布rpc服务，即将服务和方法插入map
        RpcProvider provider;
        provider.NotifyService(new FriendService());

        //MessageCallback(接收，反序列化，本地调用处理，序列化，发送) (muduo + protobuf)
        provider.run();

        return 0;

    }