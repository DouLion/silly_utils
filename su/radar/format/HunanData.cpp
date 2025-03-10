/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-03-05
 * @file: HunanData.cpp
 * @description: HunanData实现
 * @version: v1.0.1 2025-03-05 dou li yang
 */
#include "HunanData.h"

using namespace radar;

void HunanData::Clear()
{
    for (auto t : m_Data)
    {
        t.clear();
    }
    m_Data.clear();
    memset(&m_Header, 0, sizeof(bzNewRadarHeader));
    memset(&m_ScanCut, 0, sizeof(bzSCANPARAM) * 256);
    memset(&m_RadioHeader, 0, sizeof(bzRadialDataHead));
    memset(&m_RadialDatab, 0, sizeof(bzRadialDatab));
}

bool HunanData::LoadData(const char* FilePath)
{
    FILE* File = fopen(FilePath, "rb");
    if (!File)
    {
        return false;
    }
    Clear();
    unsigned char byte1;
    unsigned short byte2;

    // 1. 读取通用头 站点配置 任务配置
    size_t bytesRead = fread(&m_Header, 1, sizeof(bzNewRadarHeader), File);
    if (bytesRead != sizeof(bzNewRadarHeader))
    {
        fclose(File);
        return false;
    }

    int Rotate = 0;
    std::string SiteName = m_Header.PerformanceInfo.SiteName;
    if (SiteName == "YUNTAISHAN")
    {
        Rotate = -5;
    }
    else if (SiteName == "WUQIANGXI")
    {
        Rotate = 125;
    }
    else if (SiteName == "JIEMUXI")
    {
        Rotate = 150;
    }
    else
    {
        Rotate = -190;
    }

    bool bWriteFile = true;
    char SaveFilePath[1024];
    FILE* tmpFile;
    std::map<int, FILE*> mapFileWrite;
    if (bWriteFile)
    {
        snprintf(SaveFilePath, sizeof(SaveFilePath), "./%s", "data");
        std::filesystem::create_directories(SaveFilePath);
    }

    // 2. 读取所有CUT层的扫描配置
    size_t bzScanSize = fread(&m_ScanCut, sizeof(bzSCANPARAM), m_Header.ObservationInfo.CutNumber, File);
    if (bzScanSize != m_Header.ObservationInfo.CutNumber)
    {
        fclose(File);
        return false;
    }

    m_Data.resize(m_Header.ObservationInfo.CutNumber);

    // 循环每个仰角层
    for (int l = 0; l < m_Header.ObservationInfo.CutNumber; l++)
    {
        // 创建仰角目录
        if (bWriteFile)
        {
            snprintf(SaveFilePath, sizeof(SaveFilePath), "./data/%.1f", m_ScanCut[l].Azimuth);
            std::filesystem::create_directories(SaveFilePath);

            // 创建CSV文件
            snprintf(SaveFilePath, sizeof(SaveFilePath), "./data/%.1f/REF.csv", m_ScanCut[l].Azimuth);
            std::filesystem::path csvPath(SaveFilePath);
            std::filesystem::create_directories(csvPath.parent_path());
            tmpFile = fopen(SaveFilePath, "w");
            if (tmpFile)
            {
                mapFileWrite[1] = tmpFile;
            }

            // 创建DAT文件
            snprintf(SaveFilePath, sizeof(SaveFilePath), "./data/%.1f/REF.dat", m_ScanCut[l].Azimuth);
            std::filesystem::path datPath(SaveFilePath);
            std::filesystem::create_directories(datPath.parent_path());
            tmpFile = fopen(SaveFilePath, "wb");
            if (tmpFile)
            {
                mapFileWrite[1 + 100] = tmpFile;
            }
        }

        // 循环处理每个径向
        for (int k = 0; k < 360; k++)
        {
            // 读取径向头
            bytesRead = fread(&m_RadioHeader, 1, sizeof(bzRadialDataHead), File);
            if (bytesRead != sizeof(bzRadialDataHead))
            {
                fclose(File);
                return false;
            }

            std::vector<short> vData;
            short val = 0;
            for (int j = 0; j < m_RadioHeader.MomentNumber; j++)
            {
                // 读取径向数据头
                bytesRead = fread(&m_RadialDatab, 1, sizeof(bzRadialDatab), File);
                if (bytesRead != sizeof(bzRadialDatab) || m_RadialDatab.BinLength <= 0)
                {
                    fclose(File);
                    return false;
                }

                // 读取径向数据
                vData.clear();
                int len = m_RadialDatab.length / m_RadialDatab.BinLength;
                for (int i = 0; i < len; i++)
                {
                    switch (m_RadialDatab.DataType)
                    {
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 7:
                        case 8:
                        case 9:
                        case 11:
                        case 16:
                            if (fread(&byte1, 1, 1, File) != 1)
                            {
                                fclose(File);
                                return false;
                            }
                            val = (byte1 < 5) ? -32768 : (byte1 - m_RadialDatab.Offset);
                            vData.push_back(val);
                            break;
                        case 10:
                            if (fread(&byte2, 1, 2, File) != 2)
                            {
                                fclose(File);
                                return false;
                            }
                            val = (byte2 < 5) ? -32768 : (byte2 - m_RadialDatab.Offset);
                            vData.push_back(val);
                            break;
                        default:
                            fseek(File, m_RadialDatab.BinLength, SEEK_CUR);
                            break;
                    }
                }

                // 数据类型映射
                int cType = -1;
                switch (m_RadialDatab.DataType)
                {
                    case 1:
                        cType = 1;
                        break;
                    case 2:
                        cType = 0;
                        break;
                    case 3:
                        cType = 2;
                        break;
                    case 4:
                        cType = 3;
                        break;
                    case 7:
                        cType = 4;
                        break;
                    case 8:
                        cType = 9;
                        break;
                    case 9:
                        cType = 7;
                        break;
                    case 10:
                        cType = 6;
                        break;
                    case 11:
                        cType = 5;
                        break;
                    case 16:
                        cType = 8;
                        break;
                }

                // 调整角度
                int Degree = static_cast<int>(m_RadioHeader.Azimuth + Rotate + 360.0) % 360;
                if (cType >= 0 && cType < ELEMENTNUM)
                {
                    m_Data[l].Offset[cType] = m_RadialDatab.Offset;
                    m_Data[l].Scale[cType] = m_RadialDatab.Scale;
                    m_Data[l].rawdata[Degree][cType].swap(vData);
                }
            }
        }
    }
    fclose(File);

    // 后续数据处理
    for (auto& data : m_Data)
    {
        data.ScanData();
    }

    // 关闭所有输出文件
    for (auto& pair : mapFileWrite)
    {
        if (pair.second)
            fclose(pair.second);
    }

    return true;
}

//bool HunanData::LoadData(const char* FilePath)
//{
//    FILE* File;
//    if (fopen_s(&File, FilePath, "rb") != 0)
//    {
//        return false;
//    }
//    Clear();
//    unsigned char byte1;
//    unsigned short byte2;
//
//    //  1. 读取 通用头 站点配置 任务配置
//    size_t bytesRead = fread(&m_Header, 1, sizeof(bzNewRadarHeader), File);
//    if (bytesRead != sizeof(bzNewRadarHeader))
//    {
//        // 处理读取错误
//        return false;
//    }
//
//    int Rotate = 0;
//    std::string SiteName = m_Header.PerformanceInfo.SiteName;
//    if (SiteName == "YUNTAISHAN")
//    {
//        Rotate = -5;
//    }
//    else if (SiteName == "WUQIANGXI")
//    {
//        Rotate = 125;
//    }
//    else if (SiteName == "JIEMUXI")
//    {
//        Rotate = 150;
//    }
//    else
//    {
//        Rotate = -190;
//    }
//
//    bool bWriteFile = true;
//    char SaveFilePath[1024];
//    FILE* tmpFile;
//    std::map<int, FILE*> mapFileWrite;
//    if (bWriteFile)
//    {
//        sprintf_s(SaveFilePath, sizeof(SaveFilePath), ".\\%s", "data");
//        std::filesystem::create_directories(SaveFilePath);
//        // Method::MakeDir(SaveFilePath);
//    }
//
//    // 2. 读出所有CUT层（仰角N）的扫描配置
//    size_t bzScanSize = fread(&m_ScanCut, m_Header.ObservationInfo.CutNumber, sizeof(bzSCANPARAM), File);
//
//    m_Data.resize(m_Header.ObservationInfo.CutNumber);
//
//    // 循环每个N
//    for (int l = 0; l < m_Header.ObservationInfo.CutNumber; l++)
//    {
//        // 仰角目录
//        if (bWriteFile)
//        {
//            sprintf_s(SaveFilePath, sizeof(SaveFilePath), ".\\%s\\%0.1f", "data", m_ScanCut[l].Azimuth);
//            std::filesystem::create_directories(SaveFilePath);
//            // Method::MakeDir(SaveFilePath);
//
//            sprintf_s(SaveFilePath, sizeof(SaveFilePath), ".\\%s\\%0.1f\\REF.csv", "data", m_ScanCut[l].Azimuth);
//            if (fopen_s(&tmpFile, FilePath, "w") != 0)
//            {
//                mapFileWrite[1] = tmpFile;
//            }
//            sprintf_s(SaveFilePath, sizeof(SaveFilePath), ".\\%s\\%0.1f\\REF.dat", "data", m_ScanCut[l].Azimuth);
//            std::filesystem::create_directories(SaveFilePath);
//            // Method::MakeDir(SaveFilePath);
//            if (fopen_s(&tmpFile, FilePath, "w") != 0)
//            {
//                mapFileWrite[1 + 100] = tmpFile;
//            }
//        }
//
//        // m_Radius = m_ScanCut[l].MaxRange1 / 1000.0; // ?是否有用这变量
//        //  循环每个径向M个
//        for (int k = 0; k < 360; k++)
//        {  // 360
//
//            // 径向头
//            size_t byteRadioRead = fread(&m_RadioHeader, 1, sizeof(bzRadialDataHead), File);
//            if (byteRadioRead != sizeof(bzRadialDataHead))
//            {
//                // 处理读取错误
//                return false;
//            }
//            // 循环Moment数
//            std::vector<short> vData;
//            short val = 0;
//            for (int j = 0; j < m_RadioHeader.MomentNumber; j++)
//            {  // K,10种数据 dBz,zDR,KDP...
//
//                // 径向数据头
//                size_t bytesMomentRead = fread(&m_RadialDatab, 1, sizeof(bzRadialDatab), File);
//                if (m_RadialDatab.BinLength <= 0)
//                {
//                    // 处理错误，例如记录日志并退出
//                    // continue;
//                    return false;
//                }
//
//                // 读径向数据
//                vData.clear();
//                int len = m_RadialDatab.length / m_RadialDatab.BinLength;
//                // vData.resize(len, 0);
//                for (int i = 0; i < len; i++)  // length = 1334,
//                {
//                    switch (m_RadialDatab.DataType)
//                    {
//                        case 1:   // dBT滤波前反射率 REF
//                        case 2:   // dBZ滤波后反射率
//                        case 3:   // V径向速度
//                        case 4:   // W谱宽
//                        case 7:   // ZDR差分反射率
//                        case 8:   // LDR退偏振比
//                        case 9:   // CC协相关系数
//                        case 11:  // KDP差分相移率
//                        case 16:  // SNRH水平通道信噪比
//                            fread(&byte1, 1, m_RadialDatab.BinLength, File);
//                            if (byte1 < 5)
//                            {
//                                val = -32768;
//                                vData.push_back(val);
//                            }
//                            else
//                            {
//                                val = byte1;
//                                vData.push_back(val - m_RadialDatab.Offset);
//                            }
//                            break;
//                        case 10:  // RHODP差分相移
//                            fread(&byte2, 1, m_RadialDatab.BinLength, File);
//                            if (byte2 < 5)
//                            {
//                                val = -32768;
//                                vData.push_back(val);
//                            }
//                            else
//                            {
//                                val = byte2;
//                                vData.push_back(val - m_RadialDatab.Offset);
//                            }
//                            break;
//                        default:
//                            // fread(&byte1, 1, m_RadialDatab.BinLength, File);
//                            break;
//                    }
//                }
//
//                if (m_RadialDatab.DataType == 8 || m_RadialDatab.DataType == 9)
//                {
//                    //    continue;
//                }
//                int cType = -1;
//                // DBZ 0 RFE 1 V 2 W 3 ZDR 4 KDP 5 RHODP 6 PHIDP 7 SNR 8 LDR 9
//                switch (m_RadialDatab.DataType)
//                {
//                    case 1:  // dBT滤波前反射率 REF
//                        cType = 1;
//                        break;
//                    case 2:  // dBZ滤波后反射率
//                        cType = 0;
//                        break;
//                    case 3:  // V径向速度
//                        cType = 2;
//                        break;
//                    case 4:  // W谱宽
//                        cType = 3;
//                        break;
//                    case 7:  // ZDR差分反射率
//                        cType = 4;
//                        break;
//                    case 8:  // LDR退偏振比
//                        cType = 9;
//                        break;
//                    case 9:  // CC协相关系数
//                        cType = 7;
//                        break;
//                    case 10:  // RHODP差分相移
//                        cType = 6;
//                        break;
//                    case 11:  // KDP差分相移率
//                        cType = 5;
//                        break;
//                    case 16:  // SNRH水平通道信噪比
//                        cType = 8;
//                        break;
//                    default:
//                        break;
//                }
//                int Degree = m_RadioHeader.Azimuth + Rotate;
//                while (Degree < 0)
//                {
//                    Degree = Degree + 360;
//                }
//                Degree = Degree % 360;
//                if (cType >= 0 && cType < ELEMENTNUM)
//                {
//                    m_Data[l].Offset[cType] = m_RadialDatab.Offset;
//                    m_Data[l].Scale[cType] = m_RadialDatab.Scale;
//                    m_Data[l].rawdata[Degree][cType].swap(vData);
//                }
//            }
//        }
//        // break;
//    }
//    fclose(File);
//
//    for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
//    {
//        iter->ScanData();
//    }
//
//    return true;
//}

std::vector<double> HunanData::GetAllLayer()
{
    std::vector<double> vLayer;
    for (int i = 0; i < m_Header.ObservationInfo.CutNumber; i++)
    {
        vLayer.push_back(m_ScanCut[i].Elevation);
    }
    return vLayer;
}

double HunanData::GetLjVals(int Degree, int index, int Layer, double& dh, double& ddr)
{
    dh = ddr = 0;
    return 0;
    if (Layer < 0 || Layer >= m_Data.size() || index < 0 || index >= m_Data[Layer].rawdata[Degree][0].size())
    {
        return 0;
    }
    double val = 0, zh = 0, dbz = 0, kdp = 0, zdr = 0, Ah = 0, a1 = 0.247, a2 = 0.033, a11 = 1.37E-4, b11 = 0.779, a21 = 0.14, b21 = 1.13;
    long lval = 0;
    // DBZ 0 RFE 1 V 2 W 3 ZDR 4 KDP 5 RHODP 6 PHIDP 7 SNR 8 LDR 9
    for (int i = 0; i <= index; i++)
    {
        lval = m_Data[Layer].rawdata[Degree][0][i];
        if (lval <= -32768)
        {
            continue;
        }
        dbz = (lval - 0.0) / m_Data[Layer].Scale[0];
        lval = m_Data[Layer].rawdata[Degree][4][i];
        if (lval <= -32768)
        {
            zdr = 0;
        }
        zdr = (lval - 0.0) / m_Data[Layer].Scale[4];
        lval = m_Data[Layer].rawdata[Degree][5][i];
        if (lval <= -32768)
        {
            kdp = 0;
        }
        kdp = (lval - 0.0) / m_Data[Layer].Scale[5];

        if (kdp >= 0.1 && kdp <= 3)
        {
            dh = dh + a1 * kdp;
            ddr = ddr + a2 * dh;
        }
        else
        {
            zh = pow(10.0, dbz / 10.0);
            Ah = a11 * pow(zh, b11);
            dh = dh + Ah;
            ddr = ddr + a21 * pow(Ah, b21);
        }
    }
    return dh;
}

double HunanData::GetVal(int Degree, int cType, int index, int Layer)
{
    if (Layer < 0 || Layer >= m_Data.size() || index < 0 || index >= m_Data[Layer].rawdata[Degree][cType].size() || cType < 0 || cType >= ELEMENTNUM)
    {
        return -32768.0;
    }
    long lval = m_Data[Layer].rawdata[Degree][cType][index];
    if (lval <= -32768)
    {
        return -32768.0;
    }
    double val = (lval - 0.0) / m_Data[Layer].Scale[cType];
    return val;
}

double HunanData::GetVal(int Degree, int cType, int index)
{
    double val = -32768.0;
    if (index < 0 || cType < 0 || cType >= ELEMENTNUM)
    {
        return -32768.0;
    }
    std::vector<double> vlval;
    for (auto data : m_Data)
    {
        long lval = val = -32768;
        if (index >= data.rawdata[Degree][cType].size())
        {
            continue;
        }
        lval = data.rawdata[Degree][cType][index];
        if (lval > -32768)
        {
            val = (lval - 0.0) / data.Scale[cType];
        }
        vlval.push_back(val);
    }
    if (vlval.empty())
    {
        return -32768.0;
    }
    sort(vlval.begin(), vlval.end());
    index = vlval.size() / 2;
    val = vlval[index];

    return val;
}

// double HunanData::GetRain(int Degree, int index, int Layer, int Type)
//{
//     if (index < 0 || Layer < 0 || Layer >= m_Data.size())
//     {
//         return -1;
//     }
//     double dbz, kdp, zdr, dh, ddr, Rain = 0;
//     dbz = GetVal(Degree, 0, index, Layer);
//     zdr = GetVal(Degree, 4, index, Layer);
//     kdp = GetVal(Degree, 5, index, Layer);
//     // dh = GetLjVals(Degree, index, Layer, dh, ddr);
//     Rain = RainData::computeRainRate(dbz, zdr, kdp, Type);  // , dh, ddr);
//     return Rain;
// }

int HunanData::GetRainClass(double Rain)
{
    int ClassID = -1;
    if (Rain <= 0.5)
    {
        ClassID = 0;
    }
    else if (Rain <= 5)
    {
        ClassID = 1;
    }
    else if (Rain <= 10)
    {
        ClassID = 2;
    }
    else if (Rain <= 25)
    {
        ClassID = 3;
    }
    else if (Rain <= 50)
    {
        ClassID = 4;
    }
    else if (Rain <= 100)
    {
        ClassID = 5;
    }
    else if (Rain <= 250)
    {
        ClassID = 6;
    }
    else
    {
        ClassID = 7;
    }
    // ClassID -= 1;
    ClassID = Rain + 0.5;
    if (ClassID > 255)
    {
        ClassID = 255;
    }
    return ClassID;
}
// int HunanData::GetRainClass(int Degree, int index, int Layer, int Type)
//{
//     double Rain = GetRain(Degree, index, Layer, Type);
//     return GetRainClass(Rain);
// }

int HunanData::GetClass(int Degree, int cType, int index, int Layer)
{
    if (cType < 0 || cType >= ELEMENTNUM || Layer < 0 || Layer >= m_Data.size() || index < 0 || index >= m_Data[Layer].rawdata[Degree][cType].size())
    {
        return -1;
    }
    long lval = -32768;
    if (Layer < m_Data.size() && m_Data[Layer].rawdata[Degree][cType].size() > index)
    {
        lval = m_Data[Layer].rawdata[Degree][cType][index];
    }
    else
    {
        lval = GetVal(Degree, cType, index);
    }
    if (lval <= -32768)
    {
        return -1;
    }
    double MinVal = m_Data[Layer].MinVal[cType];
    double MaxVal = m_Data[Layer].MaxVal[cType];
    if (MaxVal <= MinVal)
    {
        return -1;
    }
    int Class = (lval - MinVal) * 255 / (MaxVal - MinVal) + 0.5;

    return Class;
}

// double HunanData::CalcAzimuthAngleFromGeo(double LGTD, double LTTD)
//{
//     double lon = m_Header.PerformanceInfo.longitude;
//     double lat = m_Header.PerformanceInfo.latitude;
//
//     return CRadarData::CalcAzimuthAngleFromGeo(lon, lat, LGTD, LTTD);
// }
//
// double HunanData::CalcDistance(double LGTD, double LTTD)
//{
//     double lon = m_Header.PerformanceInfo.longitude;
//     double lat = m_Header.PerformanceInfo.latitude;
//
//     return CRadarData::CalcDistance(lon, lat, LGTD, LTTD);
// }

// int HunanData::GetIndex(double LGTD, double LTTD)
//{
//     double dis = CalcDistance(LGTD, LTTD);
//     return GetIndex(dis);
// }

int HunanData::GetIndex(double dis)
{
    int index = dis / m_ScanCut[0].LogResolution + 0.5;
    return index;
}

void HunanData::GetSiteCoord(double& LGTD, double& LTTD)
{
    LGTD = m_Header.PerformanceInfo.longitude;
    LTTD = m_Header.PerformanceInfo.latitude;
}

bool HunanData::ProccessMerge(int ElementID, int* FinishedNum, void* pVoid)
{
    bzDualRVP9Record* pData = (bzDualRVP9Record*)pVoid;
    int val = -32768;
    int index = 0, cType = 0, MaxNum = 1000;

    std::vector<int> vlval;

    for (int i = 0; i < 360; i++)
    {
        // for (int j = 0; j < ELEMENTNUM; j++) {
        int j = ElementID;
        std::map<int, std::vector<int>> mapData;
        for (auto data : m_Data)
        {
            for (int k = 0; k < SU_MIN(MaxNum, data.rawdata[i][j].size()); k++)
            {  // data.rawdata[i][j].size(); k++) {
                val = data.rawdata[i][j][k];
                if (val > -32768)
                {
                    mapData[k].push_back(val);
                }
            }
            pData->Scale[j] = data.Scale[j];
            pData->Offset[j] = data.Offset[j];
        }

        pData->rawdata[i][j].resize(MaxNum, -32768);
        for (auto iter = mapData.begin(); iter != mapData.end(); iter++)
        {
            if (iter->second.empty())
            {
                val = -32768;
            }
            else
            {
                /*     sort(iter->second.begin(), iter->second.end());
                     index = min((ELEMENTNUM + 1) / 2, iter->second.size() - 1);
                     index = max(0, index);
                     val = iter->second[index];*/
                val = iter->second.front();
            }
            if (iter->first >= MaxNum)
            {
                continue;
            }
            pData->rawdata[i][j][iter->first] = val;
        }
        // }
    }
    // m_Mutex->lock();
    (*FinishedNum)++;
    // m_Mutex->unlock();
    return true;
}

int HunanData::Merge()
{
    bzDualRVP9Record RData;
    int FinishedNum = 0;

    for (int i = 0; i < ELEMENTNUM; i++)
    {
        std::thread theThread(&HunanData::ProccessMerge, this, i, &FinishedNum, &RData);
        theThread.detach();
    }

    while (FinishedNum < ELEMENTNUM)
    {
        //Sleep(100);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 毫秒
    }

    RData.ScanData();
    m_Data.push_back(RData);
    return m_Data.size() - 1;
}

int HunanData::MergeLayer()
{
    int val = -32768;
    int index = 0, cType = 0, MaxNum = 1000;

    std::vector<int> vlval;

    // for(int layer = 0, layer < m_Header.)

    // for (int i = 0; i < 360; i++) {
    //     //for (int j = 0; j < ELEMENTNUM; j++) {
    //     int j = ElementID;
    //     std::map<int, std::vector<int>> mapData;
    //     for (auto data : m_Data) {
    //         for (int k = 0; k < min(MaxNum, data.rawdata[i][j].size()); k++) {// data.rawdata[i][j].size(); k++) {
    //             val = data.rawdata[i][j][k];
    //             if (val > -32768) {
    //                 mapData[k].push_back(val);
    //             }
    //         }
    //         pData->Scale[j] = data.Scale[j];
    //         pData->Offset[j] = data.Offset[j];
    //     }

    //    pData->rawdata[i][j].resize(MaxNum, -32768);
    //    for (auto iter = mapData.begin(); iter != mapData.end(); iter++) {
    //        if (iter->second.empty()) {
    //            val = -32768;
    //        }
    //        else {
    //            /*     sort(iter->second.begin(), iter->second.end());
    //                 index = min((ELEMENTNUM + 1) / 2, iter->second.size() - 1);
    //                 index = max(0, index);
    //                 val = iter->second[index];*/
    //            val = iter->second.front();
    //        }
    //        if (iter->first >= MaxNum) {
    //            continue;
    //        }
    //        pData->rawdata[i][j][iter->first] = val;
    //    }
    //    // }
    //}
    // m_Mutex.lock();
    // (*FinishedNum)++;
    // m_Mutex.unlock();
    return true;
}