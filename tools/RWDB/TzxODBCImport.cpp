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


std::string insert_pptn_sql;
std::string inster_stbprp_sql;
std::string stbprp_file_path;
std::string pptn_file_path;
std::string btm;
std::string etm;
silly_otl otl;
std::map<uint32_t, std::string> index_stcd;


// 读取配置文件
bool init(const std::string& file);
// 导入stbprp
bool import_stbprp();
// 导入pptn
bool import_pptn();



int main(int argc, char** argv)
{

#ifndef NDEBUG
    std::string configPath = "../../../../tools/RWDB/import.json";
#else
    std::string configPath = "./config/import.json";
#endif

    if (!init(configPath))
    {
        return -1;
    }

    // 导入stbprp
    import_stbprp();
    // 导入pptn
    import_pptn();

    return 0;
}


bool init(const std::string& file)
{
    bool status = false;
    Json::Value jv_root = silly_jsonpp::loadf(file);
    if (jv_root.isNull())
    {
        SFP_ERROR("配置文件读取失败: {}", file);
        return status;
    }
    otl = otl_tools::conn_opt_from_json(jv_root["des_db"]);
    otl.dump_odbc();

    insert_pptn_sql = jv_root["sql"]["insert_pptn_sql"].asString();
    inster_stbprp_sql = jv_root["sql"]["inster_stbprp_sql"].asString();

    stbprp_file_path = jv_root["stbprp_file_path"].asString();
    pptn_file_path = jv_root["pptn_file_path"].asString();

    status = true;
    return status;
}


bool import_stbprp()
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
        silly_stbprp stbprp;
        if (stbprp.deserialize(once_data))  // 反序列化解析文件中的数据
        {
            des_stbprps.push_back(stbprp); 
        }
    }
    ifs.close();  // 关闭文件

    // 读取stcd对应的index,是否需要转码
    for (const auto& entry : des_stbprps)
    {
        index_stcd[entry.index] = entry.STCD;
    }

    bool trans = false;
    if (otl.type() == enum_database_type::dbDM8)
    {
        trans = true;  // 如果是达梦数据库，需要将中文字段转为gbk
    }
    if (trans)
    {
        for (auto& entry : des_stbprps)
        {
            entry.STCD = silly_encode::utf8_gbk(entry.STCD);
            entry.STNM = silly_encode::utf8_gbk(entry.STNM);
            entry.RVNM = silly_encode::utf8_gbk(entry.RVNM);
            entry.HNNM = silly_encode::utf8_gbk(entry.HNNM);
            entry.BSNM = silly_encode::utf8_gbk(entry.BSNM);
            entry.STLC = silly_encode::utf8_gbk(entry.STLC);
            entry.ADDVCD = silly_encode::utf8_gbk(entry.ADDVCD);
            entry.DTMNM = silly_encode::utf8_gbk(entry.DTMNM);
            entry.STTP = silly_encode::utf8_gbk(entry.STTP);
            entry.FRGRD = silly_encode::utf8_gbk(entry.FRGRD);
            entry.ESSTYM = silly_encode::utf8_gbk(entry.ESSTYM);
            entry.BGFRYM = silly_encode::utf8_gbk(entry.BGFRYM);
            entry.ATCUNIT = silly_encode::utf8_gbk(entry.ATCUNIT);
            entry.ADMAUTH = silly_encode::utf8_gbk(entry.ADMAUTH);
            entry.LOCALITY = silly_encode::utf8_gbk(entry.LOCALITY);
            entry.STBK = silly_encode::utf8_gbk(entry.STBK);
            entry.PHCD = silly_encode::utf8_gbk(entry.PHCD);
            entry.USFL = silly_encode::utf8_gbk(entry.USFL);
            entry.COMMENTS = silly_encode::utf8_gbk(entry.COMMENTS);
            entry.HNNM0 = silly_encode::utf8_gbk(entry.HNNM0);
            entry.ADCD = silly_encode::utf8_gbk(entry.ADCD);
            entry.ADDVCD1 = silly_encode::utf8_gbk(entry.ADDVCD1);
        }
    }

    // 插入数据库
    if (!otl.insert(inster_stbprp_sql, [&des_stbprps](otl_stream* stream) {
            for (const auto& entry : des_stbprps)
            {
                otl_value<std::string> STCD(entry.STCD);
                otl_value<std::string> STNM(entry.STNM);
                otl_value<std::string> RVNM(entry.RVNM);
                otl_value<std::string> HNNM(entry.HNNM);
                otl_value<std::string> BSNM(entry.BSNM);
                otl_value<float> LGTD(entry.LGTD);
                otl_value<float> LTTD(entry.LTTD);
                otl_value<std::string> STLC(entry.STLC);
                otl_value<std::string> ADDVCD(entry.ADDVCD);
                otl_value<std::string> DTMNM(entry.DTMNM);
                otl_value<float> DTMEL(entry.DTMEL);
                otl_value<float> DTPR(entry.DTPR);
                otl_value<std::string> STTP(entry.STTP);
                otl_value<std::string> FRGRD(entry.FRGRD);
                otl_value<std::string> ESSTYM(entry.ESSTYM);
                otl_value<std::string> BGFRYM(entry.BGFRYM);
                otl_value<std::string> ATCUNIT(entry.ATCUNIT);
                otl_value<std::string> ADMAUTH(entry.ADMAUTH);
                otl_value<std::string> LOCALITY(entry.LOCALITY);
                otl_value<std::string> STBK(entry.STBK);
                otl_value<int> STAzt(entry.STAzt);
                otl_value<float> DSTRVM(entry.DSTRVM);
                otl_value<int> DRNA(entry.DRNA);
                otl_value<std::string> PHCD(entry.PHCD);
                otl_value<std::string> USFL(entry.USFL);
                otl_value<std::string> COMMENTS(entry.COMMENTS);
                otl_datetime MODITIME = otl_tools::otl_time_from_string(entry.MODITIME);
                otl_value<std::string> HNNM0(entry.HNNM0);
                otl_value<std::string> ADCD(entry.ADCD);
                otl_value<std::string> ADDVCD1(entry.ADDVCD1);

                otl_write_row(*stream, STCD, STNM, RVNM, HNNM, BSNM, LGTD, LTTD, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    SLOG_INFO("{} 导入完成", stbprp_file_path);

    return true;
}




bool import_pptn()
{
    if (index_stcd.empty())
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
            des_pptns.push_back(temp); 
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
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            pptn.stcd = t_stcd;
        }
    }

    // 插入数据
    if (!otl.insert(insert_pptn_sql, [&des_pptns](otl_stream* stream) {
            for (const auto& entry : des_pptns)
            {
                otl_value<std::string> stcd(entry.stcd.c_str());

                struct tm* timeinfo;
                timeinfo = localtime(&entry.stamp);
                otl_datetime tm;
                tm.year = (timeinfo->tm_year + 1900);
                tm.month = timeinfo->tm_mon + 1;
                tm.day = timeinfo->tm_mday;
                tm.hour = timeinfo->tm_hour;
                tm.minute = timeinfo->tm_min;
                tm.second = timeinfo->tm_sec;

                otl_value<float> intv(entry.intv);
                otl_value<float> drp(entry.drp);

                otl_write_row(*stream, stcd, tm, drp, intv);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    SLOG_INFO("{} 导入完成", pptn_file_path);
    
    return true;
}























