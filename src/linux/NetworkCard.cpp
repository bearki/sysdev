#include "NetworkCard.hpp"
#include <string>
#include <dirent.h>
#include <fstream>
#include <ifaddrs.h>
#include <string.h>
#include <filesystem>
#include <unistd.h>

/**
 * @brief 获取网卡类型
 * @param interfaceName [in] 网卡名称
 * @param interfaceType [out] 网卡类型
 * @return 是否获取成功
 */
bool GetNetworkCardType(const std::string &interfaceName, NetworkCardType &interfaceType)
{
    // 网卡设备所在目录
    std::string devicePath = "/sys/class/net/" + interfaceName + "/device";
    // 读取子系统软链路径
    std::string subsystemPath = devicePath + "/subsystem";
    const size_t subsystemAbsPathMaxSize = 1024;
    char subsystemAbsPath[subsystemAbsPathMaxSize];
    auto readSize = readlink(subsystemPath.c_str(), subsystemAbsPath, subsystemAbsPathMaxSize);
    if (readSize <= 0)
    {
        return false;
    }
    subsystemAbsPath[readSize] = '\0';

    // 判断设备类型
    std::string subsystemStr(subsystemAbsPath);
    if (subsystemStr.find("pci") != std::string::npos)
    {
        interfaceType = NetworkCardType::NetworkCardType_PCI;
        return true;
    }
    else if (subsystemStr.find("usb") != std::string::npos)
    {
        interfaceType = NetworkCardType::NetworkCardType_USB;
        return true;
    }

    // 默认未查询到有效值
    return false;
}

/**
 * @brief 获取网卡MAC地址
 * @param interfaceName [in] 网卡名称
 * @param interfaceMacAddress [out] 网卡MAC地址
 */
bool GetMacAddress(const std::string &interfaceName, unsigned char (&interfaceMacAddress)[6])
{
    // 打开MAC地址文件
    std::string addressPath = "/sys/class/net/" + interfaceName + "/address";
    std::ifstream addressFile(addressPath);
    if (!addressFile)
    {
        return false;
    }

    // 读取MAC地址
    std::string macStr;
    std::getline(addressFile, macStr);
    if (macStr.length() != 17)
    {
        return false;
    }

    // 拷贝MAC地址
    macStr = macStr.substr(0, 17);
    for (size_t i = 0; i < 6; ++i)
    {
        sscanf(macStr.substr(i * 3, 2).c_str(), "%02hhx", &interfaceMacAddress[i]);
    }

    // OK
    return true;
}

/**
 * @implements 实现获取网卡信息列表
 */
StatusCode NetworkCard::GetList(std::vector<NetworkCardInfo> &list)
{
    // 获取所有网卡
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1)
    {
        return StatusCode::StatusCode_ErrGetClassDevs;
    }

    // 遍历所有网卡
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        // 检查网卡基础信息
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_PACKET)
        {
            continue;
        }
        // 声明网卡信息
        NetworkCardInfo info;
        // 获取网卡类型
        auto ok = GetNetworkCardType(ifa->ifa_name, info.netCardType);
        if (!ok)
        {
            // 跳过失败的
            continue;
        }
        // 获取网卡MAC地址
        ok = GetMacAddress(ifa->ifa_name, info.macAddress);
        if (!ok)
        {
            // 跳过失败的
            continue;
        }
        // 复制网卡名称
        size_t netCardNameSize = sizeof(char) * strlen(ifa->ifa_name);
        info.netCardName = (char *)malloc(netCardNameSize);
        memcpy(info.netCardName, ifa->ifa_name, netCardNameSize);
        // 添加到响应
        list.push_back(info);
    }

    // 释放网卡列表
    freeifaddrs(ifaddr);
    // OK
    return StatusCode::StatusCode_Success;
}
