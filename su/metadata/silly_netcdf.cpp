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
#include <netcdf.h>
/*
 enum ncType
   {
     nc_BYTE     = NC_BYTE, 	//!< signed 1 byte integer
     nc_CHAR     = NC_CHAR,	//!< ISO/ASCII character
     nc_SHORT    = NC_SHORT, 	//!< signed 2 byte integer
     nc_INT      = NC_INT,	//!< signed 4 byte integer
     nc_FLOAT    = NC_FLOAT, 	//!< single precision floating point number
     nc_DOUBLE   = NC_DOUBLE, 	//!< double precision floating point number
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
bool silly_netcdf::open(const std::string& path)
{
    bool status = false;
    try
    {
        m_nc_file.open(path, NcFile::read);
        status = true;
    }
    catch (NcException& e)
    {
        std::cout << e.what() << std::endl;
        return status;
    }

    return status;
}
std::vector<std::string> silly_netcdf::members()
{
    std::vector<std::string> result;
    NcGroup m_nc_all_grps = m_nc_file.getGroup("/", NcGroup::GroupLocation::AllGrps);
    auto nc_all_vars = m_nc_all_grps.getVars(netCDF::NcGroup::Current);
    for (auto [k, var] : nc_all_vars)
    {
        result.push_back(k);
    }
    for (auto nm : result)
    {
        // m_nc_all_grps
    }
    return result;
}
bool silly_netcdf::geometry(const std::string& lon, const std::string& lat)
{
    /*  NcVar lon_var = m_nc_all_grps.getVar(lon);
      NcVar lat_var = m_nc_all_grps.getVar(lat);
      lon_var.getDimCount();*/
    return false;
}
void silly_netcdf::close()
{
    m_nc_file.close();
}

std::vector<std::string> read_band_names(NcVar nv, std::string name, size_t len)
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
    std::vector<double> v_d;

    switch (nv.getType().getId())
    {
        case NcType::nc_BYTE:
        case NcType::nc_CHAR:
            v_c.resize(len);
            nv.getVar(&v_c[0]);
            for (auto& c : v_c)
            {
                result.push_back(silly_format::format("{}:{}", name, c));
            }
            break;
        case NcType::nc_SHORT:
            v_s.resize(len);
            nv.getVar(&v_s[0]);
            for (auto& s : v_s)
            {
                result.push_back(silly_format::format("{}:{}", name, s));
            }
            break;
        case NcType::nc_INT:
            v_i.resize(len);
            nv.getVar(&v_i[0]);
            for (auto& i : v_i)
            {
                result.push_back(silly_format::format("{}:{}", name, i));
            }
            break;
        case NcType::nc_INT64:
            v_ll.resize(len);
            nv.getVar(&v_ll[0]);
            for (auto& ll : v_ll)
            {
                result.push_back(silly_format::format("{}:{}", name, ll));
            }
            break;
        case NcType::nc_UBYTE:
            v_uc.resize(len);
            nv.getVar(&v_uc[0]);
            for (auto& uc : v_uc)
            {
                result.push_back(silly_format::format("{}:{}", name, uc));
            }
            break;
        case NcType::nc_USHORT:
            v_us.resize(len);
            nv.getVar(&v_us[0]);
            for (auto& us : v_us)
            {
                result.push_back(silly_format::format("{}:{}", name, us));
            }
            break;
        case NcType::nc_UINT:
            v_ui.resize(len);
            nv.getVar(&v_ui[0]);
            for (auto& ui : v_ui)
            {
                result.push_back(silly_format::format("{}:{}", name, ui));
            }
            break;
        case NcType::nc_UINT64:
            v_ull.resize(len);
            nv.getVar(&v_ull[0]);
            for (auto& ull : v_ull)
            {
                result.push_back(silly_format::format("{}:{}", name, ull));
            }
            break;
        case NcType::nc_FLOAT:
            v_f.resize(len);
            nv.getVar(&v_f[0]);
            for (auto& f : v_f)
            {
                result.push_back(silly_format::format("{}:{}", name, f));
            }
            break;
        case NcType::nc_DOUBLE:
            v_d.resize(len);
            nv.getVar(&v_d[0]);
            for (auto& d : v_d)
            {
                result.push_back(silly_format::format("{}:{}", name, d));
            }
            break;
        case NcType::nc_STRING:
            v_str.resize(len);
            nv.getVar(&v_str[0]);
            for (auto& str : v_str)
            {
                result.push_back(silly_format::format("{}:{}", name, str));
            }
            break;
        default:
            break;
    }

    return result;
}
bool silly_netcdf::read(const std::string& group, const std::string& lon, const std::string& lat)
{
    bool status = false;
    m_nc_all_grps = m_nc_file.getGroup("/", NcGroup::GroupLocation::AllGrps);
    NcVar nv_dst = m_nc_all_grps.getVar(group);
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
    std::vector<NcDim> ndims = nv_dst.getDims();
    std::vector<std::tuple<std::string, size_t, NcVar>> name_nvdims;
    std::vector<float> lat_data, lon_data, val_data;
    size_t total_val_size = 1;
    for (auto ndim : ndims)
    {
        std::string name = ndim.getName();
        SLOG_DEBUG(name)
        /* if(ndim.getSize() == 1)
         {
             continue;
         }*/
        if (lon == name)
        {
            m_width = ndim.getSize();
            lon_data.resize(m_width);
        }
        else if (lat == name)
        {
            m_height = ndim.getSize();
            lat_data.resize(m_height);
        }
        total_val_size *= ndim.getSize();
        name_nvdims.push_back({name, ndim.getSize(), m_nc_all_grps.getVar(ndim.getName())});
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
    m_left = std::min(lon_data.back(), lon_data.front());
    m_right = std::max(lon_data.back(), lon_data.front());
    m_bottom = std::min(lat_data.back(), lat_data.front());
    m_top = std::max(lat_data.back(), lat_data.front());
    m_xdelta = (m_right - m_left) / (m_width - 1);
    m_ydelta = (m_top - m_bottom) / (m_height - 1);
    if (lat.front() < lat.back())  // 纬度由小到大
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
            for (size_t n = 0; n < band_names.size(); ++n)
            {
                for (size_t m = 0; m < tmp_bands.size(); ++m)
                {
                    std::string tmp_name = silly_format::format("{}/{}", band_names[n], tmp_bands[m]);
                    new_band_names.push_back(tmp_name);
                }
            }
            band_names = new_band_names;
        }
    }
    for (auto bn : band_names)
    {
        std::cout << bn << std::endl;
    }
    std::map<std::string, NcVarAtt> attr_vars = nv_dst.getAtts();
    for (auto [key_, attr_] : attr_vars)
    {
        if (key_ == "_FillValue")
        {
            attr_.getValues(&m_fill);
        }
        else if (key_ == "add_offset")
        {
            attr_.getValues(&m_offset);
        }
        else if (key_ == "scale_factor")
        {
            attr_.getValues(&m_scale);
        }
    }

    val_data.resize(total_val_size);
    nv_dst.getVar(&val_data[0]);
    size_t curr_i = 0;
    size_t each_band_size = m_width * m_height;
    for (auto bn : band_names)
    {
        std::vector<float> tmp_data(each_band_size);
        if (m_sort)
        {
            memcpy(&tmp_data[0], &val_data[curr_i], each_band_size * sizeof(float));
        }
        else
        {
            for (int i = m_height - 1, j = 0; i >= 0; --i, ++j)
            {
                memcpy(&tmp_data[j * m_width], &val_data[curr_i + i * m_width], m_width * sizeof(float));
            }
        }
        if (m_scale != 1.0)
        {
            for (auto& f : tmp_data)
            {
                f *= m_scale;
            }
        }
        if (m_offset != 0.0)
        {
            for (auto& f : tmp_data)
            {
                f += m_offset;
            }
        }
        /*  for(auto f : tmp_data)
          {
              if(f> 0)
              {
                  SLOG_DEBUG("{:.3f}", f)
                  break;
              }
          }*/
        m_nm_data.push_back({bn, tmp_data});
        curr_i += each_band_size;
    }
    return true;
}
std::string silly_netcdf::err()
{
    return m_err;
}
double silly_netcdf::left() const
{
    return m_left;
}
double silly_netcdf::right() const
{
    return m_right;
}
double silly_netcdf::bottom() const
{
    return m_bottom;
}
double silly_netcdf::top() const
{
    return m_top;
}
double silly_netcdf::scale() const
{
    return m_scale;
}
double silly_netcdf::fill() const
{
    return m_fill;
}
double silly_netcdf::offset() const
{
    return m_offset;
}
std::vector<float> silly_netcdf::data(const std::string& band_name)
{
    for (auto [name, data] : m_nm_data)
    {
        if (name == band_name)
        {
            return data;
        }
    }
    return {};
}
std::vector<float> silly_netcdf::data(const size_t& index)
{
    if (index >= m_nm_data.size())
    {
        return {};
    }
    return m_nm_data[index].second;
}
std::vector<std::vector<float>> silly_netcdf::data()
{
    std::vector<std::vector<float>> result;
    for (auto [name, data] : m_nm_data)
    {
        result.push_back(data);
    }
    return result;
}
std::vector<std::string> silly_netcdf::band_names()
{
    std::vector<std::string> result;
    for (auto [name, data] : m_nm_data)
    {
        result.push_back(name);
    }
    return result;
}
size_t silly_netcdf::width() const
{
    return m_width;
}
size_t silly_netcdf::height() const
{
    return m_height;
}
double silly_netcdf::xdelta() const
{
    return m_xdelta;
}
double silly_netcdf::ydelta() const
{
    return m_ydelta;
}
bool silly_netcdf::write(const std::string& path, const silly_netcdf_data& nd)
{
    bool status{false};
    try
    {
        NcFile sfc;
        sfc.open(path, NcFile::replace, NcFile::nc4);
        // 创建dims
        std::vector<NcDim> dims;
        for (auto tdinfo : nd.dextra)
        {
            std::string name = std::get<0>(tdinfo);
            auto vars = std::get<1>(tdinfo);
            std::string units = std::get<2>(tdinfo);
            NcDim tmpDim = sfc.addDim(name, vars.size());
            NcVar tmpVar = sfc.addVar(name, ncFloat, tmpDim);  //

            tmpVar.putVar(&vars[0]);
            dims.push_back(tmpDim);
            tmpVar.putAtt("units", units);
        }
        // 坐标维度
        {
            NcDim yDim = sfc.addDim(nd.dgeo.yname, nd.dgeo.ylen);
            NcDim xDim = sfc.addDim(nd.dgeo.xname, nd.dgeo.xlen);
            NcVar yVar = sfc.addVar(nd.dgeo.yname, ncFloat, yDim);  // creates variable
            NcVar xVar = sfc.addVar(nd.dgeo.xname, ncFloat, xDim);
            std::vector<float> xs(nd.dgeo.xlen);
            std::vector<float> ys(nd.dgeo.ylen);
            float xstep = (nd.dgeo.xlast - nd.dgeo.xfirst) / (nd.dgeo.xlen - 1);
            for (int i = 0; i < nd.dgeo.xlen; i++)
                xs[i] = nd.dgeo.xfirst + i * xstep;

            float ystep = (nd.dgeo.ylast - nd.dgeo.yfirst) / (nd.dgeo.ylen - 1);
            for (int i = 0; i < nd.dgeo.ylen; i++)
                ys[i] = nd.dgeo.yfirst + i * ystep;
            yVar.putVar(&ys[0]);
            xVar.putVar(&xs[0]);
            yVar.putAtt("units", nd.dgeo.yunits);
            yVar.putAtt("valid_min", ncFloat, nd.dgeo.ymin);
            yVar.putAtt("valid_max", ncFloat, nd.dgeo.ymax);
            // yVar.putAtt("positive ", "south");

            xVar.putAtt("units", nd.dgeo.xunits);
            xVar.putAtt("valid_min", ncFloat, nd.dgeo.xmin);
            xVar.putAtt("valid_max", ncFloat, nd.dgeo.xmax);
            // xVar.putAtt("positive ", std::string("east"));

            dims.push_back(yDim);
            dims.push_back(xDim);
        }

        for (auto [grp, bands] : nd.grp_bands)
        {
            NcVar data = sfc.addVar(grp, ncFloat, dims);
            data.putAtt("_FillValue", ncFloat, bands[0].fill);
            data.putAtt("offset", ncFloat, bands[0].offset);
            data.putAtt("scale", ncFloat, bands[0].scale);
            data.putAtt("units", bands[0].units);
            std::vector<float> all(bands.size() * bands[0].grid.size());
            for (int i = 0; i < bands.size(); i++)
            {
                memcpy(((char*)&all[0]) + i * bands[0].grid.size() * sizeof(float), (char*)&bands[i].grid[0], bands[i].grid.size() * sizeof(float));
            }
            data.putVar(&all[0]);
        }

        sfc.close();
        status = true;
    }
    catch (NcException& e)
    {
        SLOG_ERROR("NC: {}", std::string(e.what()))
        return status;
    }
    return status;
}
