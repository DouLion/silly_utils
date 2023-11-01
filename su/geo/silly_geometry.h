//
// Created by dell on 2023/11/1.
//

#ifndef SILLY_UTILS_SILLY_GEOMETRY_H
#define SILLY_UTILS_SILLY_GEOMETRY_H

#include <geo/silly_geo.h>

namespace silly_geometry {

    /// <summary>
    /// ���Զ�
    /// </summary>
    struct property
    {
        // ��������
        enum_geoprop_types type{ enum_geoprop_types::eNone };
        // ���Գ���
        size_t	len{ 0 };
        // ��������
        unsigned char* data{ nullptr };
    };

    /// <summary>
    /// ʸ������
    /// </summary>
    class geometry_collection
    {
    public:
        std::map<std::string, property> properties{};
    };

    
    // ���ֳ������͵�ʸ��

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
