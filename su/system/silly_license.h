/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-25
 * @file: silly_license.h
 * @description: license 授权处理
 * @version: v1.0.1 2025-12-25 dou li yang
 */
#ifndef SILLY_LICENSE_H
#define SILLY_LICENSE_H
#include <system/silly_hardware.h>
#include <encode/silly_encrypt.h>
class suLicense
{
  public:
    /**
     * @brief 生成授权码, 服务端调用
     * @param user 用户名
     * @param id 内部ID
     * @param expire 到期时间
     * @return
     */
    static std::string GenAuthCode(const std::string& user, const std::string& id, const std::time_t& expire);

    /**
     * @brief 结合服务端提供的授权码 生产本地生成授权信息, 客户端调用
     * @param user 用户名, 与GenAuthCode中的一致
     * @param id 内部ID, 与GenAuthCode中的一致
     * @param auth GenAuthCode 生成的编码
     * @return
     */
    static std::string LocalMakeAuthed(const std::string& user, const std::string& id, const std::string& auth);

    /**
     * @brief 解析授权码,获取到期时间
     * @param user 用户名
     * @param id 唯一ID
     * @param authCode 授权码
     * @return
     */
    static std::time_t ExpireTime(const std::string& user, const std::string& id, const std::string& authCode);
};

// using suLicense = ReadBinFile;
#endif  // SILLY_LICENSE_H
