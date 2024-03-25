#include "NetworkCard.hpp"

/**
 * @brief 获取网卡信息
 *
 * @param pInfo [out] 网卡信息列表
 * @param size [out] 网卡信息列表大小
 * @return 状态码
 */
StatusCode SysDevGetNetworkCardInfo(NetworkCardInfo **pInfo, size_t *size) {
    // 检查
    if (pInfo == nullptr || size == nullptr) {
        return StatusCode::StatusCode_ErrInputParam;
    }

    // 获取网卡信息列表
    std::vector<NetworkCardInfo> list;
    auto networkCard = NetworkCard();
    auto code = networkCard.GetList(list);
    if (code != StatusCode::StatusCode_Success) {
        return code;
    }

    // 转换结果列表
    *size = list.size();
    *pInfo = new NetworkCardInfo[list.size()];
    memcpy(*pInfo, list.data(), list.size() * sizeof(NetworkCardInfo));

    // OK
    return StatusCode_Success;
}

/**
 * @brief 释放网卡信息
 *
 * @param pInfo [in] 网卡信息列表
 * @param size [in] 网卡信息列表大小
 * @return 状态码
 */
StatusCode SysDevFreeNetworkCardInfo(NetworkCardInfo *pInfo, size_t size) {
    // 检查
    if (pInfo == nullptr || size == 0) {
        return StatusCode::StatusCode_Success;
    }

    // 释放整个列表
    delete[] pInfo;
    pInfo = nullptr;

    // OK
    return StatusCode::StatusCode_Success;
}