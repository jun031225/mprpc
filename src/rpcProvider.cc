#include "rpcProvider.h"
#include "mprpcApplication.h"
#include "rpcHeader.pb.h"

#include <string>
#include <functional>
//#include <google/protobuf/descriptor.h>

// 注册rpc服务的函数接口(发布rpc服务)
void RpcProvider::NotifyService(::google::protobuf::Service *service)
{
    ServiceInfo service_info;
    // 获取服务对象描述信息
    const google::protobuf::ServiceDescriptor *serviceDescPtr = service->GetDescriptor();

    // 服务对象名称
    std::string service_name = serviceDescPtr->name();

    std::cout << "service_name: " << service_name << std::endl;

    // 服务对象方法数量
    int methodCnt = serviceDescPtr->method_count();

    for (int i = 0; i < methodCnt; i++)
    {
        // 获取服务对象方法的描述
        const google::protobuf::MethodDescriptor *methodDescPtr = serviceDescPtr->method(i);
        std::string method_name = methodDescPtr->name();
        std::cout << "method_name :" << method_name << std::endl;
        service_info.methodMap_.insert({method_name, methodDescPtr});
    }
    service_info.service_ = service;
    serviceMap_.insert({service_name, service_info});
}

void RpcProvider::run()
{
    // 解析配置文件并构建InetAddress对象
    std::string ip = MprpcApplication::getInstance().getConfig().load("rpcserver_ip");
    uint16_t port = atoi(MprpcApplication::getInstance().getConfig().load("rpcserver_port").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&eventLoop_, address, "rpcProvider");
    // 绑定连接回调和消息回调方法
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this,
                                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置线程数量
    server.setThreadNum(4);

    std::cout << "RpcProvider start service at ip:" << ip << "  port:" << port << std::endl;
    // 启动网络服务
    server.start();
    eventLoop_.loop();
}

// socket连接回调函数
void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

// 读写事件回调函数
// recv_buf(header_size(4字节) + head_str + args_str)
// headr_str(service_name, method_name, args_size)
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buffer, muduo::Timestamp timeStamp)
{
    //=================================接收rpc请求，反序列化请求头=========================================
    // 接收字符流
    std::string recv_buf = buffer->retrieveAllAsString();

    // 提取head_siz(数据头大小)
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 提取head_str字符流,进行反序列化，得到rpc请求信息
    std::string header_str = recv_buf.substr(4, header_size);

    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    //反序列化rpc请求头
    if (rpcHeader.ParseFromString(header_str)) 
    {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        std::cout << "head_str: " << header_str << "parse error!" << std::endl;
        return;
    }

    // 提取参数字符流
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "header_str: " << header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "============================================" << std::endl;

    //=====================================获取servie和method=====================================
    // 获取service对象
    auto it = serviceMap_.find(service_name);
    if (it == serviceMap_.end())
    {
        std::cout << "service_name: " << method_name << " dosen't exists!" << std::endl;
        return;
    }
    google::protobuf::Service *service = it->second.service_;

    // 获取method对象
    auto m_it = it->second.methodMap_.find(method_name);
    if (m_it == it->second.methodMap_.end())
    {
        std::cout << "method_name: " << method_name << " dosen't exists!" << std::endl;
    }
    const google::protobuf::MethodDescriptor *method = m_it->second;

    //=========================================生成request和response====================================

    // 生成RPC方法调用的request和response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    
    //反序列化rpcc请求参数
    if (!request->ParseFromString(args_str))
    {
        std::cout << "args:" << args_str << " parse error!" << std::endl;
        return;
    }

    //rpc发布方填充
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();


    //=========================================绑定回调函数，执行CallMethod===================================

    // 绑定Closure的回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback
    <RpcProvider, const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>
    (this, &RpcProvider::SendRpcResponse, conn, response);
    

    // 根据rpc请求调用当前rpc节点上发布的方法S
    service->CallMethod(method, nullptr, request, response, done);
}

// Closure的回调函数，用于序列化rpc和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if (!response->SerializeToString(&response_str))// response序列化
    {
        std::cout << "  serialize response_str error!" << std::endl;
    }
    else
    {
        // 通过网络发送回rpc调用方
        conn->send(response_str);
    }
    conn->shutdown();//关闭http连接
}
