#include"test.pb.h"
#include<iostream>
#include<string>


int main()
{
    fixbug::LoginResponse rspTo;
    fixbug::ResultCode *rc = rspTo.mutable_resultcode();
    rc->set_errmsg("登录失败");
    rc->set_errcode(-1);


    //对列表类型的处理
    fixbug::GetUsersListResponse rsp1;
    fixbug::ResultCode *rc1 = rsp1.mutable_resultcode();
    rc1->set_errcode(-1);
    rc1->set_errmsg("获取失败");

    fixbug::User *user1 = rsp1.add_users();
    user1->set_name("name01");
    user1->set_age(10);
    user1->set_sex(fixbug::User::MAN);

    fixbug::User *user2 = rsp1.add_users();
    user2->set_name("name02");
    user2->set_age(17);
    user2->set_sex(fixbug::User::WOMAN);

    std::cout << rsp1.users_size() << "users" << std::endl;
    std::cout << rsp1.users(0).name() << rsp1.users(0).age()<< rsp1.users(0).sex() << std::endl;
    std::cout << rsp1.users(1).name() << rsp1.users(1).age()<< rsp1.users(1).sex() << std::endl;
    return 0;
}

int main1()
{
    //封装请求消息数据
    fixbug::LoginRequest reqTo;
    reqTo.set_name("liaojun");
    reqTo.set_pwd("12345");

    //将对象数据序列化为send_str(char*)
    std::string send_str;
    if(reqTo.SerializeToString(&send_str))
    {
        std::cout << send_str << std::endl;
    }

    //将send_str(char*)反序列化为对象数据
    fixbug::LoginRequest reqFrom;

    if(reqFrom.ParseFromString(send_str))
    {
        std::cout << reqFrom.name() << std::endl;
        std::cout << reqFrom.pwd() << std::endl;
    }

    return 0;
}