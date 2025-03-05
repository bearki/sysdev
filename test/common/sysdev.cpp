#include <sysdev/sysdev.h>

#include <iostream>

int main()
{
    NetworkCardInfo *info = nullptr;
    size_t size = 0;
    auto code = SysDevGetNetworkCardInfo(&info, &size);
    if (code != StatusCode::StatusCode_Success)
    {
        std::cerr << "Failed to get network card info: " << code << std::endl;
        return -1;
    }

    std::cout << "\nNetworkCardInfo: " << std::endl;
    for (size_t i = 0; i < size; i++)
    {
        auto item = info[i];
        char address[18] = {0};
        for (size_t i = 0; i < sizeof(item.macAddress); i++)
        {
            sprintf(address + i * 3, i == sizeof(item.macAddress) - 1 ? "%02x" : "%02x:", item.macAddress[i]);
        }

        // 检查网卡名称是否为空
        if (item.netCardName != nullptr)
        {
            std::cout << "\tName: " << item.netCardName;
        }

        // 打印信息
        std::cout << "\t | Type: " << item.netCardType
                  << "\t | MacAddress: " << address
                  << std::endl;
    }

    // getchar();
}