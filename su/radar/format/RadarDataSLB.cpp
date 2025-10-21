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
bool BlockData::Read(std::fstream& input, const BlockInfo& info, SLB& slb)
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
        const int az = head.RadialNumber - 1;
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
                CalcReal(tmp, slb._th[layer][az], offset, scale);
                break;

            case eType::REF:
                CalcReal(tmp, slb._zh[layer][az], offset, scale);
                break;
            case eType::VEL:
                CalcReal(tmp, slb._vel[layer][az], offset, scale);
                break;
            case eType::WIDTH:
                CalcReal(tmp, slb._width[layer][az], offset, scale);
                break;
            case eType::ZDR:
                CalcReal(tmp, slb._zdr[layer][az], offset, scale);
                break;
            case eType::PHIDP:
                CalcReal(tmp, slb._phidp[layer][az], offset, scale);
                break;
            case eType::KDP:
                CalcReal(tmp, slb._kdp[layer][az], offset, scale);
                break;
            case eType::CC:
                CalcReal(tmp, slb._cc[layer][az], offset, scale);
                break;
            case eType::TV:
                CalcReal(tmp, slb._tv[layer][az], offset, scale);
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
    {
        const int layerNum = m_BlockInfo.LayerCounts; //层数
        const int azNum = m_BlockInfo.RadialCounts; // 方向数
        const int gateNum = m_BlockInfo.LayerParams.front().GateCounts.front();  // 每个径向的 gate 数
        _th.resize(layerNum);
        for (auto& layer : _th)
        {
            layer.resize(azNum);
            for (auto& radal : layer)
            {
                radal = std::vector<float>(gateNum, -9999);
            }
        }
        _zh = _th;
        _vel = _th;
        _width = _th;
        _zdr = _th;
        _phidp = _th;
        _kdp = _th;
        _cc = _th;
        _tv = _th;       

    }
    while (!input.eof())
    {
        BlockData tmp;
        if (tmp.Read(input, m_BlockInfo, *this))
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
PolorGrid SLB::GetData(const int& layer, const eType& type) const
{
    // layer == 9999 合并所有层级的最大值
    // 或者获取指定层级的极坐标数据
    bool mergeMax = layer == 9999;
    // 1. 确定要访问的数据源容器
    const std::vector<PolorGrid>* dataSource = nullptr;

    switch (type)
    {
        case eType::DBZ:
            dataSource = &_th;
            break;
        case eType::REF:
            dataSource = &_zh;
            break;
        case eType::VEL:
            dataSource = &_vel;
            break;
        case eType::WIDTH:
            dataSource = &_width;
            break;
        case eType::ZDR:
            dataSource = &_zdr;
            break;
        case eType::PHIDP:
            dataSource = &_phidp;
            break;
        case eType::KDP:
            dataSource = &_kdp;
            break;
        case eType::CC:
            dataSource = &_cc;
            break;
        case eType::TV:
            dataSource = &_tv;
            break;
        default:
            return {};  // 未知类型，返回空的 PolorGrid
    }
    // 2. 检查数据源是否有效
    if (dataSource == nullptr || dataSource->empty())
        return {};

    // 3. 处理普通单层请求
    if (!mergeMax)
    {
        int l = layer;

        if (l < 0 || l >= static_cast<int>(dataSource->size()))
            return {};  // 层号不合法

        return (*dataSource)[l];  // 返回该层数据（PolorGrid = vector<vector<float>>）
    }
    // 4. 处理合并所有层（layer == 9999）
    else
    {
        int numLayers = static_cast<int>(dataSource->size());

        if (numLayers == 0)
            return {};

        // 假设所有 PolorGrids（每层数据）的大小完全一致，取第一个作为模板
        const PolorGrid& firstGrid = (*dataSource)[0];
        if (firstGrid.empty() || firstGrid[0].empty())
            return {};

        size_t rows = firstGrid.size();
        size_t cols = firstGrid[0].size();

        // 构造返回的合并结果网格，初始化为最小值（或根据需求）
        PolorGrid mergedGrid(rows, std::vector<float>(cols, -std::numeric_limits<float>::infinity()));

        // 遍历所有层，逐像素取最大值
        for (int i = 0; i < numLayers; ++i)
        {
            const PolorGrid& grid = (*dataSource)[i];

            for (size_t r = 0; r < rows; ++r)
            {
                for (size_t c = 0; c < cols; ++c)
                {
                    if (r < grid.size() && c < grid[r].size())
                    {
                        mergedGrid[r][c] = std::max(mergedGrid[r][c], grid[r][c]);
                    }
                }
            }
        }

        return mergedGrid;
    }
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