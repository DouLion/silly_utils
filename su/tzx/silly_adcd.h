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



#define AD_DA 0x00000001
#define AD_ZRC 0b00000010
#define AD_CUN 0b00000100
#define AD_XIANG 0b00001000
#define AD_XIAN 0b00010000
#define AD_SHI 0b00100000
#define AD_SHENG 0b01000000
#define AD_ALL 0b01111111

class silly_adcd
{
    // TODO: 目前危险区尚未支持, 后面需要补充危险区的处理方式
    // TODO: 实现方式应该可以更灵活一些
    // TODO: 是否保留编码中的补0值
  public:
    class options{
        // 0b00000001  表示 危险区  true
        // 0b00000010  表示 自然村,组 true   0b00000010
        // 0b00000100  表示 行政村 true
        // 0b00001000  表示 乡 true
        // 0b00010000  表示 县 true
        // 0b00100000  表示 市 true
        // 0b01000000  表示 省 true
      public:

        unsigned char name_level = AD_XIANG | AD_CUN | AD_ZRC; // 默认显示xx乡xx村xx组
        // 结果输出的等级
        /*
         * 返回结果的ADCD中, 如果0b00100000 那么只返回市级的信息
         * 如果 0b00110000 那么返回市级和县级的信息
         * 如果 0b00101000 那么返回市级和乡镇级的信息 以此类推
         * */
        unsigned char code_level = AD_ZRC; // 默认只显示村
        unsigned char code_type = 0; // 保留补0值


    };
  public:
    static std::map<std::string, std::string> cascade(const std::map<std::string, std::string>& adcd_adnm, const options& opt);
};

#endif  // SILLY_UTILS_SILLY_ADCD_H
