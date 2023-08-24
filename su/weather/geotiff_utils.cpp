//
// Created by dly on 2023/7/10.
//

#include  "geotiff_utils.h "


tif_data geotiff_utils::readGeoTiff(std::string filePath)
{
    tif_data res_tif;
    bool status = true;
    // 打开影像数据
    TIFF* tiff = XTIFFOpen(filePath.c_str(),  "r ");
    if (tiff == nullptr) 
    {
        std::cout <<  "打开影像数据失败 " << std::endl;
        status = false;
    }

    // 获取影像属性信息
    GTIF* gtif = GTIFNew(tiff);
    if (gtif == nullptr) {
        std::cout << "获取影像属性信息失败 " << std::endl;
        XTIFFClose(tiff);
        status = false;
    }
    GTIFPrint(gtif, 0, 0); // 打印属性信息


    // 读取图像基本信息
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &res_tif.tif_width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &res_tif.tif_height);
    std::cout <<  "宽: " << res_tif.tif_width << std::endl;
    std::cout <<  "高: " << res_tif.tif_height << std::endl;

    // 每个点占的位数
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &res_tif.tif_bitsPerSample);
    std::cout <<  "每个点占的位数:  " << res_tif.tif_bitsPerSample << std::endl;

    //// 获取分辨率单位
    //uint16_t resolutionUnit;
    //TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &resolutionUnit);
    //std::cout <<  "分辨率单位:  " << resolutionUnit << std::endl;

    // 获取每个像素的样本数
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &res_tif.tif_samplesPerPixel);
    std::cout <<  "每个像素的样本数:  " << res_tif.tif_samplesPerPixel << std::endl;


    // 获取图像起始点的投影坐标
    
    GTIFImageToPCS(gtif, &res_tif.tif_letf, &res_tif.tif_top);
    std::cout << "tif_letf: " << res_tif.tif_letf << std::endl;
    std::cout << "tif_top: " << res_tif.tif_top << std::endl;



    double pixelScale[3];
    double modelTransform[6];
    TIFFGetField(tiff, TIFFTAG_GEOPIXELSCALE,&pixelScale[0], &pixelScale[1], &pixelScale[2]);
    TIFFGetField(tiff, TIFFTAG_GEOTRANSMATRIX , &modelTransform[0], &modelTransform[1], &modelTransform[2], &modelTransform[3], &modelTransform[4], &modelTransform[5]);
    std::cout << "pixelScale数组的值：" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "pixelScale[" << i << "] = " << pixelScale[i] << std::endl;
    }

    std::cout << "modelTransform数组的值：" << std::endl;
    for (int i = 0; i < 6; i++) {
        std::cout << "modelTransform[" << i << "] = " << modelTransform[i] << std::endl;
    }
    
    //// 计算像素大小
    //double pixelSizeX = pixelScale[0];
    //double pixelSizeY = pixelScale[1];
    //// 计算原点位置
    //double originX = modelTransform[0];
    //double originY = modelTransform[3];
    //// 打印获取到的值
    //std::cout << "pixelSizeX: " << pixelSizeX << std::endl;
    //std::cout << "pixelSizeY: " << pixelSizeY << std::endl;
    //std::cout << "originX: " << originX << std::endl;
    //std::cout << "originY: " << originY << std::endl;


    //----------------------------------------------------
    // 
    // 数据类型 :TIFFTAG_SAMPLEFORMAT
    uint16_t sampleFormat;
    if (TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &res_tif.tif_sampleFormat))
    {
        if (res_tif.tif_sampleFormat == SAMPLEFORMAT_UINT)
        {
            std::cout <<  "数据类型：无符号整数 " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_INT)
        {
            std::cout <<  "数据类型：有符号整数 " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_IEEEFP)
        {
            std::cout <<  "数据类型：浮点数 " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_VOID)
        {
            std::cout <<  "数据类型：未知数据类型 " << std::endl;
        }
        else 
        {
            // 其他未知数据类型
            std::cout <<  "未知的数据类型 " << std::endl;
        }
    }
    else 
    {
        std::cout <<  "无法获取数据类型 " << std::endl;
        status = false;
    }

    // 图像的方向标签
    uint16_t orientation;
    TIFFGetField(tiff, TIFFTAG_ORIENTATION, &orientation);
    std::cout <<  "图像的方向标签:  " << orientation << std::endl;

    // 图像的平面配置标签
    uint16_t planarConfig;
    TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &planarConfig);
    std::cout <<  "图像的平面配置标签:  " << planarConfig << std::endl;


    // 图像的光度标签
    uint16_t photometric;
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &photometric);
    std::cout <<  "图像的光度标签:  " << photometric << std::endl;

    res_tif.tif_matrix2.create(res_tif.tif_height , res_tif.tif_width);
    // 逐行读取像素数据
    for (uint32_t row = 0; row < res_tif.tif_height; ++row)
    {
        TIFFReadScanline(tiff, &(res_tif.tif_matrix2.at(row, 0)), row);
    }
    std::cout << "******** " << static_cast<float>(res_tif.tif_matrix2.at(1, 1)) << std::endl;
    // 关闭影像数据和GTIF对象
    XTIFFClose(tiff);
    GTIFFree(gtif);
    return res_tif;
}


bool geotiff_utils::writeGeoTiff(std::string filePath, tif_data tif_matrix2)
{
    bool status = true;
    // 获取矩阵的行数和列数
    size_t cols = tif_matrix2.tif_width;
    size_t rows = tif_matrix2.tif_height;

    // 打开 TIFF 文件进行写入
    TIFF* tiff = TIFFOpen(filePath.c_str(),  "w ");
    if (!tiff) {
        std::cerr <<  "无法打开 TIFF 文件进行写入 " << std::endl;
        status = false;
    }

    // 设置 TIFF 图像的基本信息
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, cols);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, rows);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, tif_matrix2.tif_samplesPerPixel);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, tif_matrix2.tif_bitsPerSample);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, tif_matrix2.tif_sampleFormat);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);       //图像的方向标签
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);      // 图像的平面配置标签
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);    // 图像的光度标签


    // 创建地理信息数组（GeoTIFF 格式）
    double pixelSizeX = 0.1;
    double pixelSizeY = -0.1;
    double originX = 0.0;
    double originY = 0.0;

    double pixelScale[3] = { pixelSizeX, pixelSizeY, 0.0 };
    double modelTransform[6] = { originX, pixelSizeX, 0.0,originY, 0.0, pixelSizeY };

    TIFFSetField(tiff, TIFFTAG_GEOPIXELSCALE, 3 ,pixelScale);
    TIFFSetField(tiff, TIFFTAG_GEOTRANSMATRIX, 6 ,modelTransform);

    // 写入像素数据

    // 写入像素数据
    for (size_t row = 0; row < rows; ++row) {
        TIFFWriteScanline(tiff, tif_matrix2.tif_matrix2.get_data()[row], row);
    }

    // 关闭 TIFF 文件
    TIFFClose(tiff);

    std::cout <<  "TIFF 文件写入成功 " << std::endl;
    return status;

}




