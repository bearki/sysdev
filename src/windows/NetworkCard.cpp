#include "NetworkCard.hpp"
#include "StringConvert.hpp"
#include <string>
#include <windows.h>
#include <cfgmgr32.h>
#include <devguid.h>
#include <handleapi.h>
#include <initguid.h>
#include <ntddndis.h>
#include <setupapi.h>
#ifdef __MINGW32__
#include <ddk/ndisguid.h>
#else
#include <ndisguid.h>
#endif // __MINGW32__

/**
 * @brief 获取网卡信息列表
 * @param list [out] 网卡信息列表
 * @return 状态码
 */
StatusCode NetworkCard::GetList(std::vector<NetworkCardInfo> &list)
{
    // 获取设备信息集句柄
    auto hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_NET, nullptr, nullptr, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (INVALID_HANDLE_VALUE == hDevInfo)
    {
        return StatusCode::StatusCode_ErrGetClassDevs;
    }

    // 枚举设备信息集中所有网络设备
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // 开始枚举
    for (DWORD index = 0; SetupDiEnumDeviceInterfaces(hDevInfo, nullptr, &GUID_DEVINTERFACE_NET, index, &deviceInterfaceData); ++index)
    {
        // 获取接收缓冲区大小
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &deviceInterfaceData, nullptr, 0, &requiredSize, nullptr);

        // 开辟接收缓冲区
        PSP_DEVICE_INTERFACE_DETAIL_DATA_A deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)malloc(requiredSize);
        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

        // 设备信息数据
        SP_DEVINFO_DATA devInfoData{};
        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        // 获取设备接口详细信息
        auto ok = SetupDiGetDeviceInterfaceDetailA(hDevInfo, &deviceInterfaceData, deviceInterfaceDetailData, requiredSize, nullptr, &devInfoData);
        if (!ok)
        {
            // 释放开辟的接收缓冲区
            free(deviceInterfaceDetailData);
            // 跳过
            continue;
        }

        // 构建网卡信息
        NetworkCardInfo info;
        // Windows下暂时不获取网卡名称，Windows7会有乱码问题
        info.netCardName = nullptr;
        // 检查设备路径中是否包含指定字符串
        std::string link_path = deviceInterfaceDetailData->DevicePath;
        if (link_path.find("pci") != link_path.npos)
        {
            // 是PCI网卡
            info.netCardType = NetworkCardType::NetworkCardType_PCI;
        }
        else if (link_path.find("usb") != link_path.npos)
        {
            // 是USB网卡
            info.netCardType = NetworkCardType::NetworkCardType_USB;
        }
        else
        {
            // 释放开辟的接收缓冲区
            free(deviceInterfaceDetailData);
            // 未知，跳过
            continue;
        }

        // 创建设备文件句柄
        HANDLE deviceFileHandle = INVALID_HANDLE_VALUE;
        deviceFileHandle = CreateFileA(deviceInterfaceDetailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (deviceFileHandle == INVALID_HANDLE_VALUE)
        {
            // 释放开辟的接收缓冲区
            free(deviceInterfaceDetailData);
            // 获取失败，跳过
            continue;
        }

        // 获取原生MAC地址
        DWORD dwID = OID_802_3_PERMANENT_ADDRESS;
        BYTE outBuffer[6] = {0};
        DWORD _bytesReturned;
        ok = DeviceIoControl(deviceFileHandle, IOCTL_NDIS_QUERY_GLOBAL_STATS,
                             &dwID, sizeof(dwID),
                             outBuffer, sizeof(outBuffer),
                             &_bytesReturned, nullptr);
        if (!ok)
        {
            // 释放开辟的接收缓冲区
            free(deviceInterfaceDetailData);
            // 获取失败，跳过
            continue;
        }

        // 赋值MAC地址
        memcpy(info.macAddress, outBuffer, 6);

        // 获取网卡名称（失败时忽略）
        {
            WCHAR deviceName[4096] = {0}; // 宽字符数组，大小应为 512 * 2 bytes
            DWORD deviceNameRequiredSize = 0;

            // 直接获取，失败就不要了
            ok = SetupDiGetDeviceRegistryPropertyW(
                hDevInfo,
                &devInfoData,
                SPDRP_FRIENDLYNAME,
                nullptr,
                (LPBYTE)deviceName, // LPBYTE 可以指向任何字节类型的数据
                sizeof(deviceName), // 缓冲区总大小（字节）
                &deviceNameRequiredSize);
            if (ok)
            {
                // 转换为UTF8编码
                auto deviceNameUtf8 = WStringToString(deviceName);
                // 拷贝名称
                info.netCardName = (char *)malloc(deviceNameUtf8.size() + 1);
                memcpy(info.netCardName, deviceNameUtf8.c_str(), deviceNameUtf8.size());
            }
        }

        // 释放开辟的接收缓冲区
        free(deviceInterfaceDetailData);

        // 添加到列表中
        list.insert(list.end(), info);
        // 关闭文件句柄
        CloseHandle(deviceFileHandle);
    }

    // OK
    return StatusCode::StatusCode_Success;
}
