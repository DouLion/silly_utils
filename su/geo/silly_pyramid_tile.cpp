/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-31
 * @file: silly_pyramid_tile
 * @description: silly_pyramid_tile实现
 * @version: v1.0.1 2025-12-31 dou li yang
 */
#include "silly_pyramid_tile.h"

void suPyramidTile::Verify(const suPoint& llp, const int& zoom, const int& vw, const int& vh, const int& tile_size)
{
    {

        printf("\n验证经纬度 xyz互转: (%.12f, %.12f)", llp.x, llp.y);
        suPoint xy  = suPyramidTile::LonLatToFXYZ(llp, zoom);
        printf("\n  xyz: (%.12f, %.12f, %d)", xy.x, xy.y,  zoom);
        suPoint nll = suPyramidTile::FXYZToLonLat(xy, zoom);
        printf("\n  误差 经度 : %f 纬度 : %f", nll.x - llp.x, nll.y - llp.y);


    }

    {
        printf("\n验证经纬度 场景坐标互转: (%.12f, %.12f)", llp.x, llp.y);
        suPoint xy  =suPyramidTile::LonLatToScenePos(llp, zoom);
        printf("\n  场景坐标: (%.12f, %.12f)", xy.x, xy.y);
        suPoint nll = suPyramidTile::ScenePosToLonLat(xy, zoom);
        printf("\n  误差 经度 : %f 纬度 : %f", nll.x - llp.x, nll.y - llp.y);
    }

    {
        printf("\n验证经纬度 视口范围互转: (%.12f, %.12f)", llp.x, llp.y);
        suRect svp = suPyramidTile::CenterLL2ViewPort(llp, zoom, vw, vh);
        printf("\n  场景边界: (%.12f, %.12f, %.12f, %.12f)", svp.min.x, svp.min.y, svp.max.x, svp.max.y);
        suPoint nll = suPyramidTile::ViewPort2CenterLL(svp, zoom);
        /*std::string fmt = R"(误差
左边界 : %f
上边界 : %f
右边界 : %f
下边界 : %f
)";*/
        printf("\n  误差 经度 : %f 纬度 : %f", nll.x - llp.x, nll.y - llp.y);

    }
}