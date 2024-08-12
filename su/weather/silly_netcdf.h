/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 24-8-9 下午6:58
 * @version: 1.0.1
 * @description: silly_netcdf 类声明
 */
#ifndef SILLY_UTILS_SILLY_NETCDF_H
#define SILLY_UTILS_SILLY_NETCDF_H
#include <netcdf>
#include <log/silly_log.h>
#include <math/silly_matrix.h>
using namespace netCDF;
using namespace netCDF::exceptions;


class silly_netcdf
{
  public:
        bool open(const std::string& path);
        std::vector<std::string> members();

        bool geometry(const std::string& lon, const std::string& lat);


        void close();

  private:
    /* 网格点顺序
     * 0 自西向东, 自南向北
     * 1 自东向西, 自北向南
     * 2 自南向北, 自西向东
     * */
    int m_sort{0};
    NcFile nc_file;
    NcGroup nc_all_grps;
    silly_math::DMatrix data;
};

#endif  // SILLY_UTILS_SILLY_NETCDF_H
