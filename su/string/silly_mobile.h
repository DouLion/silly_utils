/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-04-18
 * @file: silly_mobile.h
 * @description: silly_mobile 类声明
 * @version: v1.0.1 2025-04-18 dou li yang
 */
#ifndef XINJIANGMESSAGESERVER_SILLY_MOBILE_H
#define XINJIANGMESSAGESERVER_SILLY_MOBILE_H
#include <su_marco.h>
namespace silly
{
class mobile
{
  public:
    enum eop : int
    {
        ChinaMobile,   // 中国移动
        ChinaUnicom,   // 中国联通
        ChinaTelecom,  // 中国电信
        Unknown        // 未知
    };
    static std::string op2str(const eop& op);
    static eop opcode(const std::string& number);
    static std::map<std::string, eop> opcode(const std::vector<std::string>& numbers);
    static std::string opstr(const std::string& number);
    static std::map<std::string, std::string> opstr(const std::vector<std::string>& numbers);

};
}  // namespace silly

#endif  // XINJIANGMESSAGESERVER_SILLY_MOBILE_H
