

#include "silly_geo_operation.h"
#include <math.h>
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include <ogrsf_frmts.h>
#include <polyclipping/clipper.hpp>
using namespace ClipperLib;


silly_point geo_operation::ring_to_center(silly_ring ring)
{

    double sumX = 0;
    double sumY = 0;
    double area = 0;

    // 计算面积和形心横纵坐标的分子部分
    for (size_t i = 0; i < ring.points.size(); i++)
    {
        silly_point currentPoint = ring.points[i];
        silly_point nextPoint = ring.points[(i + 1) % ring.points.size()];

        double product = (currentPoint.lgtd * nextPoint.lttd) - (nextPoint.lgtd * currentPoint.lttd);
        area += product;

        sumX += (currentPoint.lgtd + nextPoint.lgtd) * product;
        sumY += (currentPoint.lttd + nextPoint.lttd) * product;
    }

    area /= 2;
    area < 0 ? -area : area;
    // 形心坐标的分母部分
    double denominator = 1 / (6 * area);

    // 计算形心点的横纵坐标
    double center_x = sumX * denominator;
    double center_y = sumY * denominator;

    silly_point center_point;
    center_point.lgtd = center_x;
    center_point.lttd = center_y;

    return center_point;
}


double geo_operation::two_point_azimuth(silly_point p1, silly_point p2)
{

    double theta = atan2(p2.lgtd - p1.lgtd, p2.lttd - p1.lttd);
    theta = theta * 180.0 / UM_PI;
    // 将角度范围限制在 -180 到 180 度之间
    if (theta < -180.0) 
    {
        theta += 360.0;
    }
    else if (theta > 180.0) 
    {
        theta -= 360.0;
    }
    return theta;
}


bool geo_operation::shp_to_geojson(const char* shpFile, const char* geojsonFile)
{
    //GDALAllRegister();
    //// 支持中文路径
    //CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	
    //// 属性表字段支持中文
    //CPLSetConfigOption("SHAPE_ENCODING", "");
    //GDALDataset* shpDataset = (GDALDataset*)GDALOpenEx(shpFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
    //if (shpDataset == nullptr)
    //{
    //    std::cout << "Unable to open SHP file  " << std::endl;
    //    return false;
    //}

    //GDALDriver* geojsonDriver = GetGDALDriverManager()->GetDriverByName("GeoJSON");
    //if (geojsonDriver == nullptr)
    //{
    //    std::cout << "Unable to create GeoJSON driver  " << std::endl;
    //    GDALClose(shpDataset);
    //    return false;
    //}

    //GDALDataset* geojsonDataset = geojsonDriver->CreateCopy(geojsonFile, shpDataset, FALSE, NULL, NULL, NULL);
    //if (geojsonDataset == nullptr)
    //{
    //    std::cout << "Unable to create output GeoJSON file  " << std::endl;
    //    GDALClose(shpDataset);
    //    return false;
    //}

    //GDALClose(geojsonDataset);
    //GDALClose(shpDataset);

    std::cout << "SHP file successfully converted to GeoJSON file  " << std::endl;
    return true;
}

bool geo_operation::geojson_to_shp(const char* geojsonFile, const char* shpFile)
{
    //GDALAllRegister();
    //// 支持中文路径
    //CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    //// 属性表字段支持中文
    //CPLSetConfigOption("SHAPE_ENCODING", "");

    //GDALDataset* geojsonDataset = (GDALDataset*)GDALOpenEx(geojsonFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
    //if (geojsonDataset == nullptr)
    //{
    //    std::cout << "Unable to open GeoJSON file " << std::endl;
    //    return false;
    //}

    //GDALDriver* shpDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    //if (shpDriver == nullptr)
    //{
    //    std::cout << "Unable to create SHP driver " << std::endl;
    //    GDALClose(geojsonDataset);
    //    return false;
    //}

    //GDALDataset* shpDataset = shpDriver->CreateCopy(shpFile, geojsonDataset, FALSE, NULL, NULL, NULL);
    //if (shpDataset == nullptr)
    //{
    //    std::cout << "Unable to create output SHP file " << std::endl;
    //    GDALClose(geojsonDataset);
    //    return false;
    //}

    //GDALClose(shpDataset);
    //GDALClose(geojsonDataset);

    //std::cout << "GeoJSON file successfully converted to SHP file " << std::endl;
    return true;
}

std::vector<silly_ring> geo_operation::read_shp_ring(const char* shp)
{
    std::vector<silly_ring> rings;

    //// 注册 GDAL 驱动
    //GDALAllRegister();
    ////支持中文路径
    //CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    ////支持属相表中文字符
    //CPLGetConfigOption("SHAPE_ENCODING", "UTF-8");

    //// 打开 shp 文件
    //GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(shp, GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
    //if (dataset == nullptr)
    //{
    //    std::cout << "Failed to open SHP file " << std::endl;
    //    return rings;
    //}

    //// 获取图层数量
    //int layerCount = dataset->GetLayerCount();

    //// 遍历图层
    //for (int i = 0; i < layerCount; i++)
    //{
    //    OGRLayer* layer = dataset->GetLayer(i);

    //    // 遍历要素
    //    layer->ResetReading();
    //    OGRFeature* feature;
    //    while ((feature = layer->GetNextFeature()) != nullptr)
    //    {
    //        //// 获取属性值
    //        //std::string fieldValue = feature->GetFieldAsString("NAME");
    //        //// 输出属性值
    //        //std::cout << "value: " << fieldValue << std::endl;
    //        
    //        // 获取几何对象
    //        OGRGeometry* geometry = feature->GetGeometryRef();
    //        if (geometry == nullptr)
    //        {
    //            continue;
    //        }

    //        // 创建一个新的 silly_ring 对象
    //        silly_ring outer_ring;
    //        OGRPolygon* polygon = static_cast<OGRPolygon*>(geometry);
    //        // 获取外环
    //        OGRLinearRing* outerRing = polygon->getExteriorRing();
    //        if (outerRing == nullptr)
    //        {
    //            continue;
    //        }
    //        // 遍历外环的点
    //        int numPoints = outerRing->getNumPoints();
    //        if (numPoints > 3)
    //        {
    //            for (int j = 0; j < numPoints; j++)
    //            {
    //                OGRPoint point;
    //                outerRing->getPoint(j, &point);

    //                // 创建一个新的 silly_point 对象，存储点的坐标
    //                silly_point sp(point.getX(), point.getY());

    //                // 将点添加到 silly_ring 的 points 向量中
    //                outer_ring.points.push_back(sp);
    //            }
    //            rings.push_back(outer_ring);
    //        }

    //        //silly_ring inner_ring;
    //        //// 获取内环数量
    //        //int numInnerRings = polygon->getNumInteriorRings();
    //        //if (numInnerRings > 3)
    //        //{
    //        //    for (int k = 0; k < numInnerRings; k++)
    //        //    {
    //        //        OGRLinearRing* innerRing = polygon->getInteriorRing(k);
    //        //        if (innerRing == nullptr)
    //        //        {
    //        //            continue;
    //        //        }

    //        //        // 遍历内环的点
    //        //        int numInnerPoints = innerRing->getNumPoints();
    //        //        for (int l = 0; l < numInnerPoints; l++)
    //        //        {
    //        //            OGRPoint point;
    //        //            innerRing->getPoint(l, &point);

    //        //            // 创建一个新的 silly_point 对象，存储点的坐标
    //        //            silly_point sp(point.getX(), point.getY());

    //        //            // 将点添加到外环的 points 向量中
    //        //            inner_ring.points.push_back(sp);
    //        //        }
    //        //    }
    //        //    rings.push_back(inner_ring);
    //        //}
    //        // 释放要素
    //        OGRFeature::DestroyFeature(feature);
    //    }
    //}

    //GDALClose(dataset);
    return rings;
}

std::vector<silly_ring> geo_operation::read_geojson(const char* geojsonFile)
{
    std::vector<silly_ring> rings;

    //GDALAllRegister();  // 注册所有GDAL驱动
    //GDALDataset* dataset = (GDALDataset*)GDALOpenEx(geojsonFile, GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    //if (dataset == nullptr) 
    //{
    //    std::cout << "Failed to open GeoJSON file " << std::endl;
    //    return rings;
    //}
    ////获取GeoJSON文件的第一个图层
    //OGRLayer* layer = dataset->GetLayer(0);
    //if (layer == nullptr) 
    //{
    //    std::cout << "Failed to retrieve layer from GeoJSON file " << std::endl;
    //    GDALClose(dataset);
    //    return rings;
    //}

    //OGRFeature* feature;
    //layer->ResetReading();

    //while ((feature = layer->GetNextFeature()) != nullptr) 
    //{
    //    OGRGeometry* geometry = feature->GetGeometryRef();
    //    if (geometry != nullptr && geometry->getGeometryType() == wkbPolygon) 
    //    {
    //        OGRPolygon* polygon = (OGRPolygon*)geometry;
    //        int ringCount = polygon->getNumInteriorRings() + 1;  

    //        for (int i = 0; i < ringCount; i++) 
    //        {
    //            silly_ring ring;
    //            OGRLinearRing* linearRing = (i == 0) ? polygon->getExteriorRing() : polygon->getInteriorRing(i - 1);
    //            int pointCount = linearRing->getNumPoints();

    //            for (int j = 0; j < pointCount; j++) 
    //            {
    //                OGRPoint point;
    //                linearRing->getPoint(j, &point);
    //                ring.points.push_back({ point.getX(), point.getY() });
    //            }

    //            ring.is_outer = (i == 0) ? 1 : 0;
    //            rings.push_back(ring);
    //        }
    //    }

    //    OGRFeature::DestroyFeature(feature);
    //}

    //GDALClose(dataset);

    return rings;
}



bool geo_operation::points_to_shp(std::vector<silly_point>& points, const char* shpFilePath, const char* outputShpFilePath)
{
    //// 注册 GDAL 驱动
    //GDALAllRegister();

    //// 打开现有 shp 文件
    //GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(shpFilePath, GDAL_OF_VECTOR | GDAL_OF_UPDATE, nullptr, nullptr, nullptr));
    //if (dataset == nullptr)
    //{
    //    // 处理文件打开失败的情况
    //    std::cout << "Failed to open shapefile." << std::endl;
    //    return false;
    //}

    //// 获取第一个图层
    //OGRLayer* layer = dataset->GetLayer(0);
    //if (layer == nullptr)
    //{
    //    // 处理图层获取失败的情况
    //    std::cout << "Failed to get layer." << std::endl;
    //    GDALClose(dataset);
    //    return false;
    //}

    //// 创建新的输出 shp 文件
    //GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    //GDALDataset* outputDataset = outDriver->Create(outputShpFilePath, 0, 0, 0, GDT_Unknown, nullptr);
    //if (outputDataset == nullptr)
    //{
    //    // 处理输出文件创建失败的情况
    //    std::cout << "Failed to create output shapefile." << std::endl;
    //    GDALClose(dataset);
    //    return false;
    //}

    //// 创建新的图层
    //OGRLayer* outputLayer = outputDataset->CreateLayer("points", layer->GetSpatialRef(), wkbPoint, nullptr);
    //if (outputLayer == nullptr)
    //{
    //    // 处理图层创建失败的情况
    //    std::cout << "Failed to create output layer." << std::endl;
    //    GDALClose(dataset);
    //    GDALClose(outputDataset);
    //    return false;
    //}

    //// 定义并创建字段
    //OGRFieldDefn fieldSize("Size", OFTReal);
    //if (outputLayer->CreateField(&fieldSize) != OGRERR_NONE)
    //{
    //    // 处理字段创建失败的情况
    //    std::cout << "Failed to create size field." << std::endl;
    //    GDALClose(dataset);
    //    GDALClose(outputDataset);
    //    return false;
    //}

    //OGRFieldDefn fieldColor("Color", OFTString);
    //if (outputLayer->CreateField(&fieldColor) != OGRERR_NONE)
    //{
    //    // 处理字段创建失败的情况
    //    std::cout << "Failed to create color field." << std::endl;
    //    GDALClose(dataset);
    //    GDALClose(outputDataset);
    //    return false;
    //}

    //// 创建要素并进行设置
    //for (const auto& point : points)
    //{
    //    OGRFeature* feature = OGRFeature::CreateFeature(outputLayer->GetLayerDefn());

    //    // 创建点对象
    //    OGRPoint ogrPoint;
    //    ogrPoint.setX(point.lgtd);
    //    ogrPoint.setY(point.lttd);

    //    // 设置点要素的几何对象
    //    feature->SetGeometry(&ogrPoint);


    //    // 将要素添加到图层
    //    if (outputLayer->CreateFeature(feature) != OGRERR_NONE)
    //    {
    //        // 处理要素添加失败的情况
    //        std::cout << "Failed to add feature." << std::endl;
    //        OGRFeature::DestroyFeature(feature);
    //        GDALClose(dataset);
    //        GDALClose(outputDataset);
    //        return false;
    //    }

    //    // 释放要素
    //    OGRFeature::DestroyFeature(feature);
    //}

    //// 关闭数据集
    //GDALClose(dataset);
    //GDALClose(outputDataset);

    //std::cout << "Points added to shapefile and saved successfully." << std::endl;
    return true;
}



//// 将 silly_ring 转换为 OGRPolygon
//OGRPolygon* createPolygonFromSillyRing(const silly_ring& ring) 
//{
//    OGRPolygon* polygon = new OGRPolygon;
//    OGRLinearRing* ringObj = new OGRLinearRing;
//
//    for (const auto& point : ring.points) {
//        ringObj->addPoint(point.lgtd, point.lttd);
//    }
//
//    polygon->addRingDirectly(ringObj);
//
//    return polygon;
//}
//
//// 求两个面的相交的面
//OGRPolygon IntersectingPolygon(const OGRPolygon* poly1, const OGRPolygon* poly2)
//{
//    ClipperLib::Clipper c;
//    ClipperLib::Paths sub, clp, sol;
//
//    ClipperLib::Paths temPaths;  // 多个面，内外环形
//    ClipperLib::Path tmpPath1;
//    ClipperLib::Path tmpPath2;
//    bool hasRoot = false;
//
//    OGRPolygon Polygon_reuslt;
//
//    // 获取两个面的点
//    const OGRLinearRing* boundary1 = poly1->getExteriorRing();
//    if (!boundary1->IsEmpty())
//    {
//        for (int i = 0; i < boundary1->getNumPoints(); i++)
//        {
//            ClipperLib::IntPoint temp;
//            temp.X = boundary1->getX(i) * 10e6;
//            temp.Y = boundary1->getY(i) * 10e6;
//            tmpPath1.push_back(temp);
//        }
//        temPaths.push_back(tmpPath1);
//    }
//
//    const OGRLinearRing* boundary2 = poly2->getExteriorRing();
//    if (!boundary2->IsEmpty())
//    {
//        for (int i = 0; i < boundary2->getNumPoints(); i++)
//        {
//            ClipperLib::IntPoint temp;
//            temp.X = boundary2->getX(i) * 10e6;
//            temp.Y = boundary2->getY(i) * 10e6;
//            tmpPath2.push_back(temp);
//        }
//        temPaths.push_back(tmpPath2);
//    }
//    if (!hasRoot)
//    {
//        sub = temPaths;
//        hasRoot = !hasRoot;
//    }
//    else if (temPaths.size() == 2)
//    {
//        c.Clear();
//        clp = temPaths;
//        c.AddPaths(sub, ptSubject, true);
//        c.AddPaths(clp, ptClip, true);
//        c.Execute(ctIntersection, sol, pftEvenOdd, pftEvenOdd);
//        sub = sol;
//    }
//
//    if (!sub.empty())
//    {
//
//        for (int i = 0; i < sub.size(); i++) // 从下标0开始遍历所有的环
//        {
//            OGRLinearRing boundary_result;
//            for (int j = 0; j < sub[i].size(); j++) // 遍历每个环的所有点
//            {
//                boundary_result.addPoint(sub[i][j].X / 10.0e6, sub[i][j].Y / 10.0e6);
//            }
//            boundary_result.closeRings();
//            Polygon_reuslt.addRing(&boundary_result);
//        }
//        Polygon_reuslt.closeRings();
//    }
//
//
//    return Polygon_reuslt;
//}


/// polygon类型转silly_ring
silly_ring convertPolygonToSillyRing(OGRPolygon* polygon) 
{
    silly_ring ring;

    //// 获取多边形的外环
    //OGRLinearRing* exteriorRing = polygon->getExteriorRing();

    //// 获取外环的点数
    //int numPoints = exteriorRing->getNumPoints();

    //// 将外环的点转换为 silly_point 类型，并添加到 silly_ring 中
    //for (int i = 0; i < numPoints; i++) {
    //    OGRPoint point;
    //    exteriorRing->getPoint(i, &point);
    //    ring.points.push_back(silly_point(point.getX(), point.getY()));
    //}

    //////// 获取多边形的内环（如果存在），并将内环的点转换为 silly_point 类型，并添加到 silly_ring 中
    ////int numInteriorRings = polygon->getNumInteriorRings();
    ////for (int j = 0; j < numInteriorRings; j++) 
    //// {
    ////    OGRLinearRing* interiorRing = polygon->getInteriorRing(j);
    ////    int numInteriorPoints = interiorRing->getNumPoints();
    ////    for (int k = 0; k < numInteriorPoints; k++) 
    //// {
    ////        OGRPoint interiorPoint;
    ////        interiorRing->getPoint(k, &interiorPoint);
    ////        ring.points.push_back(silly_point(interiorPoint.getX(), interiorPoint.getY()));
    ////    }
    ////}

    return ring;
}

silly_ring geo_operation::intersect_area(silly_ring ring_1, silly_ring ring_2)
{
    silly_ring ring;
    //// 创建 OGRPolygon 对象
    //OGRPolygon* poly1 = createPolygonFromSillyRing(ring_1);
    //OGRPolygon* poly2 = createPolygonFromSillyRing(ring_2);
    //// 判断两个 OGRPolygon 是否相交
    //if (!poly1->Intersects(poly2))
    //{
    //    // 不相交，返回空面
    //    delete poly1;
    //    delete poly2;
    //    return silly_ring();
    //}
    //// 计算相交区域
    //OGRPolygon IntersectPolygon = IntersectingPolygon(poly1, poly2);
    //// 计算出相加区域转为silly_ring
    //ring = convertPolygonToSillyRing(&IntersectPolygon);
    //delete poly1;
    //delete poly2;

    return ring;
}


silly_ring geo_operation::intersect_area2(silly_ring ring_1, silly_ring ring_2)
{
    silly_ring intersecting_ring;

    //// 创建 OGRPolygon 对象
    //OGRPolygon* poly1 = createPolygonFromSillyRing(ring_1);
    //OGRPolygon* poly2 = createPolygonFromSillyRing(ring_2);

    //// 判断两个 OGRPolygon 是否相交
    //if (!poly1->Intersects(poly2)) 
    //{
    //    // 不相交，返回空面
    //    delete poly1;
    //    delete poly2;
    //    return silly_ring();
    //}
    //// 计算相交区域
    //OGRGeometry* intersection = poly1->Intersection(poly2);

    //// 将 OGRGeometry* 转换为 silly_ring
    //if (intersection->getGeometryType() == wkbPolygon) 
    //{
    //    OGRPolygon* intersectingPolygon = dynamic_cast<OGRPolygon*>(intersection);
    //    OGRLinearRing* ring = intersectingPolygon->getExteriorRing();

    //    int numPoints = ring->getNumPoints();
    //    for (int i = 0; i < numPoints; i++) 
    //    {
    //        OGRPoint point;
    //        ring->getPoint(i, &point);
    //        intersecting_ring.points.push_back(silly_point(point.getX(), point.getY()));
    //    }
    //}
    //delete poly1;
    //delete poly2;
    //delete intersection;

    return intersecting_ring;
}