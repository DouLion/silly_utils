/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-11
 * @file: silly_grid_index.h
 * @description: 网格点索引统一
 * @version: v1.0.1 2024-10-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_GRID_INDEX_H
#define SILLY_UTILS_SILLY_GRID_INDEX_H
#include <su_marco.h>
class silly_grid_index
{
    /*
     * 以 北纬3度到北纬54度 东经74度到东经135度 , 先从西到东 然后从南向北构建全国的网格点索引
     * 第一个点序号为1  在此范围之外的点序号为0
     * step 必须为 1.0/pow(2, n); 1.0. 0.5
     * */
  public:
    static int64_t index(const double& lgtd, const double& lttd, const double& step);
    static std::pair<double, double> point(const int64_t& index, const double& step);

    static int64_t checked_index(const double& lgtd, const double& lttd, const double& step);
    static std::pair<double, double> checked_point(const int64_t& index, const double& step);
};

#endif  // SILLY_UTILS_SILLY_GRID_INDEX_H
