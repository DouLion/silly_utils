/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-28
 * @file: SLBDataV1.cpp
 * @description: SLBDataV1实现
 * @version: v1.0.1 2025-02-28 dou li yang
 */
#include "SLBDataV1.h"

using namespace radar;

bool SLBDataV1::convert(const HunanData& src)
{
    bool status = false;
    std::string emptyStr="";
    // ---- 公共数据块 ----

    //fileVol.VolumeLabel = ;
    //fileVol.VersionNo = ;
    fileVol.FileLength = 10;
    fileVol.RayOrder = PitchFirst;

    //siteInfo.Country =
    //siteInfo.Province =
    //siteInfo.SiteName = src.m_Header.PerformanceInfo.SiteName;





    // ---- 径向数据块 ----


    return status;
}
