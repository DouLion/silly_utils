/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 24-8-9 下午6:58
 * @version: 1.0.1
 * @description: silly_netcdf 类实现
 */
#include "silly_netcdf.h"
bool silly_netcdf::open(const std::string& path)
{
    bool status = false;
    try{
        nc_file.open(path, NcFile::read);
        status = true;
    }
    catch(NcException& e){
        std::cout << e.what() << std::endl;
        return status;
    }

    return status;
}
std::vector<std::string> silly_netcdf::members()
{
    std::vector<std::string> result;
    NcGroup nc_all_grps = nc_file.getGroup("/", NcGroup::GroupLocation::AllGrps);
    auto nc_all_vars = nc_all_grps.getVars(netCDF::NcGroup::Current);
    for(auto [k, var] : nc_all_vars)
    {
        result.push_back(k);
    }
    for(auto nm: result)
    {
        //nc_all_grps
    }
    return result;
}
bool silly_netcdf::geometry(const std::string& lon, const std::string& lat)
{

    NcVar lon_var = nc_all_grps.getVar(lon);
    NcVar lat_var = nc_all_grps.getVar(lat);
    lon_var.getDimCount();
    return false;
}
void silly_netcdf::close()
{
    nc_file.close();
}
