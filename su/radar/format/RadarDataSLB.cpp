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
#include "RadarDataSLB.h"
using namespace RadarData;

static void CalcReal(const std::vector<float>& src, std::vector<float>& ret, const int32_t& offset, const int32_t scale)
{
    ret.clear();
    ret.reserve(src.size());
    for (const auto& v : src)
    {
        // if (v == )
        ret.push_back((v - offset) / scale);
    }
}

bool FileVolume::Read(std::fstream& input)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(this), sizeof(FileVolume));
    return true;
}
bool SiteInfo::Read(std::fstream& input)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(this), sizeof(SiteInfo));
    return true;
}
bool ObserveTime::Read(std::fstream& input)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(this), sizeof(ObserveTime));
    return true;
}
bool OperationInfo::Read(std::fstream& input)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(this), sizeof(OperationInfo));
    return true;
}
bool AlertInfo::Read(std::fstream& input)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(this), sizeof(AlertInfo));
    return true;
}
bool Alerts::Read(std::fstream& input)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(&AlertCounts), sizeof(AlertCounts));
    input.read(reinterpret_cast<char*>(&Reserved), sizeof(Reserved));
    AlertList.resize(AlertCounts);
    for (int32_t i = 0; i < AlertCounts; i++)
    {
        assert(AlertList[i].Read(input));
    }
    return true;
}
bool LayerParam::Read(std::fstream& input)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(&Dbegin), sizeof(Dbegin));
    input.read(reinterpret_cast<char*>(&ElevationAngle), sizeof(ElevationAngle));
    input.read(reinterpret_cast<char*>(&MaxV), sizeof(MaxV));
    input.read(reinterpret_cast<char*>(&MinL), sizeof(MinL));
    input.read(reinterpret_cast<char*>(&VarCounts), sizeof(VarCounts));
    VarCode.resize(VarCounts);
    GateCounts.resize(VarCounts);
    input.read(reinterpret_cast<char*>(VarCode.data()), sizeof(int16_t) * VarCounts);
    input.read(reinterpret_cast<char*>(GateCounts.data()), sizeof(int16_t) * VarCounts);
    input.read(reinterpret_cast<char*>(&Reserved2), sizeof(Reserved2));

    return true;
}
bool BlockInfo::Read(std::fstream& input)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(&LayerCounts), sizeof(LayerCounts));
    input.read(reinterpret_cast<char*>(&RadialCounts), sizeof(RadialCounts));
    input.read(reinterpret_cast<char*>(&AngularResolution), sizeof(AngularResolution));
    input.read(reinterpret_cast<char*>(&DataWidth), sizeof(DataWidth));
    input.read(reinterpret_cast<char*>(&Reserved), sizeof(Reserved));
    LayerParams.resize(LayerCounts);
    for (int32_t i = 0; i < LayerCounts; i++)
    {
        assert(LayerParams[i].Read(input));
    }

    return true;
}
bool BlockData::Read(std::fstream& input, const BlockInfo& info)
{
    if (input.eof())
    {
        return false;
    }
    input.read(reinterpret_cast<char*>(&head), sizeof(head));
    if (head.VarCounts <= 0)
    {
        return false;
    }
    DataType.resize(head.VarCounts);
    Scale.resize(head.VarCounts);
    Offset.resize(head.VarCounts);
    input.read(reinterpret_cast<char*>(DataType.data()), sizeof(eDataType) * head.VarCounts);
    input.read(reinterpret_cast<char*>(Scale.data()), sizeof(int32_t) * head.VarCounts);
    input.read(reinterpret_cast<char*>(Offset.data()), sizeof(int32_t) * head.VarCounts);
    input.read(reinterpret_cast<char*>(Reserved), sizeof(Reserved));
    for (int i = 0; i < head.VarCounts; i++)
    {
        const int layer = head.ElevationNumber - 1;
        size_t count = info.LayerParams[layer].GateCounts[i];
        int32_t scale = Scale[i];
        int32_t offset = Offset[i];
        std::vector<float> tmp;
        tmp.reserve(count);
        switch (DataType[i])
        {
            case eDataType::BYTE:
            {
                std::vector<int8_t> data;
                data.resize(count);
                input.read(reinterpret_cast<char*>(data.data()), count);
                std::transform(data.begin(), data.end(), std::back_inserter(tmp), [](const int8_t& val) { return static_cast<float>(val); });
            }

            break;
            case eDataType::SHORT:
            {
                std::vector<int16_t> data;
                data.resize(count);
                input.read(reinterpret_cast<char*>(data.data()), count * sizeof(int16_t));
                std::transform(data.begin(), data.end(), std::back_inserter(tmp), [](const int16_t& val) { return static_cast<float>(val); });
            }
            break;
            case eDataType::USHORT:
            {
                std::vector<uint16_t> data;
                data.resize(count);
                input.read(reinterpret_cast<char*>(data.data()), count * sizeof(uint16_t));
                std::transform(data.begin(), data.end(), std::back_inserter(tmp), [](const uint16_t& val) { return static_cast<float>(val); });
            }
            break;
            case eDataType::UINT:
            {
                std::vector<uint32_t> data;
                data.resize(count);
                input.read(reinterpret_cast<char*>(data.data()), count * sizeof(uint32_t));
                std::transform(data.begin(), data.end(), std::back_inserter(tmp), [](const uint32_t& val) { return static_cast<float>(val); });
            }
            break;
            case eDataType::INT:
            {
                std::vector<int32_t> data;
                data.resize(count);
                input.read(reinterpret_cast<char*>(data.data()), count * sizeof(int32_t));
                std::transform(data.begin(), data.end(), std::back_inserter(tmp), [](const int32_t& val) { return static_cast<float>(val); });
            }
            break;
            case eDataType::FLOAT:
            {
                tmp.resize(count);
                input.read(reinterpret_cast<char*>(tmp.data()), count * sizeof(float));
            }
            break;
            case eDataType::LONG:
            {
                std::vector<long long> data;
                data.resize(count);
                input.read(reinterpret_cast<char*>(data.data()), count * sizeof(long long));
                std::transform(data.begin(), data.end(), std::back_inserter(tmp), [](const long long& val) { return static_cast<float>(val); });
            }
            break;
            default:
                break;
        }
        switch (TYPE_ORDER[i])
        {
            case eType::DBZ:
                CalcReal(tmp, _th, offset, scale);
                break;

            case eType::REF:
                CalcReal(tmp, _zh, offset, scale);
                break;
            case eType::VEL:
                CalcReal(tmp, _vel, offset, scale);
                break;
            case eType::WIDTH:
                CalcReal(tmp, _width, offset, scale);
                break;
            case eType::ZDR:
                CalcReal(tmp, _zdr, offset, scale);
                break;
            case eType::PHIDP:
                CalcReal(tmp, _phidp, offset, scale);
                break;
            case eType::KDP:
                CalcReal(tmp, _kdp, offset, scale);
                break;
            case eType::CC:
                CalcReal(tmp, _cc, offset, scale);
                break;
            case eType::TV:
                CalcReal(tmp, _tv, offset, scale);
                break;
            default:
                break;
        }
    }
    return true;
}
bool SLB::Read(const std::filesystem::path& file)
{
    std::fstream input(file, std::ios::binary | std::ios::in);
    if (!input.is_open())
    {
        return false;
    }
    Clear();
    assert(m_FileVol.Read(input));
    assert(m_SiteInfo.Read(input));
    assert(m_ObTime.Read(input));
    assert(m_OpInfo.Read(input));
    assert(m_Alerts.Read(input));
    assert(m_BlockInfo.Read(input));
    while (!input.eof())
    {
        BlockData tmp;
        if (tmp.Read(input, m_BlockInfo))
        {
            m_RadialBlocks.push_back(tmp);
        }
    }
    input.close();
    for (int i = 0; i < m_BlockInfo.LayerParams.size(); ++i)
    {
        m_Layer2Elevation[i] = m_BlockInfo.LayerParams[i].ElevationAngle;
    }
    m_RadialNumber0 = m_RadialBlocks.front().head.RadialNumber;
    return true;
}

static void MaxInRadial(std::vector<float>& ret, const std::vector<float>& src)
{
    int num = src.size();
    // ret.size()
    float* pr = ret.data();
    float* ps = (float*)src.data();
    int iI = 0;
    while (iI < num)
    {
        *pr = std::max(*pr, *ps);
        pr++;
        ps++;
        iI++;
    }
    return;
}
std::vector<std::vector<float>> SLB::GetData(const int& layer, const eType& type) const
{
    bool unionAll = layer == 9999;
    
    if (!unionAll)
    {
        if (layer >= m_BlockInfo.LayerParams.size())
        {
            return {};
        }
    }

    int typeIdx = 0;
    for (; typeIdx < TYPE_ORDER.size(); ++typeIdx)
    {
        if (TYPE_ORDER[typeIdx] == type)
        {
            break;
        }
    }

    int bankNum = m_BlockInfo.LayerParams[0].GateCounts[typeIdx];
    std::vector<std::vector<float>> ret(m_BlockInfo.RadialCounts, std::vector<float>(bankNum, -9999.0));
    for (const auto& block : m_RadialBlocks)
    {
        if ((block.head.ElevationNumber - 1) == layer || unionAll)
        {
            // int sort = std::round(block.head.Azimuth / m_BlockInfo.AngularResolution);
            int num = block.head.RadialNumber - m_RadialNumber0;
            if (num < 0)
            {
                num += m_BlockInfo.RadialCounts;
            }
            std::vector<float>& tmp = ret[num];
            switch (type)
            {
                case eType::DBZ:
                    MaxInRadial(tmp, block._th);
                    break;

                case eType::REF:
                    MaxInRadial(tmp, block._th);
                    break;
                case eType::VEL:
                    MaxInRadial(tmp, block._vel);
                    break;
                case eType::WIDTH:
                    MaxInRadial(tmp, block._width);
                    break;
                case eType::ZDR:
                    MaxInRadial(tmp, block._zdr);
                    break;
                case eType::PHIDP:
                    MaxInRadial(tmp, block._phidp);
                    break;
                case eType::KDP:
                    MaxInRadial(tmp, block._kdp);
                    break;
                case eType::CC:
                    MaxInRadial(tmp, block._cc);
                    break;
                case eType::TV:
                    MaxInRadial(tmp, block._tv);
                    break;
                default:
                    break;
            }
        }
    }

    return ret;
}

SiteInfo SLB::GetSiteInfo() const
{
    return m_SiteInfo;
}

std::map<int, double> SLB::Layer2Elevation() const
{
    return m_Layer2Elevation;
}

void SLB::Clear()
{  // 重置
    m_FileVol = FileVolume();
    m_SiteInfo = SiteInfo();
    m_ObTime = ObserveTime();
    m_OpInfo = OperationInfo();
    m_Alerts = Alerts();
    m_BlockInfo = BlockInfo();
    m_RadialBlocks.clear();
    m_Layer2Elevation.clear();
}