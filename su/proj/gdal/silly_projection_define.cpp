/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/7 13:02
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/6/7. 实现
//

#include "silly_projection_define.h"


char* silly_projection_define::epsg3857Wkt =
    "PROJCS[\"WGS 84 / Pseudo-Mercator\","
    "GEOGCS[\"WGS 84\","
    "DATUM[\"WGS_1984\","
    "SPHEROID[\"WGS 84\",6378137,298.257223563,"
    "AUTHORITY[\"EPSG\",\"7030\"]],"
    "AUTHORITY[\"EPSG\",\"6326\"]],"
    "PRIMEM[\"Greenwich\",0,"
    "AUTHORITY[\"EPSG\",\"8901\"]],"
    "UNIT[\"degree\",0.0174532925199433,"
    "AUTHORITY[\"EPSG\",\"9122\"]],"
    "AUTHORITY[\"EPSG\",\"4326\"]],"
    "PROJECTION[\"Mercator_1SP\"],"
    "PARAMETER[\"central_meridian\",0],"
    "PARAMETER[\"scale_factor\",1],"
    "PARAMETER[\"false_easting\",0],"
    "PARAMETER[\"false_northing\",0],"
    "UNIT[\"metre\",1,"
    "AUTHORITY[\"EPSG\",\"9001\"]],"
    "AXIS[\"X\",EAST],"
    "AXIS[\"Y\",NORTH],"
    "EXTENSION[\"PROJ4\",\"+proj=merc +datum=WGS84 +ellps=WGS84 "
    "+towgs84=0,0,0 +units=m +no_defs\"],"
    "AUTHORITY[\"EPSG\",\"3857\"]]";

char *silly_projection_define::epsg4326Wkt =
    "GEOGCS[\"WGS 84\","
    "DATUM[\"WGS_1984\","
    "SPHEROID[\"WGS 84\",6378137,298.257223563,"
    "AUTHORITY[\"EPSG\",\"7030\"]],"
    "AUTHORITY[\"EPSG\",\"6326\"]],"
    "PRIMEM[\"Greenwich\",0,"
    "AUTHORITY[\"EPSG\",\"8901\"]],"
    "UNIT[\"degree\",0.0174532925199433,"
    "AUTHORITY[\"EPSG\",\"9122\"]],"
    "AUTHORITY[\"EPSG\",\"4326\"]]";


/*
 * 这段代码表示关于"Beijing 2000"地理坐标系统的参数设置。
 * 它包括了关于椭球体（Spheroid）和大地基准（Datum）的信息，
 * 以及基准经线（Prime Meridian）、单位（Unit）和
 * EPSG（European Petroleum Survey Group）的授权信息。
 * */
char *silly_projection_define::epsg4490Wkt =
    "GEOGCS[\"China Geodetic Coordinate System 2000\","
    "DATUM[\"China_2000\","
    "SPHEROID[\"CGCS2000\",6378137,298.257222101,"
    "AUTHORITY[\"EPSG\",\"1024\"]],"
    "AUTHORITY[\"EPSG\",\"1043\"]],"
    "PRIMEM[\"Greenwich\",0,"
    "AUTHORITY[\"EPSG\",\"8901\"]],"
    "UNIT[\"degree\",0.0174532925199433,"
    "AUTHORITY[\"EPSG\",\"9122\"]],"
    "AUTHORITY[\"EPSG\",\"4490\"]]";


char *silly_projection_define::epsg4610Wkt =
    "GEOGCS[\"Xian 1980\","
    "DATUM[\"Xian_1980\","
    "SPHEROID[\"IAG 1975\",6378140,298.257,"
    "AUTHORITY[\"EPSG\",\"7049\"]],"
    "AUTHORITY[\"EPSG\",\"6610\"]],"
    "PRIMEM[\"Greenwich\",0,"
    "AUTHORITY[\"EPSG\",\"8901\"]],"
    "UNIT[\"degree\",0.0174532925199433,"
    "AUTHORITY[\"EPSG\",\"9122\"]],"
    "AUTHORITY[\"EPSG\",\"4610\"]]";

char *silly_projection_define::epsg4214Wkt =
    "GEOGCS[\"Beijing 1954\","
    "DATUM[\"Beijing_1954\","
    "SPHEROID[\"Krassowsky 1940\",6378245,298.3],"
    "TOWGS84[15.8,-154.4,-82.3,0,0,0,0]],"
    "PRIMEM[\"Greenwich\",0,"
    "AUTHORITY[\"EPSG\",\"8901\"]],"
    "UNIT[\"degree\",0.0174532925199433,"
    "AUTHORITY[\"EPSG\",\"9122\"]],"
    "AUTHORITY[\"EPSG\",\"4214\"]]";


const char *silly_projection_define::get(const silly_proj_def_enum &def)
{
    switch (def)
    {
        case silly_proj_def_enum::GCS_WGS_1984:
            return epsg4326Wkt;
        case silly_proj_def_enum::PCS_WGS_1984_WEB_MERCATOR:
            return epsg3857Wkt;
        case silly_proj_def_enum::GCS_CHINA_2000:
            return epsg4490Wkt;
        case silly_proj_def_enum::GCS_XIAN_1980:
            return epsg4610Wkt;
        case silly_proj_def_enum::GCS_BEIJING_1954:
            return epsg4214Wkt;
        case silly_proj_def_enum::BaiDu09_LL:
        case silly_proj_def_enum::GCJ02_LL:
        case silly_proj_def_enum::Baidu09_MC:
        case silly_proj_def_enum::GCJ02_MC:
        default:
            return "";
    }
}
const char *silly_projection_define::get(const int &def)
{
    switch (def)
    {
        case 4326:
            return epsg4326Wkt;
        case 3857:
            return epsg3857Wkt;
        case 4490:
            return epsg4490Wkt;
        case 4610:
            return epsg4610Wkt;
        case 4214:
            return epsg4214Wkt;
        case 910102:
        case 910101:
        case 910112:
        case 910111:
        default:
            return "";
    }
}
