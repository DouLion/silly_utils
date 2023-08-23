//
// Created by dly on 2023/7/10.
//

#include "geotiff_utils.h"


void geotiff_utils::readGeoTiff(std::string filePath, matrix_2d<unsigned char>& tif_matrix)
{
    // 打开影像数据
    TIFF* tiff = XTIFFOpen(filePath.c_str(), "r");
    if (tiff == nullptr) {
        std::cout << "打开影像数据失败" << std::endl;
        return;
    }


    // 读取图像基本信息
    uint32_t width, height;
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
    std::cout << "宽:" << width << std::endl;
    std::cout << "高:" << height << std::endl;

    //--------------------------------

    // 每个点占的位数
    uint16_t bitsPerSample;
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    std::cout << "每个点占的位数: " << bitsPerSample << std::endl;

    // 获取分辨率单位
    uint16_t resolutionUnit;
    TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &resolutionUnit);
    std::cout << "分辨率单位: " << resolutionUnit << std::endl;

    // 获取每个像素的样本数
    uint16_t samplesPerPixel;
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    std::cout << "每个像素的样本数: " << samplesPerPixel << std::endl;

    // 获取水平分辨率
    float xResolution;
    TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &xResolution);
    std::cout << "水平分辨率: " << xResolution << std::endl;

    // 获取垂直分辨率
    float yResolution;
    TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &yResolution);
    std::cout << "垂直分辨率: " << yResolution << std::endl;

    //-------------------------------------------------

    // 获取影像属性信息
    GTIF* gtif = GTIFNew(tiff);
    if (gtif == nullptr) {
        std::cout << "获取影像属性信息失败 " << std::endl;
        XTIFFClose(tiff);
        return;
    }
    GTIFPrint(gtif, 0, 0); // 打印属性信息
     // 获取图像起始点的投影坐标
    double tif_letf = 0.0, tif_top = 0.0;
    GTIFImageToPCS(gtif, &tif_letf, &tif_top);

    // 每通道位数（样本）
    uint16_t bitsPerPixel;
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerPixel);

    // 数据类型 :TIFFTAG_SAMPLEFORMAT
    uint16_t sampleFormat;
    if (TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &sampleFormat)) {
        if (sampleFormat == SAMPLEFORMAT_UINT) {
            std::cout << "数据类型：无符号整数 " << std::endl;
        }
        else if (sampleFormat == SAMPLEFORMAT_INT) {
            std::cout << "数据类型：有符号整数 " << std::endl;
        }
        else if (sampleFormat == SAMPLEFORMAT_IEEEFP) {
            std::cout << "数据类型：浮点数 " << std::endl;
        }
        else if (sampleFormat == SAMPLEFORMAT_VOID) {
            std::cout << "数据类型：未知数据类型 " << std::endl;
        }
        else {
            // 其他未知数据类型
            std::cout << "未知的数据类型 " << std::endl;
        }
    }
    else {
        std::cout << "无法获取数据类型 " << std::endl;
    }

    // 图像的方向标签
    uint16_t orientation;
    if (TIFFGetField(tiff, TIFFTAG_ORIENTATION, &orientation)) {
        std::cout << "图像的方向标签: " << orientation << std::endl;
    }
    else {
        std::cout << "无法读取 图像的方向标签 标签的值 " << std::endl;
    }

    // 图像的平面配置标签
    uint16_t planarConfig;
    if (TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &planarConfig)) {
        std::cout << "图像的平面配置标签: " << planarConfig << std::endl;
    }
    else {
        std::cout << "无法读取 图像的平面配置标签 标签的值 " << std::endl;
    }

    // 图像的光度标签
    uint16_t photometric;
    if (TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &photometric)) {
        std::cout << "图像的光度标签: " << photometric << std::endl;
    }
    else {
        std::cout << "无法读取 TIFFTAG_PHOTOMETRIC 标签的值 " << std::endl;
    }


    tif_matrix.create(height, width);
    int a = sizeof(unsigned char);
    // 逐行读取像素数据
    for (uint32_t row = 0; row < height; ++row) 
    {
        TIFFReadScanline(tiff, &(tif_matrix.at(row, 0)), row * sizeof(unsigned char) * (bitsPerSample / 8));
    }

    // 关闭影像数据和GTIF对象
    XTIFFClose(tiff);
    GTIFFree(gtif);
}



void geotiff_utils::writeGeoTiff(std::string filePath, matrix_2d<unsigned char>& tif_matrix)
{
    // 获取矩阵的行数和列数
    size_t rows = tif_matrix.row();
    size_t cols = tif_matrix.col();

    // 打开 TIFF 文件进行写入
    TIFF* tiff = TIFFOpen(filePath.c_str(), "w");
    if (!tiff) {
        std::cerr << "无法打开 TIFF 文件进行写入 " << std::endl;
        return;
    }

    // 设置 TIFF 图像的基本信息
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, cols);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, rows);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    // 写入像素数据
    for (size_t row = 0; row < rows; ++row) {
        TIFFWriteScanline(tiff, tif_matrix.get_data()[row], row);
    }

    // 关闭 TIFF 文件
    TIFFClose(tiff);

    std::cout << "TIFF 文件写入成功 " << std::endl;
}






//void geotiff_utils::readGeoTiff(std::string filePath, std::vector<std::vector<int>>& imageMatrix)
//{
//    // 打开TIFF文件
//    TIFF* tiff = TIFFOpen(filePath.c_str(), "r");
//    if (!tiff) {
//        std::cerr << "无法打开TIFF文件" << std::endl;
//        return;
//    }
//
//    // 读取图像基本信息
//    uint32_t width, height;
//    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
//    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
//
//    uint16_t bitsPerPixel;
//    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerPixel);
//
//    // 确定存储矩阵的数据类型（int、float等）和维度
//    // 这里以int类型的二维矩阵为例，可以根据实际需要进行修改
//    imageMatrix.resize(height, std::vector<int>(width));
//
//    // 逐行读取像素数据
//    for (uint32_t row = 0; row < height; ++row) {
//        TIFFReadScanline(tiff, &imageMatrix[row][0], row);
//    }
//
//    // 关闭TIFF文件
//    TIFFClose(tiff);
//}




//void geotiff_utils::readGeoTiff(std::string filePath, std::vector<std::vector<uint32_t>>& imageMatrix) 
//{
//    // 打开影像数据
//    TIFF* tiff = XTIFFOpen(filePath.c_str(), "r");
//    if (tiff == nullptr) {
//        std::cout << "打开影像数据失败" << std::endl;
//        return;
//    }
//    // 获取影像属性信息
//    GTIF* gtif = GTIFNew(tiff);
//    if (gtif == nullptr) {
//        std::cout << "获取影像属性信息失败" << std::endl;
//        XTIFFClose(tiff);
//        return;
//    }
//    GTIFPrint(gtif, 0, 0); // 打印属性信息
//
//    // 获取影像宽度和高度
//    int  width, height;
//    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
//    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
//
//    //// 分配二维矩阵内存
//    //imageMatrix.resize(height);
//    //for (uint32_t i = 0; i < height; ++i) {
//    //    imageMatrix[i].resize(width);
//    //}
//
//
//
//
//    // 关闭影像数据和GTIF对象
//    XTIFFClose(tiff);
//    GTIFFree(gtif);
//}
