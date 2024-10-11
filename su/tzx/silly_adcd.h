/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-11
 * @file: silly_adcd.h
 * @description: silly_adcd 类声明
 * @version: v1.0.1 2024-10-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_ADCD_H
#define SILLY_UTILS_SILLY_ADCD_H
#include <su_marco.h>



class silly_adcd
{
  public:
    class options{
      public:
        // 结果名称中的最高区划等级  如果是 3 最后输出的 结果是 xx县xx乡xx村
        int name_max_level = 3; // 省 1 市 2 县 3 乡镇 4 村 5 自然村 6
        // 结果输出的等级
        int result_max_level = 6;
    };
  public:
    static std::map<std::string, std::string> cascade(const std::map<std::string, std::string>& adcd_adnm, const options& opt);
};

#endif  // SILLY_UTILS_SILLY_ADCD_H
