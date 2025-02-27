#pragma once

#include <windows.h>

// 傻逼
#include <cfgmgr32.h>
#include <devguid.h>
#include <handleapi.h>
#include <initguid.h>
#include <ntddndis.h>
#include <setupapi.h>
#include <sysdev/sysdev.h>

#include <string>

#ifdef __MINGW32__
#include <ddk/ndisguid.h>
#else
#include <ndisguid.h>
#endif   // __MINGW32__

#include <vector>

#ifndef _NETWORK_CARD_INFO_HPP_
#define _NETWORK_CARD_INFO_HPP_

/**
 * @brief 网卡信息
 */
class NetworkCard {
public:
    /**
     * @brief 获取网卡信息列表
     * @param list [out] 网卡信息列表
     * @return 状态码
     */
    static StatusCode GetList(std::vector<NetworkCardInfo> &list);
};

#endif
