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

char *silly_projection_define::espg4326Wkt =
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