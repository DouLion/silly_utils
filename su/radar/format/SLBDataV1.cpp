/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-28
 * @file: SLBDataV1.cpp
 * @description: SLBDataV1实现
 * @version: v1.0.1 2025-02-28 dou li yang
 */
#include "SLBDataV1.h"

using namespace radar;

bool SLBDataV1::convert(const HunanData& src)
{
    bool status = false;
    std::string emptyStr = "";
    // ---- 公共数据块 ----
    // fileVol.VolumeLabel = ;
    // fileVol.VersionNo = ;
    // fileVol.FileLength = 10;
    // fileVol.RayOrder = PitchFirst;
    // siteInfo.Country =
    // siteInfo.Province =
    std::strncpy(siteInfo.SiteName, src.m_Header.PerformanceInfo.SiteName, sizeof(siteInfo.SiteName) - 1);
    std::strncpy(siteInfo.SiteID, src.m_Header.PerformanceInfo.SiteCode, sizeof(src.m_Header.PerformanceInfo.SiteCode) - 1);
    siteInfo.Longitude = src.m_Header.PerformanceInfo.longitude / 10000.0;
    siteInfo.Latitude = src.m_Header.PerformanceInfo.latitude / 10000.0;
    siteInfo.Height = src.m_Header.PerformanceInfo.height / 10.0;

    for (int l = 0; l < src.m_Header.ObservationInfo.CutNumber; l++)
    {
        DataRecordBlock tempRecordBlock;
        tempRecordBlock.AngularResolution = src.m_ScanCut[l].AngularResolution;
        dataRecords.push_back(tempRecordBlock);
    }

    // ---- 径向数据块 ----

    // 遍历每个仰角层
    for (size_t l = 0; l < src.m_Data.size(); ++l)
    {
        const auto& elevData = src.m_Data[l];

        // 遍历每个径向（0-359度）
        for (int degree = 0; degree < 360; ++degree)
        {
            RadialDataBlock nationalBlock;

            // 填充基础头信息
            const auto& hunanHeader = src.m_RadioHeader;
            nationalBlock.RadialState = hunanHeader.RadialState;
            nationalBlock.SequenceNumber = hunanHeader.SeqNumber;
            nationalBlock.RadialNumber = hunanHeader.RadialNumber;
            nationalBlock.ElevationNumber = hunanHeader.ElevationNumber;
            nationalBlock.Azimuth = hunanHeader.Azimuth;
            nationalBlock.Elevation = hunanHeader.Elevation;
            nationalBlock.Seconds = hunanHeader.Seconds;
            nationalBlock.LengthOfData = hunanHeader.dataLength;

            // 初始化变量容器
            nationalBlock.VarCounts = 0;
            nationalBlock.DateType.clear();
            nationalBlock.Scale.clear();
            nationalBlock.Offset.clear();
            nationalBlock.Child.clear();
            nationalBlock.Zh.clear();
            nationalBlock.V.clear();
            nationalBlock.W.clear();
            nationalBlock.ZDR.clear();
            nationalBlock.PHIDP.clear();
            nationalBlock.KDP.clear();
            nationalBlock.CC.clear();
            nationalBlock.Tv.clear();

            // 遍历每个数据元素类型
            for (int elemType = 0; elemType < ELEMENTNUM; ++elemType)
            {
                const auto& rawVec = elevData.rawdata[degree][elemType];
                if (rawVec.empty())
                {
                    continue;
                }
                // 添加变量元数据
                nationalBlock.VarCounts++;
                nationalBlock.DateType.push_back(1);  // 默认BYTE类型
                nationalBlock.Scale.push_back(elevData.Scale[elemType]);
                nationalBlock.Offset.push_back(elevData.Offset[elemType]);

                // 数据转换逻辑
                switch (elemType)
                {
                    case 0:  // dBZ → Zh
                        for (const short hn_stored_val : rawVec)
                        {
                            if (hn_stored_val == MINSHORT)
                            {
                                nationalBlock.Zh.push_back(0);
                                continue;
                            }

                            float hn_real_Val = (hn_stored_val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];  // 湖南真实数据
                            float f_c_stored_val = ((hn_real_Val * 2) + 66);
                            uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                            nationalBlock.Zh.push_back(i_c_stored_val);
                        }
                        break;

                    case 1:  // REF → Child
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.Child.push_back(0);
                            }
                            else
                            {
                                float hn_real_Val = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];  // 湖
                                float f_c_stored_val = ((hn_real_Val * 2) + 66);
                                uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                                nationalBlock.Child.push_back(i_c_stored_val);
                                // nationalBlock.Child.push_back(static_cast<int8_t>((val * 2) + 66));
                            }
                        }
                        break;

                    case 2:  // V
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.V.push_back(0);
                                continue;
                            }
                            float hn_real_Val = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];
                            float f_c_stored_val = ((hn_real_Val * 2) + 129);
                            uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                            nationalBlock.V.push_back(i_c_stored_val);
                            // nationalBlock.V.push_back(static_cast<int8_t>((realV * 2) + 129));
                        }
                        break;

                    case 3:  // W
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.W.push_back(0);
                                continue;
                            }
                            float hn_realW = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];
                            float f_c_stored_val = ((hn_realW * 2) + 129);
                            uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                            nationalBlock.W.push_back(i_c_stored_val);
                            // nationalBlock.W.push_back(static_cast<int8_t>((hn_realW * 2) + 129));
                        }
                        break;

                    case 4:  // ZDR
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.ZDR.push_back(0);
                                continue;
                            }
                            float realZDR = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];
                            float f_c_stored_val = ((realZDR * 16) + 130);
                            uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                            nationalBlock.ZDR.push_back(i_c_stored_val);
                            // nationalBlock.ZDR.push_back(static_cast<int8_t>((realZDR * 16) + 130));
                        }
                        break;

                    case 5:  // KDP
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.KDP.push_back(0);
                                continue;
                            }
                            float realKDP = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];
                            float f_c_stored_val = ((realKDP * 10) + 50);
                            uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                            nationalBlock.KDP.push_back(i_c_stored_val);
                            // nationalBlock.KDP.push_back(static_cast<int8_t>((realKDP * 10) + 50));
                        }
                        break;

                    case 6:  // PHIDP
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.PHIDP.push_back(0);
                                continue;
                            }
                            float realPHI = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];
                            float f_c_stored_val = ((realPHI * 100) + 50);
                            int16_t i_c_stored_val = static_cast<int16_t>(f_c_stored_val);
                            nationalBlock.PHIDP.push_back(i_c_stored_val);
                            // nationalBlock.PHIDP.push_back(static_cast<int16_t>((realPHI * 100) + 50));
                        }
                        nationalBlock.DateType.back() = 3;  // SHORT类型
                        break;

                    case 7:  // CC
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.CC.push_back(0);
                                continue;
                            }
                            float realCC = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];
                            float f_c_stored_val = ((realCC * 200) + 5);
                            uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                            nationalBlock.CC.push_back(i_c_stored_val);
                            // nationalBlock.CC.push_back(static_cast<uint8_t>((realCC * 200) + 5));
                        }
                        break;

                    case 8:  // SNR
                        for (const short val : rawVec)
                        {
                            nationalBlock.OriginTh = static_cast<uint8_t>((val > 4) ? 1 : 4);
                        }
                        break;

                    case 9:  // LDR → Tv
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.Tv.push_back(0);
                                continue;
                            }
                            float realTv = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];
                            float f_c_stored_val = ((realTv * 2) + 66);
                            uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                            nationalBlock.Tv.push_back(i_c_stored_val);
                            // nationalBlock.Tv.push_back(static_cast<uint8_t>((realTv * 2) + 66));
                        }
                        break;
                }
            }

            // 添加转换后的数据块
            radialData.push_back(nationalBlock);
        }
    }

    status = true;
    return status;
}
