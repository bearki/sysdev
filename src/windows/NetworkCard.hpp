#pragma once

#ifndef _NETWORK_CARD_INFO_HPP_
#define _NETWORK_CARD_INFO_HPP_

#include <sysdev/sysdev.h>
#include <vector>

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
