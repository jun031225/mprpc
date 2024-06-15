#include "mprpcConfig.h"

#include <iostream>
#include <string>

void MprpcConfig::parseConfigFile(const char *configFile)
{
    FILE *pf = fopen(configFile, "r");
    if (pf == nullptr)
    {
        std::cout << "configFile: " << configFile << " dosen't exists!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 处理配置文件(注释，空格，配置项)
    // 循环读取每一行直到文件末尾
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf); // 读取一行字符
        std::string str_buf(buf);

        trim(str_buf);
        // 解析注释
        // 这一行为注释或者全为空格
        if (str_buf[0] == '#' || str_buf.empty())
        {
            continue;
        }

        // 解析配置项
        int idx = str_buf.find('=');
        if ( idx== -1)
        {
            continue;
        }

        std::string key;
        std::string value;

        //提取key
        key = str_buf.substr(0, idx);
        trim(key);

        //提取value
        int endidx = str_buf.find('\n', idx);
        value = str_buf.substr(idx + 1, endidx - idx - 1);
        trim(value);

        configMap_.insert({key, value});
    }
}

std::string MprpcConfig::load(const std::string &key)
{
    auto it = configMap_.find(key);
    // std::cout << it->first << std::endl;
    if (it == configMap_.end())
    {
        return "NO VALUE";
    }
    return it->second;
}

void MprpcConfig::trim(std::string &str_buf)
{
    int idx = 0;

    // 解析空格
    // 去除前面的空格
    if ((idx = str_buf.find_first_not_of(' ')) != -1)
    {
        str_buf = str_buf.substr(idx, str_buf.size() - idx);
    }

    // 去除后面的空格
    if ((idx = str_buf.find_last_not_of(' ')) != -1)
    {
        str_buf = str_buf.substr(0, idx + 1);
    }
}