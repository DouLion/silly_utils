/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: silly_stbprp.h
 * @description: silly_stbprp 测站基本信息
 * @version: v1.0.1 2024-09-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_STBPRP_H
#define SILLY_UTILS_SILLY_STBPRP_H

class silly_stbprp
{
  public:
    std::string serialize() const;
    bool deserialize(const std::string& data);

  public:
    uint32_t index{0};
    std::string STCD;
    std::string STNM;
    std::string RVNM;
    std::string HNNM;
    std::string BSNM;
    double LGTD{0.};
    double LTTD{0.};
    std::string STLC;
    std::string ADDVCD;
    std::string DTMNM;
    double DTMEL{0.};
    double DTPR{0.};
    std::string STTP;
    std::string FRGRD;
    std::string ESSTYM;
    std::string BGFRYM;
    std::string ATCUNIT;
    std::string ADMAUTH;
    std::string LOCALITY;
    std::string STBK;
    int STAzt{0};
    double DSTRVM{0.};
    int DRNA{0};
    std::string PHCD;
    std::string USFL;
    std::string COMMENTS;
    std::string MODITIME;
    std::string HNNM0;
    std::string ADCD;
    std::string ADDVCD1;
    static constexpr const char* FILE_NAME = "STBPRP.dat";
};

#endif  // SILLY_UTILS_SILLY_STBPRP_H
