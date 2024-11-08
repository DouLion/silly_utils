/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-11
 * @file: silly_adcd.cpp
 * @description: silly_adcd实现
 * @version: v1.0.1 2024-10-11 dou li yang
 */
#include "silly_adcd.h"
#define SILLY_ADCD_FULL_0 "000000000000000"

#define SILLY_SHENG_LEN 2
#define SILLY_SHI_LEN 4
#define SILLY_XIAN_LEN 6
#define SILLY_XIANG_LEN 9
#define SILLY_CUN_LEN 12
#define SILLY_ZU_LEN 15
#define AD_APP_END 0b10000000

std::map<std::string, std::string> silly_adcd::cascade(const std::map<std::string, std::string>& adcd_adnm, const options& opt)
{


    bool sheng_in_name = opt.name_level << 1 & AD_APP_END;
    bool shi_in_name = opt.name_level << 2 & AD_APP_END;
    bool xian_in_name = opt.name_level << 3 & AD_APP_END;
    bool xiang_in_name = opt.name_level << 4 & AD_APP_END;
    bool cun_in_name = opt.name_level << 5 & AD_APP_END;
    bool out_sheng = opt.code_level << 1 & AD_APP_END;
    bool out_shi = opt.code_level << 2 & AD_APP_END;
    bool out_xian = opt.code_level << 3 & AD_APP_END;
    bool out_xiang = opt.code_level << 4 & AD_APP_END;
    bool out_cun = opt.code_level << 5 & AD_APP_END;
    bool out_zu = opt.code_level << 6 & AD_APP_END;

    for (auto [adcd, adnm] : adcd_adnm)
    {
        std::string n_adcd = adcd + SILLY_ADCD_FULL_0;
        n_adcd = n_adcd.substr(0, 15);
        int zero_num = 0;
        for (int i = n_adcd.size() - 1; i > 0; i--)
        {
            if (n_adcd[i] != '0')
            {
                break;
            }
            zero_num++;
        }
        if (zero_num >= 13 && sheng_in_name)
        {
            std_sheng_adnm[n_adcd.substr(0, SILLY_SHENG_LEN)] = adnm;
        }
        else if (zero_num >= 11 && shi_in_name)
        {
            std_shi_adnm[n_adcd.substr(0, SILLY_SHI_LEN)] = adnm;
        }
        else if (zero_num >= 9 && xian_in_name)
        {
            std_xian_adnm[n_adcd.substr(0, SILLY_XIAN_LEN)] = adnm;
        }
        else if (zero_num >= 6 && xiang_in_name)
        {
            std_xiang_adnm[n_adcd.substr(0, SILLY_XIANG_LEN)] = adnm;
        }
        else if (zero_num >= 3 && cun_in_name)
        {
            std_cun_adnm[n_adcd.substr(0, SILLY_CUN_LEN)] = adnm;
        }
        else if (zero_num == 0)
        {
            std_zu_adnm[n_adcd] = adnm;
        }
    }
    // 去除市名中包含的省名称
    if (shi_in_name)
    {
        for (auto& [adcd, adnm] : std_shi_adnm)
        {
            std::string sheng = std_sheng_adnm[adcd.substr(0, SILLY_SHENG_LEN)];
            if (sheng == adnm.substr(0, sheng.size()))  // 市名中包含了 省名
            {
                adnm = adnm.substr(sheng.size());
            }
        }
    }

    // 去除县名中包含的市名称
    if (xian_in_name)
    {
        for (auto& [adcd, adnm] : std_xian_adnm)
        {
            std::string shi = adcd.substr(0, SILLY_SHI_LEN);
            if (shi == adnm.substr(0, shi.size()))  // 县名中包含了 县名
            {
                adnm = adnm.substr(shi.size());
            }
        }
    }

    // 去除乡镇名中包含的县名
    if (xiang_in_name)
    {
        for (auto& [adcd, adnm] : std_xiang_adnm)
        {
            std::string xian = std_xian_adnm[adcd.substr(0, SILLY_XIAN_LEN)];
            if (xian == adnm.substr(0, xian.size()))  // 乡镇名中包含了 乡镇名
            {
                adnm = adnm.substr(xian.size());
            }
        }
    }

    // 去除村名中包含的乡镇名
    if (xiang_in_name)
    {
        for (auto& [adcd, adnm] : std_cun_adnm)
        {
            std::string xiang = std_xiang_adnm[adcd.substr(0, SILLY_XIANG_LEN)];
            if (xiang == adnm.substr(0, xiang.size()))  // 村名中包含了 村名
            {
                adnm = adnm.substr(xiang.size());
            }
        }
    }

    // 去除组名中包含的村名
    if (cun_in_name)
    {
        for (auto& [adcd, adnm] : std_zu_adnm)
        {
            std::string cun = std_cun_adnm[adcd.substr(0, SILLY_CUN_LEN)];
            if (cun == adnm.substr(0, cun.size()))  // 组名中包含了 村名
            {
                adnm = adnm.substr(cun.size());
            }
        }
    }
    // 拼接结果

    std::map<std::string, std::string> result;
    if (out_sheng)  // 输出省
    {
        for (auto [adcd, adnm] : std_sheng_adnm)
        {
            result[(adcd + SILLY_ADCD_FULL_0).substr(0, 15)] = adnm;
        }
    }

    if (out_shi)  // 输出市
    {
        for (auto [adcd, adnm] : std_shi_adnm)
        {
            if (sheng_in_name)
            {
                adnm = std_sheng_adnm[adcd.substr(0, SILLY_SHENG_LEN)] + adnm;
            }
        }
    }

    if (out_xian)  // 输出县
    {
        for (auto [adcd, adnm] : std_xian_adnm)
        {
            std::string name;
            if (sheng_in_name)
            {
                name.append(std_sheng_adnm[adcd.substr(0, SILLY_SHENG_LEN)]);
            }
            if (shi_in_name)
            {
                name.append(std_shi_adnm[adcd.substr(0, SILLY_SHI_LEN)]);
            }
            name.append(adnm);
            result[(adcd + SILLY_ADCD_FULL_0).substr(0, 15)] = name;
        }
    }
    if (out_xiang)  // 输出乡镇
    {
        for (auto [adcd, adnm] : std_xiang_adnm)
        {
            std::string name;
            if (sheng_in_name)
            {
                name.append(std_sheng_adnm[adcd.substr(0, SILLY_SHENG_LEN)]);
            }
            if (shi_in_name)
            {
                name.append(std_shi_adnm[adcd.substr(0, SILLY_SHI_LEN)]);
            }
            if (xian_in_name)
            {
                name.append(std_xian_adnm[adcd.substr(0, SILLY_XIAN_LEN)]);
            }
            name.append(adnm);
            result[(adcd + SILLY_ADCD_FULL_0).substr(0, 15)] = name;
        }
    }

    if (out_cun)  // 输出村
    {
        for (auto [adcd, adnm] : std_cun_adnm)
        {
            std::string name;
            if (sheng_in_name)
            {
                name.append(std_sheng_adnm[adcd.substr(0, SILLY_SHENG_LEN)]);
            }
            if (shi_in_name)
            {
                name.append(std_shi_adnm[adcd.substr(0, SILLY_SHI_LEN)]);
            }
            if (xian_in_name)
            {
                name.append(std_xian_adnm[adcd.substr(0, SILLY_XIAN_LEN)]);
            }
            if (xiang_in_name)
            {
                name.append(std_xiang_adnm[adcd.substr(0, SILLY_XIANG_LEN)]);
            }
            name.append(adnm);
            result[(adcd + SILLY_ADCD_FULL_0).substr(0, 15)] = name;
        }
    }

    if (out_zu)
    {
        for (auto [adcd, adnm] : std_zu_adnm)
        {
            std::string name;
            if (sheng_in_name)
            {
                name.append(std_sheng_adnm[adcd.substr(0, SILLY_SHENG_LEN)]);
            }
            if (shi_in_name)
            {
                name.append(std_shi_adnm[adcd.substr(0, SILLY_SHI_LEN)]);
            }
            if (xian_in_name)
            {
                name.append(std_xian_adnm[adcd.substr(0, SILLY_XIAN_LEN)]);
            }
            if (xiang_in_name)
            {
                name.append(std_xiang_adnm[adcd.substr(0, SILLY_XIANG_LEN)]);
            }
            if (cun_in_name)
            {
                name.append(std_cun_adnm[adcd.substr(0, SILLY_CUN_LEN)]);
            }
            name.append(adnm);
            result[(adcd + SILLY_ADCD_FULL_0).substr(0, 15)] = name;
        }
    }

    return result;
}
void silly_adcd::clean()
{
    std_sheng_adnm.clear();
    std_shi_adnm.clear();
    std_xian_adnm.clear();
    std_xiang_adnm.clear();
    std_cun_adnm.clear();
    std_zu_adnm.clear();
}
