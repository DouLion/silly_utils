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
#include "export_tool.h"
#include "datetime/silly_timer.h"  // 计时


silly_otl otl;
std::string insert_pptn_sql;
std::string insert_stbprp_sql;
std::string insert_river_sql;
std::string stbprp_file_path;
std::string pptn_file_path;
std::string river_file_path;
std::string btm;
std::string etm;
std::string src_encode;
std::string dst_encode;
std::map<uint32_t, std::string> index_stcd;


// 读取配置文件
bool init(const std::string& file);
// 导入stbprp
bool import_stbprp(bool insert=false);
// 导入pptn
bool import_pptn();

bool import_river();



int main(int argc, char** argv)
{

    // 初始化布尔变量，默认值为false
    bool has_pptn = false;
    bool has_river = true;
    bool has_rsvr = false;

    // 遍历命令行参数（从索引1开始，因为argv[0]是程序名）
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "pptn")
        {
            has_pptn = true;
        }
        else if (arg == "river")
        {
            has_river = true;
        }
        else if (arg == "rsvr")
        {
            has_rsvr = true;
        }
    }

#ifndef NDEBUG
    std::string configPath = std::filesystem::path(DEFAULT_SU_ROOT_DIR).append("docs").append("数据库导入导出").append("import.json").string();
#else
    std::string configPath = "./import.json";
#endif

    if (!init(configPath))
    {
        SLOG_ERROR("init failed:{}", configPath);
        return -1;
    }
    SLOG_INFO("读取配置文件完成开始导入数据");

    silly_timer timer;
    timer.restart();

    // 导入stbprp

    if (!import_stbprp())
    {
        SLOG_ERROR("import stbprp failed");
        return -1;
    }
    SLOG_INFO("stbprp 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    
    if (has_pptn)
    {
        // 导入pptn
        timer.restart();
        import_pptn();
        SLOG_INFO("pptn 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }

    if (has_river)
    {
        timer.restart();
        if (!import_river())
        {
            SLOG_ERROR("import river failed");
            return -1;
        }
        SLOG_INFO("river 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }


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
    otl = otl_tools::conn_opt_from_json(jv_root["db"]);
    otl.odbc();

    insert_pptn_sql = jv_root["sql"]["insert_pptn_sql"].asString();
    insert_stbprp_sql = jv_root["sql"]["insert_stbprp_sql"].asString();
    insert_river_sql = jv_root["sql"]["insert_river_sql"].asString();

    stbprp_file_path = jv_root["stbprp_file_path"].asString();
    pptn_file_path = jv_root["pptn_file_path"].asString();
    river_file_path = jv_root["river_file_path"].asString();


    src_encode = jv_root["encode"]["src"].asString();
    dst_encode = jv_root["encode"]["dst"].asString();

    status = true;
    return status;
}


bool import_stbprp(bool insert)
{
    std::vector<silly_stbprp> des_stbprps;

    // -------------文件读取-----------------
    std::ifstream ifs(stbprp_file_path, std::ios::binary);
    if (!ifs.is_open())
    {
        SLOG_ERROR("stbprp_file_path open failed: {}", stbprp_file_path);
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

    // --------------index 找 stcd--------------
    for (const auto& entry : des_stbprps)
    {
        index_stcd[entry.index] = entry.STCD;
    }

    // --------------转编码-----------------
    if (!src_encode.empty() && !dst_encode.empty())
    {
        for (auto& entry : des_stbprps)
        {
            encode(entry, src_encode, dst_encode);
        }
    }
    SLOG_INFO("stbprp insert size:{}", des_stbprps.size());
    if (!insert)
    {
        return true;
    }
    //return true; // 临时添加
    // --------------插入数据库--------------
    if (!otl.insert(insert_stbprp_sql, [&des_stbprps](otl_stream* stream) {
            for (const auto& entry : des_stbprps)
            {
                otl_value<std::string> STCD(entry.STCD);
                otl_value<std::string> STNM(entry.STNM);
                //otl_value<std::string> RVNM(entry.RVNM);
                //otl_value<std::string> HNNM(entry.HNNM);
                //otl_value<std::string> BSNM(entry.BSNM);
                otl_value<double> LGTD(entry.LGTD);
                otl_value<double> LTTD(entry.LTTD);
                //otl_value<std::string> STLC(entry.STLC);
                //otl_value<std::string> ADDVCD(entry.ADDVCD);
                //otl_value<std::string> DTMNM(entry.DTMNM);
                //otl_value<double> DTMEL(entry.DTMEL);
                //otl_value<double> DTPR(entry.DTPR);
                //otl_value<std::string> STTP(entry.STTP);
                //otl_value<std::string> FRGRD(entry.FRGRD);
                //otl_value<std::string> ESSTYM(entry.ESSTYM);
                //otl_value<std::string> BGFRYM(entry.BGFRYM);
                //otl_value<std::string> ATCUNIT(entry.ATCUNIT);
                //otl_value<std::string> ADMAUTH(entry.ADMAUTH);
                //otl_value<std::string> LOCALITY(entry.LOCALITY);
                //otl_value<std::string> STBK(entry.STBK);
                //otl_value<int> STAzt(entry.STAzt);
                //otl_value<double> DSTRVM(entry.DSTRVM);
                //otl_value<int> DRNA(entry.DRNA);
                //otl_value<std::string> PHCD(entry.PHCD);
                //otl_value<std::string> USFL(entry.USFL);
                //otl_value<std::string> COMMENTS(entry.COMMENTS);
                //otl_datetime MODITIME = otl_tools::otl_time_from_string(entry.MODITIME);
                //otl_value<std::string> HNNM0(entry.HNNM0);
                //otl_value<std::string> ADCD(entry.ADCD);
                //otl_value<std::string> ADDVCD1(entry.ADDVCD1);

                otl_write_row(*stream, STCD, STNM/*, RVNM, HNNM, BSNM*/, LGTD, LTTD/*, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1*/);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    SLOG_INFO("{} 导入完成, 导入数量: {}", stbprp_file_path, des_stbprps.size());

    return true;
}


bool import_pptn()
{
    if (index_stcd.empty())
    {
        return false;
    }

    std::vector<silly_pptn> des_pptns;                    // 用于存储反序列化后的对象

    // -----------文件读取-------------
    std::ifstream ifs(pptn_file_path, std::ios::binary);  // 以二进制方式打开文件
    if (!ifs.is_open())
    {
        SLOG_ERROR("pptn_file_path open failed: {}", pptn_file_path);
        return false;
    }
    while (!ifs.eof())
    {
        std::string buffer;               
        buffer.resize(silly_pptn::SIZE_V1);  
        ifs.read(&buffer[0], silly_pptn::SIZE_V1);
        if (ifs.gcount() == 0)
        {
            break;  // 检查是否读取结束到达文件末尾
        }
        silly_pptn pptn;
        // 反序列化
        silly_pptn temp;
        if (temp.deserialize(buffer))
        {
            des_pptns.push_back(temp); 
        }
        else
        {
            std::cout << "Failed to deserialize an object from the data." << std::endl;
        }
    }
    ifs.close();
    
    // -----------index 找 stcd-------------
    size_t nindex = 0;
    for (auto& pptn : des_pptns)
    {
        uint32_t t_index = pptn.index;
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            if (!t_stcd.empty())
            {
                pptn.stcd = t_stcd;
            }
            else
            {
                nindex++;
            }
        }
    }
    SLOG_INFO("pptn not found stcd: {}", nindex);
    SLOG_INFO("pptn insert size: {}", des_pptns.size());

    //return true;// 临时添加
    // -----------数据插入-------------
    if (!otl.insert(insert_pptn_sql, [&des_pptns](otl_stream* stream) {
            int count = 0;
            for (const auto& entry : des_pptns)
            {
                if (entry.stcd.empty())
                {
                    continue;
                }
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

                otl_value<double> intv(entry.intv);
                otl_value<double> drp(entry.drp);

                otl_write_row(*stream, stcd, tm, drp, intv);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    SLOG_INFO("{} 导入完成, 导入数量: {}", pptn_file_path, des_pptns.size());
    
    return true;
}

bool import_river()
{
    if (index_stcd.empty())
    {
        return false;
    }

    std::vector<silly_river> des_rivers;  // 用于存储反序列化后的对象

    // -----------文件读取-------------
    std::ifstream ifs(river_file_path, std::ios::binary);  // 以二进制方式打开文件
    if (!ifs.is_open())
    {
        SLOG_ERROR("river_file_path open failed: {}", river_file_path);
        return false;
    }
    
    while (!ifs.eof())
    {
        std::string buffer;
        buffer.resize(silly_river::SIZE_V2);
        ifs.read(&buffer[0], silly_river::SIZE_V2);
        if (ifs.gcount() == 0)
        {
            break;  // 检查是否读取结束到达文件末尾
        }
        silly_river pptn;
        // 反序列化
        silly_river temp;
        if (temp.deserialize(buffer))
        {
            des_rivers.push_back(temp);
            //if (des_rivers.size() == 100)
            //{
            //    break;
            //}
        }
        else
        {
            std::cout << "Failed to deserialize an object from the data." << std::endl;
        }
    }
    ifs.close();

    // -----------index 找 stcd-------------
    for (auto& river : des_rivers)
    {
        uint32_t t_index = river.index;
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            if (!t_stcd.empty())
            {
                river.stcd = t_stcd;
            }
        }
    }
    SLOG_INFO("rivers insert size: {}", des_rivers.size());
    // -----------数据插入-------------
    //return true; // 临时添加
    int bi = 0,    ei = 0;
    int step = 5000;
    ei = SU_MIN(step, des_rivers.size());
    while (bi < des_rivers.size())
    {
        if (!otl.insert(insert_river_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = des_rivers[i];
                    otl_value<std::string> stcd(entry.stcd);
                    struct tm* timeinfo;
                    timeinfo = localtime(&entry.stamp);
                    otl_datetime tm;
                    tm.year = (timeinfo->tm_year + 1900);
                    tm.month = timeinfo->tm_mon + 1;
                    tm.day = timeinfo->tm_mday;
                    tm.hour = timeinfo->tm_hour;
                    tm.minute = timeinfo->tm_min;
                    tm.second = timeinfo->tm_sec;

                    otl_value<double> zz(entry.zz);
                    otl_value<double> qq(entry.qq);
                    otl_value<std::string> wptn(entry.wptn);

                    otl_write_row(*stream, stcd, tm, zz, qq, wptn);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return false;
        }
        SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, des_rivers.size());
    }
    

    SLOG_INFO("{} 导入完成, 导入数量: {}", river_file_path, des_rivers.size());

    return true;

}