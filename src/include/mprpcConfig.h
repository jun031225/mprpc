#pragma once

#include<unordered_map>
#include<string>

//配置文件类
//rpcServer_ip, rpcServer_port, zookeeper_ip, zookeeper_port

class MprpcConfig
{
    public:
    //解析加载配置文件
    void  parseConfigFile(const char *configFile);
    //查询配置项信息
    std::string load(const std::string &key);
    private:
    std::unordered_map<std::string, std::string> configMap_;

    //去除字符串前后空格
    void trim(std::string &str_buf);
};