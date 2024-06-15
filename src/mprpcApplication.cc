#include "mprpcApplication.h"

#include <iostream>
#include <unistd.h>

// 静态成员变量初始化
MprpcConfig MprpcApplication::config_;

MprpcConfig& MprpcApplication::getConfig()
{
    return config_;
}

void showArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

MprpcApplication &MprpcApplication::getInstance()
{
    static MprpcApplication app;
    return app;
}

//初始化(解析加载配置文件)
void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        showArgsHelp();
        exit(EXIT_FAILURE);
    }

    int opt = 0;
    std::string config_file;

    // 获取配置文件
    while ((opt = getopt(argc, argv, "i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            if (optopt == 'i')
            {
                std::cout << "requires an argument" << std::endl;
                showArgsHelp();
            }
            else
            {
                std::cout << "invalid args!" << std::endl;
                showArgsHelp();
            }
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 解析并加载配置文件 rpc，zookeeper (ip，port)
    MprpcApplication::config_.parseConfigFile(config_file.c_str());

    //std::cout << "rpcserver_ip:" << MprpcApplication::config_.load("rpcserver_ip") << std::endl;
    //std::cout << "rpcserver_port:" << MprpcApplication::config_.load("rpcserver_port") << std::endl;
    //std::cout << "zookeeper_ip:" << MprpcApplication::config_.load("zookeeper_ip") << std::endl;
    //std::cout << "zookeeper_port:" << MprpcApplication::config_.load("zookeeper_port") << std::endl;
}