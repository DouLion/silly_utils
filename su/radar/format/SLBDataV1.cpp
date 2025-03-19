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
            nationalBlock.Th.clear();
            nationalBlock.Zh.clear();
            nationalBlock.V.clear();
            nationalBlock.W.clear();
            nationalBlock.ZDR.clear();
            nationalBlock.PHIDP.clear();
            nationalBlock.KDP.clear();
            nationalBlock.CC.clear();
            nationalBlock.Tv.clear();

            // 遍历每个数据元素类型
            for (int elemType = 0; elemType < src.m_RadioHeader.MomentNumber; ++elemType)
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
                    case 0:  // dBT
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

                    case 1:  // dBZ
                        for (const short val : rawVec)
                        {
                            if (val == MINSHORT)
                            {
                                nationalBlock.Th.push_back(0);
                            }
                            else
                            {
                                float hn_real_Val = (val - elevData.Offset[elemType]) * 1.0f / elevData.Scale[elemType];  // 湖
                                float f_c_stored_val = ((hn_real_Val * 2) + 66);
                                uint8_t i_c_stored_val = static_cast<uint8_t>(f_c_stored_val);
                                nationalBlock.Th.push_back(i_c_stored_val);
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
                    case 8:  // LDR → Tv
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

#include <fstream>
#include <vector>
#include <cstring>

bool SLBDataV1::write(const std::string& filename)
{
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open())
    {
        return false;
    }
    // ---- 公共数据块 ----

    // 写入文件卷信息
    outFile.write(fileVol.VolumeLabel, sizeof(fileVol.VolumeLabel));
    outFile.write(fileVol.VersionNo, sizeof(fileVol.VersionNo));
    outFile.write(reinterpret_cast<const char*>(&fileVol.FileLength), sizeof(fileVol.FileLength));
    outFile.write(reinterpret_cast<const char*>(&fileVol.RayOrder), sizeof(fileVol.RayOrder));

    // 写入雷达站基本信息
    outFile.write(siteInfo.Country, sizeof(siteInfo.Country));
    outFile.write(siteInfo.Province, sizeof(siteInfo.Province));
    outFile.write(siteInfo.SiteName, sizeof(siteInfo.SiteName));
    outFile.write(siteInfo.SiteID, sizeof(siteInfo.SiteID));
    outFile.write(siteInfo.RadarType, sizeof(siteInfo.RadarType));
    outFile.write(reinterpret_cast<const char*>(&siteInfo.Longitude), sizeof(siteInfo.Longitude));
    outFile.write(reinterpret_cast<const char*>(&siteInfo.Latitude), sizeof(siteInfo.Latitude));
    outFile.write(reinterpret_cast<const char*>(&siteInfo.Height), sizeof(siteInfo.Height));
    outFile.write(siteInfo.Reserved, sizeof(siteInfo.Reserved));

    // 写入观测时间
    outFile.write(reinterpret_cast<const char*>(&obTime.SYear), sizeof(obTime.SYear));
    outFile.write(reinterpret_cast<const char*>(&obTime.SMonth), sizeof(obTime.SMonth));
    outFile.write(reinterpret_cast<const char*>(&obTime.SDay), sizeof(obTime.SDay));
    outFile.write(reinterpret_cast<const char*>(&obTime.SHour), sizeof(obTime.SHour));
    outFile.write(reinterpret_cast<const char*>(&obTime.SMinute), sizeof(obTime.SMinute));
    outFile.write(reinterpret_cast<const char*>(&obTime.SSecond), sizeof(obTime.SSecond));
    outFile.write(reinterpret_cast<const char*>(&obTime.EYear), sizeof(obTime.EYear));
    outFile.write(reinterpret_cast<const char*>(&obTime.EMonth), sizeof(obTime.EMonth));
    outFile.write(reinterpret_cast<const char*>(&obTime.EDay), sizeof(obTime.EDay));
    outFile.write(reinterpret_cast<const char*>(&obTime.EHour), sizeof(obTime.EHour));
    outFile.write(reinterpret_cast<const char*>(&obTime.EMinute), sizeof(obTime.EMinute));
    outFile.write(reinterpret_cast<const char*>(&obTime.ESecond), sizeof(obTime.ESecond));
    outFile.write(obTime.Reserved, sizeof(obTime.Reserved));

    // 写入运维数据信息
    outFile.write(reinterpret_cast<const char*>(&opInfo.AntennaCenterHeight), sizeof(opInfo.AntennaCenterHeight));
    outFile.write(reinterpret_cast<const char*>(&opInfo.RadarTerminalVersion), sizeof(opInfo.RadarTerminalVersion));
    outFile.write(reinterpret_cast<const char*>(&opInfo.Frequency), sizeof(opInfo.Frequency));
    outFile.write(opInfo.Date, sizeof(opInfo.Date));
    outFile.write(opInfo.Time, sizeof(opInfo.Time));
    outFile.write(reinterpret_cast<const char*>(&opInfo.ScanModel), sizeof(opInfo.ScanModel));
    outFile.write(reinterpret_cast<const char*>(&opInfo.ControlFlag), sizeof(opInfo.ControlFlag));
    outFile.write(reinterpret_cast<const char*>(&opInfo.SystemStatus), sizeof(opInfo.SystemStatus));
    outFile.write(reinterpret_cast<const char*>(&opInfo.version), sizeof(opInfo.version));
    outFile.write(reinterpret_cast<const char*>(&opInfo.DPRFlag), sizeof(opInfo.DPRFlag));
    outFile.write(reinterpret_cast<const char*>(&opInfo.RotatingRate), sizeof(opInfo.RotatingRate));
    outFile.write(reinterpret_cast<const char*>(&opInfo.TransmitterPower), sizeof(opInfo.TransmitterPower));
    outFile.write(reinterpret_cast<const char*>(&opInfo.TransmitterStatus), sizeof(opInfo.TransmitterStatus));
    outFile.write(reinterpret_cast<const char*>(&opInfo.MotorMode), sizeof(opInfo.MotorMode));
    outFile.write(reinterpret_cast<const char*>(&opInfo.RadarBaseTemperature), sizeof(opInfo.RadarBaseTemperature));
    outFile.write(reinterpret_cast<const char*>(&opInfo.RadarBaseHumidity), sizeof(opInfo.RadarBaseHumidity));
    outFile.write(reinterpret_cast<const char*>(&opInfo.MainPower1Temperature), sizeof(opInfo.MainPower1Temperature));
    outFile.write(reinterpret_cast<const char*>(&opInfo.MainPower2Temperature), sizeof(opInfo.MainPower2Temperature));
    outFile.write(reinterpret_cast<const char*>(&opInfo.MainPower1InVoltage), sizeof(opInfo.MainPower1InVoltage));
    outFile.write(reinterpret_cast<const char*>(&opInfo.MainPower2InVoltage), sizeof(opInfo.MainPower2InVoltage));
    outFile.write(reinterpret_cast<const char*>(&opInfo.MainPower1OutVoltage), sizeof(opInfo.MainPower1OutVoltage));
    outFile.write(reinterpret_cast<const char*>(&opInfo.MainPower2OutVoltage), sizeof(opInfo.MainPower2OutVoltage));
    outFile.write(opInfo.AirConStatus1, sizeof(opInfo.AirConStatus1));
    outFile.write(opInfo.AirConStatus2, sizeof(opInfo.AirConStatus2));
    outFile.write(opInfo.SabA, sizeof(opInfo.SabA));
    outFile.write(opInfo.SabB, sizeof(opInfo.SabB));
    outFile.write(reinterpret_cast<const char*>(&opInfo.PeakPower), sizeof(opInfo.PeakPower));
    outFile.write(reinterpret_cast<const char*>(&opInfo.PulseWidth), sizeof(opInfo.PulseWidth));
    outFile.write(reinterpret_cast<const char*>(&opInfo.ReflectivityExpectedValue), sizeof(opInfo.ReflectivityExpectedValue));
    outFile.write(reinterpret_cast<const char*>(&opInfo.ReflectivityMeasuredValue), sizeof(opInfo.ReflectivityMeasuredValue));
    outFile.write(reinterpret_cast<const char*>(&opInfo.CalibrationConstant), sizeof(opInfo.CalibrationConstant));
    outFile.write(reinterpret_cast<const char*>(&opInfo.ZDRCalibrationValue), sizeof(opInfo.ZDRCalibrationValue));
    outFile.write(reinterpret_cast<const char*>(&opInfo.DPCalibrationValue), sizeof(opInfo.DPCalibrationValue));
    outFile.write(opInfo.Reserved, sizeof(opInfo.Reserved));

    // // 1~M 个告警数据块
    outFile.write(reinterpret_cast<const char*>(&alerts.AlertCounts), sizeof(alerts.AlertCounts));
    outFile.write(alerts.Reserved, sizeof(alerts.Reserved));
    // 写入数组
    writeVector(outFile, alerts.AlertInfoList);

    // 写入数据记录块信息
    uint32_t dataRecordCount = dataRecords.size();
    // 写入数据记录块数量
    outFile.write(reinterpret_cast<const char*>(&dataRecordCount), sizeof(uint32_t));
    for (const auto& record : dataRecords)
    {
        writeLayerParam(outFile, record);
    }

    // ---- 径向数据块 ----
    //// 写入径向数据块信息
    writeVector(outFile, radialData);
    uint32_t radialDataCount = radialData.size();
    outFile.write(reinterpret_cast<const char*>(&radialDataCount), sizeof(uint32_t));
    for (const auto& radial : radialData)
    {
        writeRadialDataBlock(outFile, radial);
    }

    outFile.close();
    return true;
}

void SLBDataV1::writeRadialDataBlock(std::ofstream& outFile, const RadialDataBlock& block)
{
    outFile.write(reinterpret_cast<const char*>(&block.RadialState), sizeof(block.RadialState));
    outFile.write(reinterpret_cast<const char*>(&block.SequenceNumber), sizeof(block.SequenceNumber));
    outFile.write(reinterpret_cast<const char*>(&block.RadialNumber), sizeof(block.RadialNumber));
    outFile.write(reinterpret_cast<const char*>(&block.ElevationNumber), sizeof(block.ElevationNumber));
    outFile.write(reinterpret_cast<const char*>(&block.Azimuth), sizeof(block.Azimuth));
    outFile.write(reinterpret_cast<const char*>(&block.Elevation), sizeof(block.Elevation));
    outFile.write(reinterpret_cast<const char*>(&block.Seconds), sizeof(block.Seconds));
    outFile.write(reinterpret_cast<const char*>(&block.LengthOfData), sizeof(block.LengthOfData));
    outFile.write(reinterpret_cast<const char*>(&block.VarCounts), sizeof(block.VarCounts));
    writeVector(outFile, block.DateType);
    writeVector(outFile, block.Scale);
    writeVector(outFile, block.Offset);
    outFile.write(block.Reserved, sizeof(block.Reserved));

    // 写入数据
    writeVector(outFile, block.Th);
    writeVector(outFile, block.Zh);
    writeVector(outFile, block.V);
    writeVector(outFile, block.W);
    writeVector(outFile, block.ZDR);
    writeVector(outFile, block.PHIDP);
    writeVector(outFile, block.KDP);
    writeVector(outFile, block.CC);
    writeVector(outFile, block.Tv);
}

void SLBDataV1::writeLayerParam(std::ofstream& outFile, const DataRecordBlock& record)
{
    outFile.write(reinterpret_cast<const char*>(&record.LayerCounts), sizeof(record.LayerCounts));
    outFile.write(reinterpret_cast<const char*>(&record.RadialCounts), sizeof(record.RadialCounts));
    outFile.write(reinterpret_cast<const char*>(&record.AngularResolution), sizeof(record.AngularResolution));
    outFile.write(reinterpret_cast<const char*>(&record.DataWidth), sizeof(record.DataWidth));
    outFile.write(record.Reserved, sizeof(record.Reserved));

    // 写入LayerParamList
    uint32_t layerParamCount = record.LayerParamList.size();
    outFile.write(reinterpret_cast<const char*>(&layerParamCount), sizeof(uint32_t));
    for (const auto& layerParam : record.LayerParamList)
    {
        outFile.write(reinterpret_cast<const char*>(&layerParam.Dbegin), sizeof(layerParam.Dbegin));
        outFile.write(reinterpret_cast<const char*>(&layerParam.ElevationAngle), sizeof(layerParam.ElevationAngle));
        outFile.write(reinterpret_cast<const char*>(&layerParam.MaxV), sizeof(layerParam.MaxV));
        outFile.write(reinterpret_cast<const char*>(&layerParam.MinL), sizeof(layerParam.MinL));
        outFile.write(reinterpret_cast<const char*>(&layerParam.VarCounts), sizeof(layerParam.VarCounts));
        writeVector(outFile, layerParam.VarCode);     // 写入VarCode
        writeVector(outFile, layerParam.GateCounts);  // 写入GateCounts
        outFile.write(layerParam.Reserved2, sizeof(layerParam.Reserved2));
    }
}