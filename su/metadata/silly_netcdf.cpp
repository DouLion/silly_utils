/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 24-8-9 下午6:58
 * @version: 1.0.1
 * @description: silly_netcdf 类实现
 */
#include "silly_netcdf.h"
#if SU_THIRD_SUPPORT_NETCDF_CXX
#include <netcdf.h>
/*
 enum ncType
   {
     nc_BYTE     = NC_BYTE, 	//!< signed 1 byte integer
     nc_CHAR     = NC_CHAR,	//!< ISO/ASCII character
     nc_SHORT    = NC_SHORT, 	//!< signed 2 byte integer
     nc_INT      = NC_INT,	//!< signed 4 byte integer
     nc_FLOAT    = NC_FLOAT, 	//!< single precision floating point number
     nc_DOUBLE   = NC_DOUBLE, 	//!< float precision floating point number
     nc_UBYTE    = NC_UBYTE,	//!< unsigned 1 byte int
     nc_USHORT   = NC_USHORT,	//!< unsigned 2-byte int
     nc_UINT     = NC_UINT,	//!< unsigned 4-byte int
     nc_INT64    = NC_INT64,	//!< signed 8-byte int
     nc_UINT64   = NC_UINT64,	//!< unsigned 8-byte int
     nc_STRING   = NC_STRING, 	//!< string
     nc_VLEN     = NC_VLEN,   	//!< "NcVlen type"
     nc_OPAQUE   = NC_OPAQUE, 	//!< "NcOpaque type"
     nc_ENUM     = NC_ENUM, 	//!< "NcEnum type"
     nc_COMPOUND = NC_COMPOUND //!< "NcCompound type"
   };
 * */
std::vector<std::string> read_band_names(netCDF::NcVar nv, std::string name, size_t len)
{
    std::vector<std::string> result;
    if (1 == len)
    {
        result.push_back(name);
        return result;
    }
    std::vector<char*> v_str;
    std::vector<unsigned char> v_uc;
    std::vector<char> v_c;
    std::vector<short> v_s;
    std::vector<int> v_i;
    std::vector<long long> v_ll;
    std::vector<unsigned short> v_us;
    std::vector<unsigned int> v_ui;
    std::vector<unsigned long long> v_ull;
    std::vector<float> v_f;
    std::vector<float> v_d;

    switch (nv.getType().getId())
    {
        case netCDF::NcType::nc_BYTE:
        case netCDF::NcType::nc_CHAR:
            v_c.resize(len);
            nv.getVar(&v_c[0]);
            for (auto& c : v_c)
            {
                result.push_back(SUFMT("{}:{}", name, c));
            }
            break;
        case netCDF::NcType::nc_SHORT:
            v_s.resize(len);
            nv.getVar(&v_s[0]);
            for (auto& s : v_s)
            {
                result.push_back(SUFMT("{}:{}", name, s));
            }
            break;
        case netCDF::NcType::nc_INT:
            v_i.resize(len);
            nv.getVar(&v_i[0]);
            for (auto& i : v_i)
            {
                result.push_back(SUFMT("{}:{}", name, i));
            }
            break;
        case netCDF::NcType::nc_INT64:
            v_ll.resize(len);
            nv.getVar(&v_ll[0]);
            for (auto& ll : v_ll)
            {
                result.push_back(SUFMT("{}:{}", name, ll));
            }
            break;
        case netCDF::NcType::nc_UBYTE:
            v_uc.resize(len);
            nv.getVar(&v_uc[0]);
            for (auto& uc : v_uc)
            {
                result.push_back(SUFMT("{}:{}", name, uc));
            }
            break;
        case netCDF::NcType::nc_USHORT:
            v_us.resize(len);
            nv.getVar(&v_us[0]);
            for (auto& us : v_us)
            {
                result.push_back(SUFMT("{}:{}", name, us));
            }
            break;
        case netCDF::NcType::nc_UINT:
            v_ui.resize(len);
            nv.getVar(&v_ui[0]);
            for (auto& ui : v_ui)
            {
                result.push_back(SUFMT("{}:{}", name, ui));
            }
            break;
        case netCDF::NcType::nc_UINT64:
            v_ull.resize(len);
            nv.getVar(&v_ull[0]);
            for (auto& ull : v_ull)
            {
                result.push_back(SUFMT("{}:{}", name, ull));
            }
            break;
        case netCDF::NcType::nc_FLOAT:
            v_f.resize(len);
            nv.getVar(&v_f[0]);
            for (auto& f : v_f)
            {
                result.push_back(SUFMT("{}:{}", name, f));
            }
            break;
        case netCDF::NcType::nc_DOUBLE:
            v_d.resize(len);
            nv.getVar(&v_d[0]);
            for (auto& d : v_d)
            {
                result.push_back(SUFMT("{}:{}", name, d));
            }
            break;
        case netCDF::NcType::nc_STRING:
            v_str.resize(len);
            nv.getVar(&v_str[0]);
            for (auto& str : v_str)
            {
                result.push_back(SUFMT("{}:{}", name, str));
            }
            break;
        default:
            break;
    }

    return result;
}
#endif

bool suNetCDF::open(const suPath& file)
{
    bool status = false;
#if SU_THIRD_SUPPORT_NETCDF_CXX
    auto fp = suPath(file);
    try
    {
        m_nc_file.open(fp.string(), netCDF::NcFile::read);
        status = true;
    }
    catch (netCDF::exceptions::NcException& e)
    {
        std::cout << e.what() << std::endl;
        return status;
    }
#endif
    return status;
}
std::vector<std::string> suNetCDF::members()
{
    std::vector<std::string> result;
#if SU_THIRD_SUPPORT_NETCDF_CXX
    m_nc_all_grps = m_nc_file.getGroup("/", netCDF::NcGroup::GroupLocation::AllGrps);
    auto nc_all_vars = m_nc_all_grps.getVars(netCDF::NcGroup::Current);
    for (auto& [k, var] : nc_all_vars)
    {
        std::cout << k << ": " << var.getDimCount() << std::endl;
        if (var.getDimCount() > 1)
        {
            result.push_back(k);
        }
    }
#endif
    return result;
}
bool suNetCDF::geometry(const std::string& lon, const std::string& lat)
{
    /*  netCDF::NcVar lon_var = m_nc_all_grps.getVar(lon);
      netCDF::NcVar lat_var = m_nc_all_grps.getVar(lat);
      lon_var.getDimCount();*/
    return false;
}
void suNetCDF::close()
{
#if SU_THIRD_SUPPORT_NETCDF_CXX
    m_nc_file.close();
#endif
}

std::vector<float> suNetCDF::read1d(const std::string& group)
{
    std::vector<float> ret;
#if SU_THIRD_SUPPORT_NETCDF_CXX
    m_nc_all_grps = m_nc_file.getGroup("/", netCDF::NcGroup::GroupLocation::AllGrps);
    netCDF::NcVar nv_dst = m_nc_all_grps.getVar(group);
    if (nv_dst.isNull())
    {
        return ret;
    }
    std::vector<netCDF::NcDim> ndims = nv_dst.getDims();
    std::vector<std::tuple<std::string, size_t, netCDF::NcVar>> name_nvdims;
    size_t total_val_size = 1;
    for (const auto& ndim : ndims)
    {
        std::string name = ndim.getName();
        if (ndim.getSize() == 1)
        {
            continue;
        }
        SLOG_DEBUG(name)
        total_val_size *= ndim.getSize();
    }

    std::map<std::string, netCDF::NcVarAtt> attr_vars = nv_dst.getAtts();
    for (const auto& [key, attr] : attr_vars)
    {
        m_attr_names.push_back(key);
        if (key == "_FillValue")
        {
            attr.getValues(&m_fill);
        }
        else if (key == "add_offset")
        {
            attr.getValues(&m_offset);
        }
        else if (key == "scale_factor")
        {
            attr.getValues(&m_scale);
        }
    }

    ret.resize(total_val_size);

    nv_dst.getVar(ret.data());
    if (std::abs(m_scale - 1.0) > 1e-6)
    {
        for (auto& v : ret)
        {
            v *= m_scale;
        }
    }
    if (std::abs(m_offset) > 1e-6)
    {
        for (auto& v : ret)
        {
            v += m_offset;
        }
        
    }

#endif
    return ret;
}
bool suNetCDF::read(const std::string& group, const std::string& lon, const std::string& lat)
{
    bool status = false;
    m_bands.clear();
    m_dem_names.clear();
    m_attr_names.clear();
    m_geo = Geo();
#if SU_THIRD_SUPPORT_NETCDF_CXX
    m_nc_all_grps = m_nc_file.getGroup("/", netCDF::NcGroup::GroupLocation::AllGrps);
    netCDF::NcVar nv_dst = m_nc_all_grps.getVar(group);
    if (nv_dst.isNull())
    {
        return status;
    }
    /*维度顺序：
        NetCDF 中的多维数组是按照维度顺序存储的，典型的顺序是从“外到内”。
    例如，一个三维数组的维度顺序为 (time, latitude, longitude)，
    则在存储数据时，time 维度的变化最先影响数据的读取。
        对于多维数据集，使用 C 语言的存储顺序（列优先，即最后一个维度最先变化）
    或 Fortran 的行优先（行优先，即第一个维度最先变化），
    实际行为取决于你创建数据时使用的 API。
     */
    std::vector<netCDF::NcDim> ndims = nv_dst.getDims();
    std::vector<std::tuple<std::string, size_t, netCDF::NcVar>> name_nvdims;
    std::vector<float> lat_data, lon_data, val_data;
    size_t total_val_size = 1;
    for (const auto& ndim : ndims)
    {
        std::string name = ndim.getName();
        if (ndim.getSize() == 1)
        {
            continue;
        }
        m_dem_names.push_back(name);
        SLOG_DEBUG(name)
        if (lon == name)
        {
            m_geo.xlen = ndim.getSize();
            lon_data.resize(m_geo.xlen);
        }
        else if (lat == name)
        {
            m_geo.ylen = ndim.getSize();
            lat_data.resize(m_geo.ylen);
        }
        total_val_size *= ndim.getSize();
        name_nvdims.emplace_back(name, ndim.getSize(), m_nc_all_grps.getVar(ndim.getName()));
    }

    if (name_nvdims.size() < 2)
    {
        throw std::runtime_error("至少需要两个维度");
    }

    auto riter = name_nvdims.end() - 1;
    if (lon != std::get<0>(*riter))
    {
        throw std::runtime_error("lon维度必须在倒数第一个");
    }
    std::get<2>(*riter).getVar(&lon_data[0]);
    riter--;
    if (lat != std::get<0>(*riter))
    {
        throw std::runtime_error("lat维度必须在倒数第二个");
    }
    std::get<2>(*riter).getVar(&lat_data[0]);
    // 判断矢量方向
    m_geo.xmin = std::min(lon_data.back(), lon_data.front());
    m_geo.xmax = std::max(lon_data.back(), lon_data.front());
    m_geo.ymin = std::min(lat_data.back(), lat_data.front());
    m_geo.ymax = std::max(lat_data.back(), lat_data.front());
    m_geo.xstep = (m_geo.xmax - m_geo.xmin) / (m_geo.xlen - 1);
    m_geo.ystep = (m_geo.ymax - m_geo.ymin) / (m_geo.ylen - 1);
    if (lat_data.front() < lat_data.back())  // 纬度由小到大
    {
        m_sort = 0;
        SLOG_DEBUG("从南往北")
    }
    else
    {
        m_sort = 1;  // 本库中所有网格存储顺序皆为 从左往右 从上往下
        SLOG_DEBUG("从北往南")
    }

    std::get<2>(*riter).getVar(&lat_data[0]);
    std::vector<std::string> band_names;
    for (size_t i = 0; i < name_nvdims.size() - 2; ++i)
    {
        std::vector<std::string> tmp_bands = read_band_names(std::get<2>(name_nvdims[i]), std::get<0>(name_nvdims[i]), std::get<1>(name_nvdims[i]));

        if (band_names.empty())
        {
            band_names = tmp_bands;
        }
        else
        {
            if (tmp_bands.empty())
            {
                continue;
            }
            std::vector<std::string> new_band_names;
            for (auto& band_name : band_names)
            {
                for (auto& tmp_band : tmp_bands)
                {
                    std::string tmp_name = SUFMT("{}/{}", band_name, tmp_band);
                    new_band_names.push_back(tmp_name);
                }
            }
            band_names = new_band_names;
        }
    }
    if (band_names.empty())
    {
        band_names.push_back(group);
    }
    std::map<std::string, netCDF::NcVarAtt> attr_vars = nv_dst.getAtts();
    for (const auto& [key, attr] : attr_vars)
    {
        m_attr_names.push_back(key);
        if (key == "_FillValue")
        {
            attr.getValues(&m_fill);
        }
        else if (key == "add_offset")
        {
            attr.getValues(&m_offset);
        }
        else if (key == "scale_factor")
        {
            attr.getValues(&m_scale);
        }
    }

    val_data.resize(total_val_size);
    nv_dst.getVar(val_data.data());
    const size_t each_band_size = m_geo.xlen * m_geo.ylen;
    float* ptr = val_data.data();
    for (const auto& bn : band_names)
    {
        Band nbd;
        nbd.group = group;
        nbd.name = bn;

        nbd.grid.resize(each_band_size);
        float* dptr = nbd.grid.data();
        if (m_sort)
        {
            memcpy(dptr, ptr, each_band_size * sizeof(float));
        }
        else
        {
            for (int r = 0; r < m_geo.ylen; ++r)
            {
                memcpy(dptr + (m_geo.ylen - r - 1) * m_geo.xlen, ptr + r * m_geo.xlen, m_geo.xlen * sizeof(float));
            }
        }
        ptr += each_band_size;
        if (m_scale != 1.0)
        {
            for (auto& f : nbd.grid)
            {
                f *= m_scale;
            }
        }
        if (m_offset != 0.0)
        {
            for (auto& f : nbd.grid)
            {
                f += m_offset;
            }
        }
        m_bands.emplace_back(nbd);
    }

    status = true;
#endif
    return status;
}
std::string suNetCDF::err()
{
    return m_err;
}

suNetCDF::Geo suNetCDF::geo() const
{
    return m_geo;
}
float suNetCDF::left() const
{
    return m_geo.xmin;
}
float suNetCDF::right() const
{
    return m_geo.xmax;
}
float suNetCDF::bottom() const
{
    return m_geo.ymin;
}
float suNetCDF::top() const
{
    return m_geo.ymax;
}
float suNetCDF::scale() const
{
    return m_scale;
}
float suNetCDF::fill() const
{
    return m_fill;
}
float suNetCDF::offset() const
{
    return m_offset;
}
std::vector<float> suNetCDF::data(const std::string& band_name) const
{
    for (const auto& band : m_bands)
    {
        if (band_name == band.name)
        {
            return band.grid;
        }
    }
    return {};
}
std::vector<float> suNetCDF::data(const size_t& index) const
{
    if (index >= m_bands.size())
    {
        return {};
    }
    return m_bands[index].grid;
}
std::vector<std::vector<float>> suNetCDF::data() const
{
    std::vector<std::vector<float>> result;
    for (const auto& band : m_bands)
    {
        result.push_back(band.grid);
    }
    return result;
}

std::vector<suNetCDF::Band> suNetCDF::bands(const size_t& b, const size_t& e) const
{
    std::vector<suNetCDF::Band> ret;
    for (size_t i = b; i < e && i < m_bands.size(); ++i)
    {
        ret.push_back(m_bands[i]);
    }
    return ret;
}
std::vector<std::string> suNetCDF::band_names() const
{
    std::vector<std::string> result;
    result.reserve(m_bands.size());
    for (const auto& band : m_bands)
    {
        result.push_back(band.name);
    }
    return result;
}
size_t suNetCDF::width() const
{
    return m_geo.xlen;
}
size_t suNetCDF::height() const
{
    return m_geo.ylen;
}
float suNetCDF::xdelta() const
{
    return m_geo.xstep;
}
float suNetCDF::ydelta() const
{
    return m_geo.ystep;
}
bool suNetCDF::write(const suPath& file, const Data& snd)
{
    bool status{false};
#if SU_THIRD_SUPPORT_NETCDF_CXX
    auto fp = suPath(file);
    try
    {
        netCDF::NcFile sfc;
        sfc.open(fp.string(), netCDF::NcFile::replace, netCDF::NcFile::nc4);
        // 创建dims
        std::vector<netCDF::NcDim> dims;
        for (const auto& di : snd.dextra)
        {
            std::string name = std::get<0>(di);
            auto& vars = std::get<1>(di);
            std::string units = std::get<2>(di);
            netCDF::NcDim tmpDim = sfc.addDim(name, vars.size());
            netCDF::NcVar tmpVar = sfc.addVar(name, netCDF::ncFloat, tmpDim);  //
            if (!units.empty())
            {
                tmpVar.putAtt("units", units);
            }
            tmpVar.putVar(&vars[0]);
            dims.push_back(tmpDim);
        }
        // 坐标维度
        {
            netCDF::NcDim yDim = sfc.addDim(snd.dgeo.yname, snd.dgeo.ylen);
            netCDF::NcDim xDim = sfc.addDim(snd.dgeo.xname, snd.dgeo.xlen);
            netCDF::NcVar yVar = sfc.addVar(snd.dgeo.yname, netCDF::ncFloat, yDim);  // creates variable
            netCDF::NcVar xVar = sfc.addVar(snd.dgeo.xname, netCDF::ncFloat, xDim);
            std::vector<float> xs(snd.dgeo.xlen);
            std::vector<float> ys(snd.dgeo.ylen);
            for (int i = 0; i < snd.dgeo.xlen; i++)
            {
                xs[i] = snd.dgeo.xmin + i * snd.dgeo.xstep;
            }

            for (int i = 0; i < snd.dgeo.ylen; i++)
            {
                ys[i] = snd.dgeo.ymax - i * snd.dgeo.ystep;
            }

            yVar.putVar(ys.data());
            xVar.putVar(xs.data());
            dims.push_back(yDim);
            dims.push_back(xDim);
        }

        for (const auto& [grp, bands] : snd.grp_bands)
        {
            netCDF::NcVar data = sfc.addVar(grp, netCDF::ncFloat, dims);
            data.putAtt("_FillValue", netCDF::ncFloat, bands.front().fill);
            data.putAtt("offset", netCDF::ncFloat, bands.front().offset);
            data.putAtt("scale", netCDF::ncFloat, bands.front().scale);
            if (!bands.front().units.empty())
            {
                data.putAtt("units", bands.front().units);
            }
            std::vector<float> all(bands.size() * bands.front().grid.size());
            data.setCompression(true, true, 5);  // 设置压缩
            float* ptr = all.data();
            const size_t step = snd.dgeo.xlen * snd.dgeo.ylen * sizeof(float);
            for (const auto& band : bands)
            {
                memcpy(ptr, band.grid.data(), step);
                ptr += snd.dgeo.xlen * snd.dgeo.ylen;
            }
            data.putVar(all.data());
        }

        sfc.close();
        status = true;
    }
    catch (netCDF::exceptions::NcException& e)
    {
        SLOG_ERROR("NC: {}", std::string(e.what()))
        return status;
    }
#endif
    return status;
}

bool suNetCDF::write(const suPath& file) const
{
    bool status{false};
    // auto fp = suPath(file);
    // try
    //{
    //     netCDF::NcFile sfc;
    //     sfc.open(fp.string(), netCDF::NcFile::replace, netCDF::NcFile::nc4);
    //     // 创建dims
    //     std::vector<netCDF::NcDim> dims;
    //     for (auto tdinfo : nd.dextra)
    //     {
    //         std::string name = std::get<0>(tdinfo);
    //         auto vars = std::get<1>(tdinfo);
    //         std::string units = std::get<2>(tdinfo);
    //         netCDF::NcDim tmpDim = sfc.addDim(name, vars.size());
    //         netCDF::NcVar tmpVar = sfc.addVar(name, netCDF::ncFloat, tmpDim);  //

    //        tmpVar.putVar(&vars[0]);
    //        dims.push_back(tmpDim);
    //        tmpVar.putAtt("units", units);
    //    }
    //    // 坐标维度
    //    {
    //        netCDF::NcDim yDim = sfc.addDim(nd.dgeo.yname, nd.dgeo.ylen);
    //        netCDF::NcDim xDim = sfc.addDim(nd.dgeo.xname, nd.dgeo.xlen);
    //        netCDF::NcVar yVar = sfc.addVar(nd.dgeo.yname, netCDF::ncFloat, yDim);  // creates variable
    //        netCDF::NcVar xVar = sfc.addVar(nd.dgeo.xname, netCDF::ncFloat, xDim);
    //        std::vector<float> xs(nd.dgeo.xlen);
    //        std::vector<float> ys(nd.dgeo.ylen);
    //        float xstep = (nd.dgeo.xmax - nd.dgeo.xmin) / (nd.dgeo.xlen - 1);
    //        for (int i = 0; i < nd.dgeo.xlen; i++)
    //            xs[i] = nd.dgeo.xmin + i * xstep;

    //        float ystep = (nd.dgeo.ymax - nd.dgeo.ymin) / (nd.dgeo.ylen - 1);
    //        for (int i = 0; i < nd.dgeo.ylen; i++)
    //            ys[i] = nd.dgeo.ymin + i * ystep;
    //        yVar.putVar(&ys[0]);
    //        xVar.putVar(&xs[0]);
    //        yVar.putAtt("units", nd.dgeo.yunits);
    //        yVar.putAtt("valid_min", netCDF::ncFloat, nd.dgeo.ymin);
    //        yVar.putAtt("valid_max", netCDF::ncFloat, nd.dgeo.ymax);
    //        // yVar.putAtt("positive ", "south");

    //        xVar.putAtt("units", nd.dgeo.xunits);
    //        xVar.putAtt("valid_min", netCDF::ncFloat, nd.dgeo.xmin);
    //        xVar.putAtt("valid_max", netCDF::ncFloat, nd.dgeo.xmax);
    //        // xVar.putAtt("positive ", std::string("east"));

    //        dims.push_back(yDim);
    //        dims.push_back(xDim);
    //    }

    //    for (const auto& [grp, bands] : m_nm_data)
    //    {
    //        netCDF::NcVar data = sfc.addVar(grp, netCDF::ncFloat, dims);
    //        data.putAtt("_FillValue", netCDF::ncFloat, m_fill);
    //        data.putAtt("offset", netCDF::ncFloat, m_offset);
    //        data.putAtt("scale", netCDF::ncFloat, m_scale);
    //        data.putAtt("units", m_nm_unit.at(grp));
    //        std::vector<float> all(bands);
    //        data.setCompression(true, true, 5);
    //        for (int i = 0; i < bands.size(); i++)
    //        {
    //            memcpy(((char*)&all[0]) + i * bands[0].grid.size() * sizeof(float), (char*)&bands[i].grid[0], bands[i].grid.size() * sizeof(float));
    //        }
    //        data.putVar(&all[0]);
    //    }

    //    sfc.close();
    //    status = true;
    //}
    // catch (NcException& e)
    //{
    //    SLOG_ERROR("NC: {}", std::string(e.what()))
    //    return status;
    //}
    return status;
}

silly_tzx_grid suNetCDF::convert(const size_t& b, const size_t& e) const
{
    silly_tzx_grid ret;
    if (m_bands.empty() || b > e)
    {
        return ret;
    }
    suRect bound;
    bound.min.x = m_geo.xmin;
    bound.min.y = m_geo.ymin;
    bound.max.x = m_geo.xmax;
    bound.max.y = m_geo.ymax;
    ret.rect(bound);
    ret.rows(m_geo.ylen);
    ret.cols(m_geo.xlen);

    for (size_t i = b; i < e && i < m_bands.size(); ++i)
    {
        suFMatrix tmp;
        tmp.create(m_geo.ylen, m_geo.xlen);
        auto max_it = std::max_element(m_bands[i].grid.begin(), m_bands[i].grid.end());
        // auto min_it = std::min_element(m_bands[i].grid.begin(), m_bands[i].grid.end());
        memcpy(tmp.data(), m_bands[i].grid.data(), m_bands[i].grid.size() * sizeof(float));
        SLOG_DEBUG("{}: {}, {}", i, tmp.max(), *max_it)
        ret.add(tmp);
    }

    return ret;
}