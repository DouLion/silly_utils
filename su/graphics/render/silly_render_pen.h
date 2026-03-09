/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-25
 * @file: silly_render_pen.h
 * @description: silly_render_pen 类声明
 * @version: v1.0.1 2024-10-25 dou li yang
 */
#ifndef SILLY_RENDER_PEN_H
#define SILLY_RENDER_PEN_H
#include <su_macro.h>
class suRenderPen
{
  public:
    suRenderPen();
    ~suRenderPen();

  private:
    uint8_t m_dash_type;
    std::vector<float> m_dash_arr;
};

#endif  // SILLY_RENDER_PEN_H
