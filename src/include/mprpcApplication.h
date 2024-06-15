#pragma once


#include "mprpcConfig.h"
// 框架基础类（单例）
class MprpcApplication
{
public:
    static MprpcApplication &getInstance();

    static void Init(int argc, char **argv);

    static MprpcConfig& getConfig();

private:

     static MprpcConfig config_;
    MprpcApplication() {}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete;
    MprpcApplication &operator=(const MprpcApplication &) = delete;
};