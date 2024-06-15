#include "mprpcChannel.h"
#include "mprpcApplication.h"
#include "rpcHeader.pb.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

// head_size + head_str(service_name + method_name + args_size) + args
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                              google::protobuf::Message *response, google::protobuf::Closure *done)
{
    //==============================序列化rpc请求=======================================

    // 序列化请求参数
    uint32_t args_size = 0;
    std::string args_str;
    if (!request->SerializeToString(&args_str))
    {
        std::cout << "serialiae request error!" << std::endl;
        return;
    }

    // 获取待填入请求头中数据
    const google::protobuf::ServiceDescriptor *service = method->service();
    std::string service_name = service->name();
    std::string method_name = method->name();
    args_size = args_str.size();

    // 定义请求头，填入数据
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    // 序列化请求头
    std::string header_str;
    uint32_t header_size = 0;
    if (rpcHeader.SerializeToString(&header_str))
    {
        header_size = header_str.size();
    }
    else
    {
        std::cout << "serialize rpc header error!" << std::endl;
        return;
    }

    // 组织待发送的rpc请求(header_size + header_str(service_name + method_name + args_size) + args_str)
    //                   (header_size + rpcHeader + request)
    std::string send_str;
    send_str.insert(0, std::string((char *)&header_size, 4));
    send_str += header_str;
    send_str += args_str;

    // 调试信息
    std::cout << "==========================================" << std::endl;
    std::cout << "header_size:" << header_size << std::endl;
    std::cout << "header_str:" << header_str << std::endl;
    std::cout << "service_name:" << service_name << std::endl;
    std::cout << "method_name:" << method_name << std::endl;
    std::cout << "args_size:" << args_size << std::endl;
    std::cout << "args_str:" << args_str << std::endl;
    std::cout << "==========================================" << std::endl;

    //==========================网络编程，发送与接收================================

    // 创建套接字
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
    {
        std::cout << "socket() error! errno = " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // 读取配置文件信息（rpc服务器ip和port)
    std::string ip = MprpcApplication::getInstance().getConfig().load("rpcserver_ip");
    uint32_t port = atoi(MprpcApplication::getInstance().getConfig().load("rpcserver_port").c_str());

    // 配置服务端地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务
    if (connect(clientfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "connect() error! errno = " << errno << std::endl;
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    // 发送rpc请求
    if (send(clientfd, send_str.c_str(), send_str.size(), 0) == -1)
    {
        std::cout << "send() error! errno = " << errno << std::endl;
        close(clientfd);
        return;
    }

    // 接收rpc请求的响应
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if ((recv_size = recv(clientfd, recv_buf, 1024, 0)) == -1)
    {
        std::cout << "recv() error! errno = " << errno << std::endl;
        close(clientfd);
        return;
    }

    //=====================================反序列化rpc响应==================================

    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        std::cout << "response: " << recv_buf << "parse error!" << std::endl;
        close(clientfd);
        return;
    }

    /*
    std::string response_str(recv_buf, 0, recv_size);
    if (!response->ParseFromString(response_str))
    {
        std::cout << "response: " << response_str << "parse error!" << std::endl;
        close(clientfd);
        return;
    }
    */

    close(clientfd);
}