#include "NetworkCard.hpp"

/**
 * @brief 构造函数
 */
NetworkCard::NetworkCard() {}

/**
 * @brief 析构函数
 */
NetworkCard::~NetworkCard() {
    // 释放设备信息集句柄
    if (this->deviceInfoSet != INVALID_HANDLE_VALUE) {
        SetupDiDestroyDeviceInfoList(this->deviceInfoSet);
    }
}

/**
 * @brief 获取网卡信息列表
 * @param list [out] 网卡信息列表
 * @return 状态码
 */
StatusCode NetworkCard::GetList(std::vector<NetworkCardInfo> &list) {
    // 获取设备信息集句柄
    auto hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_NET, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (INVALID_HANDLE_VALUE == hDevInfo) {
        return StatusCode::StatusCode_ErrGetClassDevs;
    }

    // 枚举设备信息集中所有网络设备
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // 开始枚举
    for (DWORD index = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_NET, index, &deviceInterfaceData); ++index) {
        // 获取接收缓冲区大小
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

        // 开辟接收缓冲区
        PSP_DEVICE_INTERFACE_DETAIL_DATA_A deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)malloc(requiredSize);
        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

        // 获取设备接口详细信息
        auto ok = SetupDiGetDeviceInterfaceDetailA(hDevInfo, &deviceInterfaceData, deviceInterfaceDetailData, requiredSize, NULL, NULL);
        if (!ok) {
            // 释放开辟的接收缓冲区
            free(deviceInterfaceDetailData);
            // 跳过
            continue;
        }

        // 构建网卡信息
        NetworkCardInfo info;
        // 检查设备路径中是否包含指定字符串
        std::string link_path = deviceInterfaceDetailData->DevicePath;
        std::string::size_type pci_pos = link_path.find("pci");
        std::string::size_type usb_pos = link_path.find("usb");
        if (pci_pos != link_path.npos) {
            // 是PCI网卡
            info.netCardType = NetworkCardType::NetworkCardType_PCI;
        } else if (usb_pos != link_path.npos) {
            // 是USB网卡
            info.netCardType = NetworkCardType::NetworkCardType_USB;
        } else {
            // 释放开辟的接收缓冲区
            free(deviceInterfaceDetailData);
            // 未知，跳过
            continue;
        }

        // 获取网卡的MAC地址
        HANDLE deviceFileHandle = INVALID_HANDLE_VALUE;
        deviceFileHandle = CreateFileA(deviceInterfaceDetailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (deviceFileHandle == INVALID_HANDLE_VALUE) {
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
                             &_bytesReturned, NULL);
        if (ok) {
            // 赋值MAC地址
            for (size_t i = 0; i < 6; i++) {
                info.macAddress[i] = outBuffer[i];
            }
            // 添加到列表中
            list.insert(list.end(), info);
        }

        // 关闭文件句柄
        CloseHandle(deviceFileHandle);
    }

    // OK
    return StatusCode::StatusCode_Success;
}