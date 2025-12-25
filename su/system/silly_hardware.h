/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-25
 * @file: silly_hardware.h
 * @description: 硬件相关
 * @version: v1.0.1 2025-12-25 dou li yang
 */
#ifndef SILLY_HARDWARE_H
#define SILLY_HARDWARE_H
#include <su_marco.h>
class suHardWare
{
    public:
    /**
     * @brief 获取Mac地址
     * @return
     */
    static std::map<std::string, std::string> MacInfo();

};

#endif  // SILLY_HARDWARE_H
