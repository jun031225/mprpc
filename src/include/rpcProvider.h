#pragma once

#include "google/protobuf/service.h"

#include<string>
#include<unordered_map>


#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpServer.h>
#include<muduo/net/TcpConnection.h>
#include<muduo/net/Buffer.h>
#include<muduo/base/Timestamp.h>



// 发布rpc服务的网络对象
class RpcProvider
{
public:
    // 发布rpc方法的函数接口
    void NotifyService(::google::protobuf::Service *service);

    // 启动rpc服务节点
    void run();

private:

    muduo::net::EventLoop eventLoop_;

    //储存service服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *service_;//服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> methodMap_;//服务方法对象
    };
    std::unordered_map<std::string, ServiceInfo> serviceMap_;

    //socket连接对调函数
    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    //读写事件回调函数
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer *buffer, muduo::Timestamp timeStamp);

    //rpc响应回调
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message*rsponse );
};