#pragma once

#ifndef _SYSDEV_H_
#define _SYSDEV_H_

// 引入必要头文件
#include <stdint.h>
#include <stddef.h>

// StatusCode 状态码
typedef enum
{
    StatusCode_Success,             // 成功
    StatusCode_ErrInputParam,       // 传入参数错误
    StatusCode_ErrGetClassDevs,     // 获取设备信息集句柄失败
} StatusCode;

// NetworkCardType 网卡类型
typedef enum
{
    NetworkCardType_PCI,     // 网卡类型：PCI网卡
    NetworkCardType_USB,     // 网卡类型：USB网卡
} NetworkCardType;

// NetworkCardInfo 网卡信息
typedef struct
{
    NetworkCardType netCardType; // 网卡类型
    uint8_t macAddress[6];       // 网卡MAC地址
    char *netCardName;           // 网卡名称
} NetworkCardInfo;

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief 获取网卡信息
     *
     * @param pInfo [out] 网卡信息列表
     * @param size [out] 网卡信息列表大小
     * @return 状态码
     */
    StatusCode SysDevGetNetworkCardInfo(NetworkCardInfo **pInfo, size_t *size);

    /**
     * @brief 释放网卡信息
     *
     * @param pInfo [in] 网卡信息列表
     * @param size [in] 网卡信息列表大小
     * @return 状态码
     */
    StatusCode SysDevFreeNetworkCardInfo(NetworkCardInfo *pInfo, size_t size);

#ifdef __cplusplus
}
#endif

#endif