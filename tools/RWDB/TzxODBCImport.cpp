/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: TzxODBCImport.c
 * @description: 使用ODBC导入到自定文件到RWDB数据库
 * @version: v1.0.1 2024-09-30 dou li yang
 */

#include <database/otl/silly_otl.h>
#include <database/otl/otl_tools.h>
#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_stbprp.h>
#include <tzx/rwdb/silly_rsvr.h>
#include <tzx/rwdb/silly_river.h>
#include "encode/silly_encode.h"


// 读取stcd对应的索引
std::map<uint32_t, std::string> deserialize_stcd_index(const std::string& filepath)
{
    // std::map<std::string, uint32_t> des_stcd_index;
    std::map<uint32_t, std::string> des_index_stcd;
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        SFP_ERROR("Failed to open file: {}", filepath);
        return des_index_stcd;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string stcd;
        uint32_t index;
        if (!(iss >> stcd >> index))
        {
            continue;
        }

        des_index_stcd[index] = stcd;
    }

    file.close();
    return des_index_stcd;
}

class silly_import_pptn
{
  public:
    bool init(const std::string& file);
    bool import_pptn();

    std::string des_odbc;
    std::string insert_pptn_sql;

    std::string pptn_file_path;
    std::string stcd_index_file;

    silly_otl otl;
};

bool silly_import_pptn::init(const std::string& file)
{
    bool status = false;
    Json::Value jv_root = silly_jsonpp::loadf(file);
    if (jv_root.isNull())
    {
        SFP_ERROR("配置文件读取失败: {}", file);
        return status;
    }
    otl = otl_tools::conn_opt_from_json(jv_root["des_db"]);
    des_odbc = otl.dump_odbc();
    insert_pptn_sql = jv_root["sql"]["insert_pptn_sql"].asString();

    pptn_file_path = jv_root["pptn_file_path"].asString();
    stcd_index_file = jv_root["stcd_index_file"].asString();

    status = true;
    return status;
}

bool silly_import_pptn::import_pptn()
{
    std::map<uint32_t, std::string> des_index_stcd = deserialize_stcd_index(stcd_index_file);
    if (des_index_stcd.empty())
    {
        return false;
    }

    std::vector<silly_pptn> des_pptns;                    // 用于存储反序列化后的对象
    std::ifstream ifs(pptn_file_path, std::ios::binary);  // 以二进制方式打开文件
    if (!ifs.is_open())
    {
        std::cout << "Failed to open file for reading." << std::endl;
        return false;
    }
    while (!ifs.eof())
    {
        std::string buffer;                  // 存储每次读取的数据
        buffer.resize(silly_pptn::SIZE_V1);  // 为每个对象分配大小
                                             // 读取数据到 buffer
        ifs.read(&buffer[0], silly_pptn::SIZE_V1);
        if (ifs.gcount() == 0)
        {           // 检查是否读取结束
            break;  // 到达文件末尾
        }
        // 创建一个新的 silly_pptn 对象
        silly_pptn pptn;
        // 反序列化
        silly_pptn temp;
        if (temp.deserialize(buffer))
        {
            des_pptns.push_back(temp);  // 将反序列化后的对象添加到 vector 中
        }
        else
        {
            std::cerr << "Failed to deserialize an object from the data." << std::endl;
        }
    }
    ifs.close();

    for (auto& pptn : des_pptns)
    {
        uint32_t t_index = pptn.index;
        if (des_index_stcd.find(t_index) != des_index_stcd.end())
        {
            std::string t_stcd = des_index_stcd[t_index];
            pptn.stcd = t_stcd;
        }
    }

    // 插入数据
    if (!otl.insert(insert_pptn_sql, [&des_pptns](otl_stream* stream) {
            // 循环插入每条数据
            for (const auto& entry : des_pptns)
            {
                otl_value<std::string> stcd(entry.stcd.c_str());

                struct tm* timeinfo;
                char buffer[20];
                timeinfo = localtime(&entry.stamp);
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

                otl_datetime tm = otl_tools::otl_time_from_string(buffer);
                otl_value<float> intv(entry.intv);
                otl_value<float> drp(entry.drp);

                otl_write_row(*stream, stcd, tm, drp, intv);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    return true;
}

class silly_import_stbprp
{
  public:
    // 初始化
    bool init(const std::string& file);

    // 导出测站基本信息
    bool import_stbprp();

    std::string des_odbc;
    std::string inster_stbprp_sql;

    std::string stbprp_file_path;

    silly_otl otl;
};

bool silly_import_stbprp::init(const std::string& file)
{
    bool status = false;
    Json::Value jv_root = silly_jsonpp::loadf(file);
    if (jv_root.isNull())
    {
        SFP_ERROR("配置文件读取失败: {}", file);
        return status;
    }
    otl = otl_tools::conn_opt_from_json(jv_root["des_db"]);
    des_odbc = otl.dump_odbc();

    inster_stbprp_sql = jv_root["sql"]["inster_stbprp_sql"].asString();
    stbprp_file_path = jv_root["stbprp_file_path"].asString();
    status = true;
    return status;
}

bool silly_import_stbprp::import_stbprp()
{
    std::vector<silly_stbprp> des_stbprps;
    std::ifstream ifs(stbprp_file_path, std::ios::binary);
    if (!ifs.is_open())
    {
        SLOG_ERROR("stbprp_file_path open failed");
        return false;
    }
    while (!ifs.eof())  // 当文件没有到达末尾时
    {
        // 获取当前的文件位置指示器
        std::streampos beforPosition = ifs.tellg();
        uint32_t once_size;
        ifs.read(reinterpret_cast<char*>(&once_size), sizeof(once_size));  // 读取4个字节
        ifs.seekg(beforPosition);
        std::string once_data;
        once_data.resize(once_size);
        ifs.read(&once_data[0], once_size);
        // 检查读取是否成功
        if (ifs.fail())
        {
            break;  // 如果读取失败，退出循环
        }
        silly_stbprp stbprp;                // 创建一个新对象
        if (stbprp.deserialize(once_data))  // 尝试反序列化
        {
            des_stbprps.push_back(stbprp);  // 添加到容器中
        }
    }
    ifs.close();  // 关闭文件

    // 插入数据库
    bool isDm8 = false;
    if (otl.type() == enum_database_type::dbDM8)
    {
        isDm8 = true; // 如果是达梦数据库，需要将中文字段转为gbk
    }
    if (!otl.insert(inster_stbprp_sql, [&des_stbprps, &isDm8](otl_stream* stream) {
            for (const auto& entry : des_stbprps)
            {
                otl_value<std::string> STCD(isDm8 ? silly_encode::utf8_gbk(entry.STCD) : entry.STCD);
                otl_value<std::string> STNM(isDm8 ? silly_encode::utf8_gbk(entry.STNM) : entry.STNM);
                otl_value<std::string> RVNM(isDm8 ? silly_encode::utf8_gbk(entry.RVNM) : entry.RVNM);
                otl_value<std::string> HNNM(isDm8 ? silly_encode::utf8_gbk(entry.HNNM) : entry.HNNM);
                otl_value<std::string> BSNM(isDm8 ? silly_encode::utf8_gbk(entry.BSNM) : entry.BSNM);
                otl_value<float> LGTD(entry.LGTD);
                otl_value<float> LTTD(entry.LTTD);
                otl_value<std::string> STLC(isDm8 ? silly_encode::utf8_gbk(entry.STLC) : entry.STLC);
                otl_value<std::string> ADDVCD(isDm8 ? silly_encode::utf8_gbk(entry.ADDVCD) : entry.ADDVCD);
                otl_value<std::string> DTMNM(isDm8 ? silly_encode::utf8_gbk(entry.DTMNM) : entry.DTMNM);
                otl_value<float> DTMEL(entry.DTMEL);
                otl_value<float> DTPR(entry.DTPR);
                otl_value<std::string> STTP(isDm8 ? silly_encode::utf8_gbk(entry.STTP) : entry.STTP);
                otl_value<std::string> FRGRD(isDm8 ? silly_encode::utf8_gbk(entry.FRGRD) : entry.FRGRD);
                otl_value<std::string> ESSTYM(isDm8 ? silly_encode::utf8_gbk(entry.ESSTYM) : entry.ESSTYM);
                otl_value<std::string> BGFRYM(isDm8 ? silly_encode::utf8_gbk(entry.BGFRYM) : entry.BGFRYM);
                otl_value<std::string> ATCUNIT(isDm8 ? silly_encode::utf8_gbk(entry.ATCUNIT) : entry.ATCUNIT);
                otl_value<std::string> ADMAUTH(isDm8 ? silly_encode::utf8_gbk(entry.ADMAUTH) : entry.ADMAUTH);
                otl_value<std::string> LOCALITY(isDm8 ? silly_encode::utf8_gbk(entry.LOCALITY) : entry.LOCALITY);
                otl_value<std::string> STBK(isDm8 ? silly_encode::utf8_gbk(entry.STBK) : entry.STBK);
                otl_value<int> STAzt(entry.STAzt);
                otl_value<float> DSTRVM(entry.DSTRVM);
                otl_value<int> DRNA(entry.DRNA);
                otl_value<std::string> PHCD(isDm8 ? silly_encode::utf8_gbk(entry.PHCD) : entry.PHCD);
                otl_value<std::string> USFL(isDm8 ? silly_encode::utf8_gbk(entry.USFL) : entry.USFL);
                otl_value<std::string> COMMENTS(isDm8 ? silly_encode::utf8_gbk(entry.COMMENTS) : entry.COMMENTS);

                otl_datetime MODITIME = otl_tools::otl_time_from_string(entry.MODITIME);
                otl_value<std::string> HNNM0(isDm8 ? silly_encode::utf8_gbk(entry.HNNM0) : entry.HNNM0);
                otl_value<std::string> ADCD(isDm8 ? silly_encode::utf8_gbk(entry.ADCD) : entry.ADCD);
                otl_value<std::string> ADDVCD1(isDm8 ? silly_encode::utf8_gbk(entry.ADDVCD1) : entry.ADDVCD1);


                otl_write_row(*stream, STCD, STNM, RVNM, HNNM, BSNM, LGTD, LTTD, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    return true;
}

int main(int argc, char** argv)
{

    silly_import_pptn pptn;
    pptn.init("../../../../tools/RWDB/import.json");

    silly_import_stbprp stbprp;
    stbprp.init("../../../../tools/RWDB/import.json");

    pptn.import_pptn();

    stbprp.import_stbprp();

    return 0;
}

