#include <iostream>
#include <string>

#include "user.pb.h"
#include "mprpcApplication.h"
#include "rpcProvider.h"

class UserService : public fixbug::UserServiceRpc // rpc服务端
{
public:
    bool Login(std::string name, std::string pwd) // 本地业务方法
    {
        std::cout << "name:" << name << "  " << "pwd:" << " " << pwd << std::endl;
        if (!name.empty() && !pwd.empty())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Register(uint32_t id, std::string name, std::string pwd, std::string email)
    {
        std::cout << "id: " << id << ", name: " << name << ", pwd: " << pwd << ", email: " << email << std::endl;
        if (!id || id <= 0 || name.empty() || pwd.empty())
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    //.......

    // 重写基类Userservice虚函数，实现业务并回调
    void Login(::google::protobuf::RpcController *controller,
               const ::fixbug::LoginRequest *request,
               ::fixbug::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 根据请求request获取参数实现本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();
        bool login_result = Login(name, pwd);
        //......

        // 将响应写入response(errcode,errmsg,success, return value ......)
        fixbug::ResultCode *rc = response->mutable_rc();
        rc->set_errcode("-1");
        rc->set_errmsg("this is errmsg");
        response->set_success(login_result);
        //............

        // 执行回调操作(序列化，通过网络发送)
        done->Run();
    }

    void Register(::google::protobuf::RpcController *controller,
                  const ::fixbug::RegisterRequest *request,
                  ::fixbug::RegisterResponse *response,
                  ::google::protobuf::Closure *done)
    {
        //获取参数实现本地业务方法
        int id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();
        std::string email = request->email();
        bool register_result = Register(id,name,pwd,email);

        //=============================填写响应===================================

        fixbug::RegisterRequest *register_info = response->mutable_register_info();
        register_info->set_id(id);
        register_info->set_name(name);
        register_info->set_pwd(pwd);
        register_info->set_email(email);

        fixbug::ResultCode *rc = response->mutable_rc();
        if(register_result)
        {
            rc->set_errcode("0");
            rc->set_errmsg("");
        }
        else
        {
            rc->set_errcode("-1");
            rc->set_errmsg("Register() error !");
        }
        response->set_success(register_result);

        //回调
        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 框架初始化
    MprpcApplication::Init(argc, argv);

    // 通过RpcProvider将rpc服务对象发布到rpc节点
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动rpc服务发布节点，进入阻塞状态，等待远程rpc调用请求
    provider.run();
    return 0;
}