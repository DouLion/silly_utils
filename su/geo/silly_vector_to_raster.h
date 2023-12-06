/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/6 16:15
 * @version: 1.0.1
 * @description: 矢量光栅化
 */
#ifndef SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H
#define SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H

namespace  silly_vector_to_raster
{
    /// 扫描线算法
    class scan_line
    {
        bool init();

        // 光栅化  输入 一个矢量面(线? 点?) 输出一组 坐标
        bool rasterization();
    };
}



#endif //SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H
