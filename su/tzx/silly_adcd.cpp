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

std::map<std::string, std::string> silly_adcd::cascade(const std::map<std::string, std::string>& adcd_adnm, const options& opt)
{
    std::map<std::string, std::string> std_adcd_adnm; // 归一化后的adcd_adnm  省2市2县2乡镇3行政村3自然村3
    std::map<std::string, std::string> std_sheng_adnm;
    std::map<std::string, std::string> std_shi_adnm;
    std::map<std::string, std::string> std_xian_adnm;
    std::map<std::string, std::string> std_xiang_adnm;
    std::map<std::string, std::string> std_cun_adnm; // 村
    std::map<std::string, std::string> std_zu_adnm; // 组

    for(auto [adcd, adnm]: adcd_adnm)
    {
        std::string n_adcd = adcd + SILLY_ADCD_FULL_0;
        n_adcd = n_adcd.substr(0, 15);
        std_adcd_adnm[n_adcd] = adnm;
        int zero_num = 0;
        for(int i =  n_adcd.size() - 1; i > 0; i--)
        {
           if (n_adcd[i] != '0')
           {
               break;
           }
           zero_num++;
        }
        if(zero_num >= 13 && opt.name_max_level < 2)
        {
            std_sheng_adnm[n_adcd.substr(0, 2)] = adnm;
        }
        else if(zero_num >= 11 && opt.name_max_level < 3)
        {
            std_shi_adnm[n_adcd.substr(0, 4)] = adnm;
        }
        else if(zero_num >= 9 && opt.name_max_level < 4)
        {
            std_xian_adnm[n_adcd.substr(0, 6)] = adnm;
        }
        else if(zero_num >= 6 && opt.name_max_level < 5)
        {
            std_xiang_adnm[n_adcd.substr(0, 9)] = adnm;
        }
        else if(zero_num >= 3 && opt.name_max_level < 6)
        {
            std_cun_adnm[n_adcd.substr(0, 12)] = adnm;
        }
        else if(zero_num == 0)
        {
            std_zu_adnm[n_adcd] = adnm;
        }
    }

    std::map<std::string, std::string> result;
    for(auto& [adcd, adnm]: std_zu_adnm)
    {
        std::string sheng = std_sheng_adnm[adcd.substr(0, 2)];
        std::string shi = std_shi_adnm[adcd.substr(0, 4)];
        std::string xian = std_xian_adnm[adcd.substr(0, 6)];
        std::string xiang = std_xiang_adnm[adcd.substr(0, 9)];
        std::string cun = std_cun_adnm[adcd.substr(0, 12)];
        if(cun == adnm.substr(0, cun.size())) // 自然村名中包含了 行政村名
        {
            result[adcd] = sheng + shi + xian + xiang + adnm;
        }
        else
        {
            result[adcd] = sheng + shi + xian + xiang + cun + adnm;
        }
    }

    if(opt.result_max_level == 6)
    {
        return result;
    }

    for(auto &[adcd, adnm]: std_cun_adnm)
    {
        std::string sheng = std_sheng_adnm[adcd.substr(0, 2)];
        std::string shi = std_shi_adnm[adcd.substr(0, 4)];
        std::string xian = std_xian_adnm[adcd.substr(0, 6)];
        std::string xiang = std_xiang_adnm[adcd.substr(0, 9)];
        result[adcd] = sheng + shi +xian + xiang + adnm;
    }

    if(opt.result_max_level == 5)
    {
        return result;
    }

    for(auto &[adcd, adnm]: std_xiang_adnm)
    {
        std::string sheng = std_sheng_adnm[adcd.substr(0, 2)];
        std::string shi = std_shi_adnm[adcd.substr(0, 4)];
        std::string xian = std_xian_adnm[adcd.substr(0, 6)];
        result[adcd] = sheng + shi + xian + adnm;
    }

    if(opt.result_max_level == 4)
    {
        return result;
    }

    for (auto &[adcd, adnm]: std_xian_adnm)
    {
        std::string sheng = std_sheng_adnm[adcd.substr(0, 2)];
        std::string shi = std_shi_adnm[adcd.substr(0, 4)];
        result[adcd] = sheng + shi + adnm;
    }

    if(opt.result_max_level == 3)
    {
        return result;
    }

    for(auto &[adcd, adnm]: std_shi_adnm)
    {
        std::string sheng = std_sheng_adnm[adcd.substr(0, 2)];
        result[adcd] = sheng + adnm;
    }

    if(opt.result_max_level == 2)
    {
        return result;
    }

    for(auto &[adcd, adnm]: std_sheng_adnm)
    {
        result[adcd] = adnm;
    }
    return result;

}