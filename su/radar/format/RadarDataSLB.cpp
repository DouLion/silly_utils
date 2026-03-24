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
#include <compress/silly_bz2.h>
using namespace RadarData;

template <typename T>
static void CalcVal(const std::vector<T>& src, float* ret, const int32_t& offset, const float scale)
{
    float* fp = ret;
    for (const T& v : src)
    {
        if (v == 1 || v == 4)
        {
            *fp = -9999;
        }

        else
        {
            // 强制赋值,使char能够正常下溢
            T t = v - offset;
            *fp = t / scale;
        }
        fp++;
    }
}

bool FileVolume::Read(char*& p)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(this), p, sizeof(FileVolume));
    p += sizeof(FileVolume);
    return true;
}
bool SiteInfo::Read(char*& p)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(this), p, sizeof(SiteInfo));
    p += sizeof(SiteInfo);
    return true;
}
bool ObserveTime::Read(char*& p)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(this), p, sizeof(ObserveTime));
    p += sizeof(ObserveTime);
    return true;
}
bool OperationInfo::Read(char*& p)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(this), p, sizeof(OperationInfo));
    p += sizeof(OperationInfo);
    return true;
}
bool AlertInfo::Read(char*& p)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(this), p, sizeof(AlertInfo));
    p += sizeof(AlertInfo);
    return true;
}
bool Alerts::Read(char*& p)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(&AlertCounts), p, sizeof(AlertCounts));
    p += sizeof(AlertCounts);
    std::memcpy(reinterpret_cast<char*>(&Reserved), p, sizeof(Reserved));
    p += sizeof(Reserved);
    AlertList.resize(AlertCounts);
    for (int32_t i = 0; i < AlertCounts; i++)
    {
        bool ret = AlertList[i].Read(p);
        assert(ret);
        if (!ret)
        {
            return false;
        }
    }
    return true;
}
bool LayerParam::Read(char*& p)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(&Dbegin), p, sizeof(Dbegin));
    p += sizeof(Dbegin);
    std::memcpy(reinterpret_cast<char*>(&ElevationAngle), p, sizeof(ElevationAngle));
    p += sizeof(ElevationAngle);
    std::memcpy(reinterpret_cast<char*>(&MaxV), p, sizeof(MaxV));
    p += sizeof(MaxV);
    std::memcpy(reinterpret_cast<char*>(&MinL), p, sizeof(MinL));
    p += sizeof(MinL);
    std::memcpy(reinterpret_cast<char*>(&VarCounts), p, sizeof(VarCounts));
    p += sizeof(VarCounts);
    VarCode.resize(VarCounts);
    GateCounts.resize(VarCounts);
    std::memcpy(reinterpret_cast<char*>(VarCode.data()), p, sizeof(int16_t) * VarCounts);
    p += sizeof(int16_t) * VarCounts;
    std::memcpy(reinterpret_cast<char*>(GateCounts.data()), p, sizeof(int16_t) * VarCounts);
    p += sizeof(int16_t) * VarCounts;
    std::memcpy(reinterpret_cast<char*>(&Reserved2), p, sizeof(Reserved2));
    p += sizeof(Reserved2);

    return true;
}

bool BlockInfo::Read(char*& p)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(&LayerCounts), p, sizeof(LayerCounts));
    p += sizeof(LayerCounts);
    std::memcpy(reinterpret_cast<char*>(&RadialCounts), p, sizeof(RadialCounts));
    p += sizeof(RadialCounts);
    std::memcpy(reinterpret_cast<char*>(&AngularResolution), p, sizeof(AngularResolution));
    p += sizeof(AngularResolution);
    std::memcpy(reinterpret_cast<char*>(&DataWidth), p, sizeof(DataWidth));
    p += sizeof(DataWidth);
    std::memcpy(reinterpret_cast<char*>(&Reserved), p, sizeof(Reserved));
    p += sizeof(Reserved);
    LayerParams.resize(LayerCounts);
    for (int32_t i = 0; i < LayerCounts; i++)
    {
        bool ret = LayerParams[i].Read(p);
        assert(ret);
        if (!ret)
        {
            return false;
        }
    }

    return true;
}
bool BlockData::Read(char*& p, const BlockInfo& info, SLB& slb, size_t azNum, size_t gateNum)
{
    if (!p)
    {
        return false;
    }
    std::memcpy(reinterpret_cast<char*>(&head), p, sizeof(head));
    if (head.VarCounts <= 0 || head.VarCounts > 100)
    {
        return false;
    }
    p += sizeof(head);
    DataType.resize(head.VarCounts);
    Scale.resize(head.VarCounts);
    Offset.resize(head.VarCounts);
    std::memcpy(reinterpret_cast<char*>(DataType.data()), p, sizeof(eDataType) * head.VarCounts);
    p += sizeof(eDataType) * head.VarCounts;
    std::memcpy(reinterpret_cast<char*>(Scale.data()), p, sizeof(int32_t) * head.VarCounts);
    p += sizeof(int32_t) * head.VarCounts;
    std::memcpy(reinterpret_cast<char*>(Offset.data()), p, sizeof(int32_t) * head.VarCounts);
    p += sizeof(int32_t) * head.VarCounts;
    std::memcpy(reinterpret_cast<char*>(Reserved), p, sizeof(Reserved));
    p += sizeof(Reserved);

    for (int i = 0; i < head.VarCounts; i++)
    {
        const int layer = head.ElevationNumber - 1;
        const int az = head.RadialNumber - 1;
        size_t count = info.LayerParams[layer].GateCounts[i];
        if (count != gateNum)
        {
            return false;
        }
        size_t npoff = layer * azNum * gateNum + az * gateNum;
        float* SetP = slb.GetFP(TYPE_ORDER[i], npoff);
        if (!SetP)
        {
            return false;
        }
        int32_t scale = Scale[i];
        int32_t offset = Offset[i];
        eDataType validType = eDataType::INVALID;
        switch (DataType[i])
        {
            case eDataType::BYTE:
            {
                std::vector<uint8_t> data;
                data.resize(count);
                std::memcpy(reinterpret_cast<char*>(data.data()), p, count);
                p += count * sizeof(uint8_t);
                CalcVal(data, SetP, offset, scale);
            }

            break;
            case eDataType::SHORT:
            {
                std::vector<int16_t> data;
                data.resize(count);
                std::memcpy(reinterpret_cast<char*>(data.data()), p, count * sizeof(int16_t));
                p += count * sizeof(int16_t);
                CalcVal(data, SetP, offset, scale);
            }
            break;
            case eDataType::USHORT:
            {
                std::vector<uint16_t> data;
                data.resize(count);
                std::memcpy(reinterpret_cast<char*>(data.data()), p, count * sizeof(uint16_t));
                p += count * sizeof(uint16_t);
                CalcVal(data, SetP, offset, scale);
            }
            break;
            case eDataType::UINT:
            {
                std::vector<uint32_t> data;
                data.resize(count);
                std::memcpy(reinterpret_cast<char*>(data.data()), p, count * sizeof(uint32_t));
                p += count * sizeof(uint32_t);
                CalcVal(data, SetP, offset, scale);
            }
            break;
            case eDataType::INT:
            {
                std::vector<int32_t> data;
                data.resize(count);
                std::memcpy(reinterpret_cast<char*>(data.data()), p, count * sizeof(int32_t));
                p += count * sizeof(int32_t);
                CalcVal(data, SetP, offset, scale);
            }
            break;
            case eDataType::FLOAT:
            {
                std::memcpy(reinterpret_cast<char*>(SetP), p, count * sizeof(float));
                p += count * sizeof(float);
                for (int nii = 0; nii < count; ++nii)
                {
                    SetP[nii] = (SetP[nii] - offset) / scale;
                }
            }
            break;
            case eDataType::LONG:
            {
                std::vector<long long> data;
                data.resize(count);
                std::memcpy(reinterpret_cast<char*>(data.data()), p, count * sizeof(long long));
                p += count * sizeof(long long);
                CalcVal(data, SetP, offset, scale);
            }
            break;
            default:
                break;
        }
    }

    return true;
}
bool SLB::Read(const suPath& file)
{
    size_t size = file.file_size();
    std::string content = sufile::read(file);
    if (!(size > 8 && content.size() == size))
    {
        return false;
    }
    char* bp = content.data();
    std::string bz2dp;
    if (suBz2::valid(content))
    {
        if (eCompressErr::Ok != suBz2::decompress(content, bz2dp))
        {
            return false;
        }
        bp = bz2dp.data();
        size = bz2dp.size();
    }
    char* p = bp;
    Clear();
    do
    {
        if (!m_FileVol.Read(p))
        {
            break;
        }
        if (m_FileVol.FileLength != size)
        {
            break;
        }
        if (!m_SiteInfo.Read(p))
        {
            break;
        }
        if (!m_ObTime.Read(p))
        {
            break;
        }
        if (!m_OpInfo.Read(p))
        {
            break;
        }
        if (!m_Alerts.Read(p))
        {
            break;
        }
        if (!m_BlockInfo.Read(p))
        {
            break;
        }
        const int layerNum = m_BlockInfo.LayerCounts;                            // 层数
        const int azNum = m_BlockInfo.RadialCounts;                              // 方向数
        const int gateNum = m_BlockInfo.LayerParams.front().GateCounts.front();  // 每个径向的 gate 数
        {
            _th.resize(layerNum * azNum * gateNum);
            _zh.resize(layerNum * azNum * gateNum);
            _vel.resize(layerNum * azNum * gateNum);
            _width.resize(layerNum * azNum * gateNum);
            _zdr.resize(layerNum * azNum * gateNum);
            _phidp.resize(layerNum * azNum * gateNum);
            _kdp.resize(layerNum * azNum * gateNum);
            _cc.resize(layerNum * azNum * gateNum);
            _tv.resize(layerNum * azNum * gateNum);
        }
        int count = 0;
        while (p - bp < size)
        {
            count++;
            BlockData tmp;
            if (tmp.Read(p, m_BlockInfo, *this, azNum, gateNum))
            {
                m_RadialBlocks.push_back(tmp);
            }
            else
            {
                return false;
            }
        }
        for (int i = 0; i < m_BlockInfo.LayerParams.size(); ++i)
        {
            m_Layer2Elevation[i] = m_BlockInfo.LayerParams[i].ElevationAngle;
        }
        m_RadialNumber0 = m_RadialBlocks.front().head.RadialNumber;
    } while (0);
    return !m_RadialBlocks.empty();
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
}
std::vector<float> SLB::GetData(const int& layer, const eType& type) const
{
    // layer == 9999 合并所有层级的最大值
    // 或者获取指定层级的极坐标数据
    bool mergeMax = layer == 9999;
    const int layerNum = m_BlockInfo.LayerCounts;                            // 层数
    const int azNum = m_BlockInfo.RadialCounts;                              // 方向数
    const int gateNum = m_BlockInfo.LayerParams.front().GateCounts.front();  // 每个径向的 gate 数
    std::vector<float> ret(azNum * gateNum, -9999);
    ret.resize(azNum * gateNum);
    // 1. 确定要访问的数据源容器
    if (!(layer >= 0 && layer < layerNum) && !mergeMax)
    {
        return ret;
    }
    const float* dataSource = nullptr;

    switch (type)
    {
        case eType::DBZ:
            dataSource = _zh.data();

            break;
        case eType::REF:
            dataSource = _th.data();
            break;
        case eType::VEL:
            dataSource = _vel.data();
            break;
        case eType::WIDTH:
            dataSource = _width.data();
            break;
        case eType::ZDR:
            dataSource = _zdr.data();
            break;
        case eType::PHIDP:
            dataSource = _phidp.data();
            break;
        case eType::KDP:
            dataSource = _kdp.data();
            break;
        case eType::CC:
            dataSource = _cc.data();
            break;
        case eType::TV:
            dataSource = _tv.data();
            break;
        default:
            return ret;  // 未知类型，返回空的 PolorGrid
    }
    // 2. 检查数据源是否有效

    // 3. 处理普通单层请求
    const size_t layerCount = azNum * gateNum;
    if (!mergeMax)
    {
        size_t off = layer * layerCount;
        std::memcpy(ret.data(), dataSource + off, sizeof(float) * layerCount);
    }
    // 4. 处理合并所有层（layer == 9999）
    else
    {
        for (int l = 0; l < layerNum; ++l)
        {
            size_t off = l * layerCount;
            float* tp = (float*)dataSource + off;
            for (auto& v : ret)
            {
                v = std::max(v, *tp);
                tp++;
            }
        }
    }

    return ret;
}

SiteInfo SLB::GetSiteInfo() const
{
    return m_SiteInfo;
}

BlockInfo SLB::GetBlockInfo() const
{
    return m_BlockInfo;
}

ObserveTime SLB::GetObserveTime() const
{
    return m_ObTime;
}

std::map<int, double> SLB::Layer2Elevation() const
{
    return m_Layer2Elevation;
}

static constexpr double EARTH_RADIUS_M = 6371000.0;
static constexpr double K_FACTOR = 4.0 / 3.0;
static constexpr double DEG_TO_RAD = M_PI / 180.0;
static constexpr double RAD_TO_DEG = 180.0 / M_PI;

std::optional<std::pair<size_t, size_t>> RadarData::SLB::CalcAG(float lon, float lat, float targetAlt) const
{
    double lat1 = static_cast<double>(m_SiteInfo.Latitude * 10000.0) * DEG_TO_RAD;
    double lon1 = static_cast<double>(m_SiteInfo.Longitude * 10000.0) * DEG_TO_RAD;
    double lat2 = static_cast<double>(lat) * DEG_TO_RAD;
    double lon2 = static_cast<double>(lon) * DEG_TO_RAD;

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    // 1. 使用 Haversine 公式计算高精度的地面大圆距离 (Ground Range)
    double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0) + std::cos(lat1) * std::cos(lat2) * std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
    a = std::clamp(a, 0.0, 1.0);  // 防止浮点数误差导致越界
    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    double groundDist = EARTH_RADIUS_M * c;

    // 2. 计算精确的正向方位角 (True Bearing)
    double y = std::sin(dLon) * std::cos(lat2);
    double x = std::cos(lat1) * std::sin(lat2) - std::sin(lat1) * std::cos(lat2) * std::cos(dLon);
    double azRad = std::atan2(y, x);

    if (azRad < 0.0)
    {
        azRad += 2.0 * M_PI;
    }

    // 3. 计算方位角索引 (支持非 360 的任意方位角数量)
    double azDeg = azRad * RAD_TO_DEG;
    // 例如 AzNum = 360, 每一度一个; AzNum = 720, 每 0.5 度一个
    double azRes = 360.0 / static_cast<double>(m_BlockInfo.RadialCounts);
    size_t azIndex = static_cast<size_t>(std::round(azDeg / azRes));

    if (azIndex >= m_BlockInfo.RadialCounts)
    {
        azIndex = 0;  // 360度回绕到 0
    }

    // 4. 计算斜距 (Slant Range) - 改进的数值稳定算法
    double R_eff = EARTH_RADIUS_M * K_FACTOR;
    double r_radar = R_eff + static_cast<double>(m_SiteInfo.Height);
    double r_target = R_eff + static_cast<double>(targetAlt);

    double theta = groundDist / R_eff;

    // 稳定的余弦定理变体，避免大数相减导致的精度丢失 (Catastrophic Cancellation)
    double delta_r = r_target - r_radar;
    double sinHalfTheta = std::sin(theta / 2.0);

    double slantRangeSq = delta_r * delta_r + 4.0 * r_radar * r_target * sinHalfTheta * sinHalfTheta;

    double slantRange = std::sqrt(slantRangeSq);

    // 5. 计算距离门索引 - 应该用向下取整 (Floor/截断) 而非 Round
    // 因为每个距离门是一个区间：Gate 0 是 [0, GateLen)
    size_t gateIndex = static_cast<size_t>(slantRange / static_cast<double>(m_BlockInfo.DataWidth));

    // 6. 边界检查
    if (gateIndex >= 1000.0)
    {
        return std::nullopt;  // 超出雷达探测范围
    }

    return std::make_pair(azIndex, gateIndex);
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

float* SLB::GetFP(const eType& tp, size_t npoff)
{
    float* SetP = nullptr;

    switch (tp)
    {
        case eType::DBZ:
            SetP = &_zh[npoff];
            break;
        case eType::REF:
            SetP = &_th[npoff];
            break;
        case eType::VEL:
            SetP = &_vel[npoff];
            break;
        case eType::WIDTH:
            SetP = &_width[npoff];
            break;
        case eType::ZDR:
            SetP = &_zdr[npoff];
            break;
        case eType::PHIDP:
            SetP = &_phidp[npoff];
            break;
        case eType::KDP:
            SetP = &_kdp[npoff];
            break;
        case eType::CC:
            SetP = &_cc[npoff];
            break;
        case eType::TV:
            SetP = &_tv[npoff];
            break;
        default:
            break;
    }
    return SetP;
}