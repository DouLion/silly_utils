/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/7 13:02
 * @version: 1.0.1
 * @description: https://epsg.io/4534
 */
//
// Created by dell on 2024/6/7. 实现
//

#include "silly_projection_define.h"
using namespace silly::geo::proj;

const std::string CRS::wkt(const type &def)
{
    std::string ret;

    return ret;
}
const std::string CRS::proj4(const type &def)
{
    std::string ret;

    return ret;
}
#if ENABLE_GDAL
const OGRSpatialReference CRS::reference(const type &def)
{
    OGRSpatialReference ret;

    return ret;
}
#endif

std::string CRS::EPSG3857WKT = R"()";
std::string CRS::EPSG3857PROJ4 = R"(+proj=merc +a=6378137 +b=6378137 +lat_ts=0 +lon_0=0 +x_0=0 +y_0=0 +k=1 +units=m +nadgrids=@null +wktext +no_defs)";

std::string CRS::EPSG4326WKT = R"()";
std::string CRS::EPSG4326PROJ4 = R"(+proj=longlat +datum=WGS84 +no_defs)";

std::string CRS::EPSG4490WKT = R"()";
std::string CRS::EPSG4490PROJ4 = R"(+proj=longlat +ellps=GRS80 +no_defs)";

std::string CRS::EPSG4534WKT = R"()";
std::string CRS::EPSG4534PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=75 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4535WKT = R"()";
std::string CRS::EPSG4535PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=78 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4536WKT = R"()";
std::string CRS::EPSG4536PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=81 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4537WKT = R"()";
std::string CRS::EPSG4537PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=84 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4538WKT = R"()";
std::string CRS::EPSG4538PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=87 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4539WKT = R"()";
std::string CRS::EPSG4539PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=90 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4540WKT = R"()";
std::string CRS::EPSG4540PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=93 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4541WKT = R"()";
std::string CRS::EPSG4541PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=96 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4542WKT = R"()";
std::string CRS::EPSG4542PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=99 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4543WKT = R"()";
std::string CRS::EPSG4543PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=102 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4544WKT = R"()";
std::string CRS::EPSG4544PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=105 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4545WKT = R"()";
std::string CRS::EPSG4545PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=108 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4546WKT = R"()";
std::string CRS::EPSG4546PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=111 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4547WKT = R"()";
std::string CRS::EPSG4547PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4548WKT = R"()";
std::string CRS::EPSG4548PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=117 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4549WKT = R"()";
std::string CRS::EPSG4549PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=120 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4550WKT = R"()";
std::string CRS::EPSG4550PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=123 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4551WKT = R"()";
std::string CRS::EPSG4551PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=126 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4552WKT = R"()";
std::string CRS::EPSG4552PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=129 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4553WKT = R"()";
std::string CRS::EPSG4553PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=132 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";

std::string CRS::EPSG4554WKT = R"()";
std::string CRS::EPSG4554PROJ4 = R"(+proj=tmerc +lat_0=0 +lon_0=135 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs)";
