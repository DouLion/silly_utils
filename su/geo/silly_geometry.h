//
// Created by dell on 2023/11/1.
//

#ifndef SILLY_UTILS_SILLY_GEOMETRY_H
#define SILLY_UTILS_SILLY_GEOMETRY_H

#include <geo/silly_geo.h>

namespace silly_geometry {

    /// <summary>
    /// 属性段
    /// </summary>
    struct property
    {
        // 树型类型
        enum_geoprop_types type{ enum_geoprop_types::eNone };
        // 属性长度
        size_t	len{ 0 };
        // 属性内容
        unsigned char* data{ nullptr };
    };

    /// <summary>
    /// 矢量基类
    /// </summary>
    class geometry_collection
    {
    public:
        std::map<std::string, property> properties{};
    };

    
    // 各种常见类型的矢量

    class point : public geometry_collection
    {
        silly_point m_point;
    };


    class multi_point : public geometry_collection
    {
        std::vector<silly_point> m_points;
    };


    class line : public geometry_collection
    {
        silly_line m_line;
    };


    class multi_line : public geometry_collection
    {
        std::vector<silly_line> m_lines;
    };


    class polygon : public geometry_collection
    {
        silly_poly m_polygon;
    };


    class multi_polygon : public geometry_collection
    {
        std::vector<silly_poly> m_polygons;
    };

};


#endif //SILLY_UTILS_SILLY_GEOMETRY_H
