#include<iostream>

#include"mprpcApplication.h"
#include"user.pb.h"
#include"mprpcChannel.h"

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());


    //=============================Login=========================================

    //填充请求参数
    fixbug::LoginRequest login_request;
    login_request.set_name("jj");
    login_request.set_pwd("12345");

    //构建响应消息对象
    fixbug::LoginResponse login_response;

    //发起rpc调用(MrpcChannel::CallMethod())
    stub.Login(nullptr, &login_request, &login_response, nullptr);

    //读取调用返回的响应
    if(login_response.rc().errcode() == "0")
    {
        std::cout << "rpc login response error:" << login_response.rc().errmsg() << std::endl;
    }
    else
    {
        std::cout << "rpc login response success:" << login_response.success() << std::endl;
    }

    //==============================================Register=================================

    //填充请求参数
    fixbug::RegisterRequest register_request;
    register_request.set_id(001);
    register_request.set_name("DingZhen");
    register_request.set_pwd("shuozhanghuale");
    register_request.set_email("9999@Dingzhen.sc");

    //构建响应对象
    fixbug::RegisterResponse register_response;

    //发起rpc方法调用
    stub.Register(nullptr, &register_request,&register_response,nullptr);


    //对响应实现业务

    if(register_response.mutable_rc()->errcode() == "0")
    {
        std::cout << "register() behavior:" << register_response.success() << std::endl;
        
        fixbug::RegisterRequest *register_info = register_response.mutable_register_info();
        std::cout << "register info:" << std::endl;
        std::cout << "id: " << register_info->id() << ", name: " << register_info->name() << ", password: " << register_info->pwd() 
        << ", email: "  << register_info->email() << std::endl;

    }
    else
    {
        fixbug::ResultCode *rc = register_response.mutable_rc();
        std::cout << rc->errmsg() << ", errorcode: " << rc->errcode() << std::endl; 
    }
    

    return 0;
}