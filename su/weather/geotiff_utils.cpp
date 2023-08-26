//
// Created by dly on 2023/7/10.
//

#include  "geotiff_utils.h"


tif_data geotiff_utils::readGeoTiff(std::string filePath)
{
    tif_data res_tif;
    bool status = true;
    // ��Ӱ������
    TIFF* tiff = XTIFFOpen(filePath.c_str(),  "r ");
    if (tiff == nullptr) 
    {
        std::cout <<  "��Ӱ������ʧ�� " << std::endl;
        return res_tif;
    }

    // ��ȡӰ��������Ϣ
    GTIF* gtif = GTIFNew(tiff);
    if (gtif == nullptr) {
        std::cout << "��ȡӰ��������Ϣʧ�� " << std::endl;
        XTIFFClose(tiff);
        status = false;
    }
    GTIFPrint(gtif, 0, 0); // ��ӡ������Ϣ

    //// ��ȡͼ����ʼ���ͶӰ����
    //GTIFImageToPCS(gtif, &res_tif.tif_letf, &res_tif.tif_top);
    //std::cout << "tif_letf: " << res_tif.tif_letf << std::endl;
    //std::cout << "tif_top: " << res_tif.tif_top << std::endl;

    // ��ȡͼ�������Ϣ
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &res_tif.tif_width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &res_tif.tif_height);
    std::cout <<  "��: " << res_tif.tif_width << std::endl;
    std::cout <<  "��: " << res_tif.tif_height << std::endl;

    std::cout << "GTIFF_TIEPOINTS �������� " << std::endl;
    double* tif_coordinate = NULL;
    uint32_t tif_coordinate_count;
    TIFFGetField(tiff, GTIFF_TIEPOINTS, &tif_coordinate_count, &tif_coordinate);
    if (tif_coordinate_count > 4)
    {
        res_tif.tif_letf = tif_coordinate[3];
        res_tif.tif_top = tif_coordinate[4];
        std::cout << "tif_letf: " << res_tif.tif_letf << std::endl;
        std::cout << "tif_top: " << res_tif.tif_top << std::endl;
    }
    else
    {
        std::cout << "��ȡ��������ʧ�� " << std::endl;
        return res_tif;
    }


    std::cout << "GTIFF_PIXELSCALE ���طֱ��� " << std::endl;
    double* pixel_scale = NULL;
    int pixel_scale_count;
    TIFFGetField(tiff, GTIFF_PIXELSCALE, &pixel_scale_count, &pixel_scale);
    if (pixel_scale_count > 2)
    {
        res_tif.pixelSizeX = pixel_scale[0];
        res_tif.pixelSizeY = pixel_scale[1];
        std::cout << "pixelSizeX: " << res_tif.pixelSizeX << std::endl;
        std::cout << "pixelSizeY: " << res_tif.pixelSizeY << std::endl;
    }
    else
    {
        std::cout << "���طֱ���ʧ�� " << std::endl;
        return res_tif;
    }

    // ÿ����ռ��λ��
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &res_tif.tif_bitsPerSample);
    std::cout <<  "ÿ����ռ��λ��:  " << res_tif.tif_bitsPerSample << std::endl;

    //// ��ȡ�ֱ��ʵ�λ
    //uint16_t resolutionUnit;
    //TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &resolutionUnit);
    //std::cout <<  "�ֱ��ʵ�λ:  " << resolutionUnit << std::endl;

    // ��ȡÿ�����ص�������
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &res_tif.tif_samplesPerPixel);
    std::cout <<  "ÿ�����ص�������:  " << res_tif.tif_samplesPerPixel << std::endl;


    //----------------------------------------------------
    // 
    // �������� :TIFFTAG_SAMPLEFORMAT
    if (TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &res_tif.tif_sampleFormat))
    {
        if (res_tif.tif_sampleFormat == SAMPLEFORMAT_UINT)
        {
            std::cout <<  "�������ͣ�uint " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_INT)
        {
            std::cout <<  "�������ͣ�int " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_IEEEFP)
        {
            std::cout <<  "�������ͣ�float " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_VOID)
        {
            std::cout <<  "�������ͣ�δ֪�������� " << std::endl;
        }
        else 
        {
            // ����δ֪��������
            std::cout <<  "δ֪���������� " << std::endl;
        }
    }
    else 
    {
        std::cout <<  "�޷���ȡ�������� " << std::endl;
        return res_tif;
    }

    // ͼ��ķ����ǩ
    TIFFGetField(tiff, TIFFTAG_ORIENTATION, &res_tif.tif_orientation);
    std::cout <<  "ͼ��ķ����ǩ:  " << res_tif.tif_orientation << std::endl;

    // ͼ���ƽ�����ñ�ǩ
    TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &res_tif.tif_planarConfig);
    std::cout <<  "ͼ���ƽ�����ñ�ǩ:  " << res_tif.tif_planarConfig << std::endl;

    // ͼ��Ĺ�ȱ�ǩ
    uint16_t photometric;
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &res_tif.tif_photometric);
    std::cout <<  "ͼ��Ĺ�ȱ�ǩ:  " << res_tif.tif_photometric << std::endl;
    //----------------------------------

    // ���ж�ȡ��������
    res_tif.tif_matrix2.create(res_tif.tif_height , res_tif.tif_width);
    for (uint32_t row = 0; row < res_tif.tif_height; ++row)
    {
        TIFFReadScanline(tiff, res_tif.tif_matrix2.get_data()[row], row);
    }

    // �ر�Ӱ�����ݺ�GTIF����
    XTIFFClose(tiff);
    GTIFFree(gtif);
    return res_tif;
}


bool geotiff_utils::writeGeoTiff(std::string filePath, tif_data tif_matrix2)
{
    bool status = true;
    // ��ȡ���������������
    size_t cols = tif_matrix2.tif_width;
    size_t rows = tif_matrix2.tif_height;

    // �� TIFF �ļ�����д��
    TIFF* tiff = TIFFOpen(filePath.c_str(),  "w ");
    if (!tiff) {
        std::cerr <<  "�޷��� TIFF �ļ�����д�� " << std::endl;
        status = false;
    }

    // ���� TIFF ͼ��Ļ�����Ϣ
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, cols);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, rows);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, tif_matrix2.tif_samplesPerPixel);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, tif_matrix2.tif_bitsPerSample);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, tif_matrix2.tif_sampleFormat);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);       //ͼ��ķ����ǩ
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);      // ͼ���ƽ�����ñ�ǩ
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);    // ͼ��Ĺ�ȱ�ǩ


    double pixelScale[3] = { tif_matrix2.pixelSizeX, tif_matrix2.pixelSizeY, 1.0 };
    double modelTransform[6] = { 0.0, 0.0, 0.0,tif_matrix2.tif_letf , tif_matrix2.tif_top, 0.0 };

    TIFFSetField(tiff, GTIFF_PIXELSCALE,3, &pixelScale);
    TIFFSetField(tiff, GTIFF_TIEPOINTS, 6, &modelTransform);

    // д����������
    for (size_t row = 0; row < rows; ++row) {
        TIFFWriteScanline(tiff, tif_matrix2.tif_matrix2.get_data()[row], row);
    }

    // �ر� TIFF �ļ�
    TIFFClose(tiff);

    std::cout <<  "TIFF �ļ�д��ɹ� " << std::endl;
    return status;

}




tif_data geotiff_utils::readGeoTiffTile(std::string filePath)
{
    tif_data tileData;
    TIFF* tif = TIFFOpen(filePath.c_str(), "r");
    if (tif == nullptr) {
        // �����tif�ļ�ʧ�ܵ����
        return tileData;
    }

    // ��ȡtif�ļ��Ļ�����Ϣ
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &(tileData.tif_width)); // ��ȡͼ����
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &(tileData.tif_height)); // ��ȡͼ��߶�
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &(tileData.tif_bitsPerSample)); // ��ȡÿ�����ݵ�λ��
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &(tileData.tif_samplesPerPixel)); // ��ȡÿ�����ص�������

    std::cout << "GTIFF_TIEPOINTS �������� " << std::endl;
    double* tif_coordinate;
    int tif_coordinate_count;
    TIFFGetField(tif, GTIFF_TIEPOINTS, &tif_coordinate_count, &tif_coordinate);
    if (tif_coordinate_count > 4)
    {
        tileData.tif_letf = tif_coordinate[3];
        tileData.tif_top = tif_coordinate[4];
        std::cout << "tif_letf: " << tileData.tif_letf << std::endl;
        std::cout << "tif_top: " << tileData.tif_top << std::endl;
    }
    else
    {
        std::cout << "��ȡ��������ʧ�� " << std::endl;
        return tileData;
    }


    std::cout << "GTIFF_PIXELSCALE ���طֱ��� " << std::endl;
    double* pixel_scale = NULL;
    int pixel_scale_count;
    TIFFGetField(tif, GTIFF_PIXELSCALE, &pixel_scale_count, &pixel_scale);
    if (pixel_scale_count > 2)
    {
        tileData.pixelSizeX = pixel_scale[0];
        tileData.pixelSizeY = pixel_scale[1];
        std::cout << "pixelSizeX: " << tileData.pixelSizeX << std::endl;
        std::cout << "pixelSizeY: " << tileData.pixelSizeY << std::endl;
    }
    else
    {
        std::cout << "���طֱ���ʧ�� " << std::endl;
        return tileData;
    }


    if (TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &tileData.tif_sampleFormat))
    {
        if (tileData.tif_sampleFormat == SAMPLEFORMAT_UINT)
        {
            std::cout << "�������ͣ�uint " << std::endl;
        }
        else if (tileData.tif_sampleFormat == SAMPLEFORMAT_INT)
        {
            std::cout << "�������ͣ�int " << std::endl;
        }
        else if (tileData.tif_sampleFormat == SAMPLEFORMAT_IEEEFP)
        {
            std::cout << "�������ͣ�float " << std::endl;
        }
        else if (tileData.tif_sampleFormat == SAMPLEFORMAT_VOID)
        {
            std::cout << "�������ͣ�δ֪�������� " << std::endl;
        }
        else
        {
            // ����δ֪��������
            std::cout << "δ֪���������� " << std::endl;
        }
    }
    else
    {
        std::cout << "�޷���ȡ�������� " << std::endl;
        return tileData;
    }
    int ignore = FALSE;
    tileData.tif_matrix2.create(tileData.tif_height, tileData.tif_width);
    unsigned char* buffer = (unsigned char*)_TIFFmalloc(TIFFTileSize(tif));
    // ��ȡ��һ����Ƭ���ݣ�������Ƭ����������float��
    if (TIFFReadTile(tif, buffer, 0, 0, 0, 0) < 0 && !ignore){
        // �����ȡ��������
        XTIFFClose(tif);
    }
    //// ��ȡ tile �Ŀ�Ⱥ͸߶�
    //TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileData.tif_tileWidth);
    //TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileData.tif_tileHeight);
    //int numTiles = TIFFNumberOfTiles(tif);
    //// ��ȡͼ������
    //for (uint32_t row = 0; row < tileData.tif_height; row += tileData.tif_tileHeight) {
    //    for (uint32_t col = 0; col < tileData.tif_width; col += tileData.tif_tileWidth) {
    //        TIFFReadTile(tif, &(tileData.tif_matrix2.at(row, col)), col, row, 0, 0);
    //    }
    //}

    XTIFFClose(tif);
    return tileData;
}


bool geotiff_utils::writeGeoTiffTile(std::string filePath, tif_data tif_matrix2)
{
    bool status = true;
    // ��ȡ���������������
    size_t cols = tif_matrix2.tif_width;
    size_t rows = tif_matrix2.tif_height;

    // �� TIFF �ļ�����д��
    TIFF* tiff = TIFFOpen(filePath.c_str(), "w ");
    if (!tiff) {
        std::cerr << "�޷��� TIFF �ļ�����д�� " << std::endl;
        status = false;
    }

    // ���� TIFF ͼ��Ļ�����Ϣ
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, cols);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, rows);
    TIFFSetField(tiff, TIFFTAG_TILEWIDTH, tif_matrix2.tif_tileWidth);
    TIFFSetField(tiff, TIFFTAG_TILELENGTH, tif_matrix2.tif_tileHeight);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, tif_matrix2.tif_samplesPerPixel);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, tif_matrix2.tif_bitsPerSample);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, tif_matrix2.tif_sampleFormat);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);       //ͼ��ķ����ǩ
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);      // ͼ���ƽ�����ñ�ǩ
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);    // ͼ��Ĺ�ȱ�ǩ


    double pixelScale[3] = { tif_matrix2.pixelSizeX, tif_matrix2.pixelSizeY, 1.0 };
    double modelTransform[6] = { 0.0, 0.0, 0.0,tif_matrix2.tif_letf , tif_matrix2.tif_top, 0.0 };

    TIFFSetField(tiff, GTIFF_PIXELSCALE, 3, &pixelScale);
    TIFFSetField(tiff, GTIFF_TIEPOINTS, 6, &modelTransform);

    // д����������
    // ��ͼ������д�� TIFF �ļ�
    for (uint32_t row = 0; row < tif_matrix2.tif_height; row += tif_matrix2.tif_tileHeight) {
        for (uint32_t col = 0; col < tif_matrix2.tif_width; col += tif_matrix2.tif_tileWidth) {
            TIFFWriteTile(tiff, &(tif_matrix2.tif_matrix2.at(row, col)), col, row, 0, 0);
        }
    }

    // �ر� TIFF �ļ�
    TIFFClose(tiff);

    std::cout << "TIFF �ļ�д��ɹ� " << std::endl;
    return status;

}