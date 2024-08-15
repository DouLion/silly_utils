/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 24-8-9 下午4:10
 * @version: 1.0.1
 * @description: silly_font 类声明
 */
#ifndef SILLY_UTILS_SILLY_FONT_H
#define SILLY_UTILS_SILLY_FONT_H
#include <su_marco.h>
#if IS_WIN32
// 宋体
#define SimSun "SimSun"
// 微软雅黑常规
#define MicroSoftYaHei "Microsoft YaHei"
// 楷体
#define KaiTi "KaiTi"
// 隶书
#define LiSu "LiSu"
#elif IS_MACOS

#define SimSun "SimSun"
#define MicroSoftYaHei "Microsoft YaHei"
#define KaiTi "KaiTi"
#define LiSu "LiSu"

#else

#define SimSun "SimSun"
#define MicroSoftYaHei "Microsoft YaHei"
#define KaiTi "KaiTi"
#define LiSu "LiSu"

#endif

class silly_font
{
  public:
    silly_font();
    ~silly_font();

  private:
};

#endif  // SILLY_UTILS_SILLY_FONT_H
