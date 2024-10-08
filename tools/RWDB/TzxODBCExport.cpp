/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: TzxODBCExport.c
 * @description: RWDB数据使用ODBC导出到自定文件
 * @version: v1.0.1 2024-09-30 dou li yang
 */
#include <database/otl/silly_otl.h>
#include <database/otl/otl_tools.h>
#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_stbprp.h>
#include <tzx/rwdb/silly_rsvr.h>
#include <tzx/rwdb/silly_river.h>

void init(const std::string& file);
// 导出测站基本信息
void export_stbprp();
// 导出降雨记录
void export_pptn(const std::string& btm, const std::string& etm);
//// 导出水库水情记录
//void export_rsvr(const std::string& btm, const std::string& etm);
//// 导出河道水情记录
//void export_river(const std::string& btm, const std::string& etm);

// 全局变量
silly_otl otl;
std::string sql_select_stbprp = "select * from stbprp";
std::string sql_select_pptn = "SELECT STCD, TM, DRP, INTV FROM (SELECT STCD, TM, DRP, INTV FROM ST_PPTN_R WHERE TM > TO_DATE(:f1<char[32]>, 'YYYY-MM-DD HH24:MI:SS') AND TM <= TO_DATE(:f2<char[32]>, 'YYYY-MM-DD HH24:MI:SS')) WHERE ROWNUM <= 100;";
std::string sql_select_rsvr = "select * from rsvr";
std::string sql_select_river = "select * from river";
std::map<std::string, uint32_t> stcd_index;

int main(int argc, char** argv)
{
    init("./export.json");
    std::string r_odbc = (R"({
    "type": "oracle",
    "ip": "192.168.0.71",
    "port": 1521,
    "driver": "Oracle in instantclient_12_2",
    "schema": "ORCL",
    "user": "RWDB_HN",
    "password": "RWDB_HN"})");

    otl.load(r_odbc);
    std::string btm("2023-02-23 01:00:00"), etm("2023-03-24 13:00:00");

    export_pptn(btm, etm);
    export_stbprp();
    //export_rsvr(btm, etm);
    //export_river(btm, etm);
    return 0;
}

void init(const std::string& file)
{
    SLOG_ERROR("未实现")
    //exit(-1);
}

void export_stbprp()
{
    std::vector<silly_stbprp> stbprps;

    if (!otl.select(sql_select_stbprp, [&stbprps](otl_stream* stream) {
            while (!stream->eof())
            {
                // TODO : 完成
            }
    }))
    {
        SLOG_ERROR(otl.err())
    }
    std::ofstream ofs(silly_stbprp::FILE_NAME, std::ios::binary);
    for (auto& stbprp : stbprps)
    {
        ofs << stbprp.serialize();
    }
    ofs.close();
    SLOG_INFO("{} 导出完成", silly_stbprp::FILE_NAME)
}

void export_pptn(const std::string& btm, const std::string& etm)
{
    std::vector<silly_pptn> pptns;

    if (!otl.select(sql_select_pptn, [&pptns, &btm, &etm](otl_stream* stream) {
            otl_write_row(*stream, btm.c_str(), etm.c_str());  // 传入参数
            uint32_t index = 0;
            while (!stream->eof())
            {
                std::string stcd;
                double drp, intv;
                otl_datetime tm;
                otl_read_row(*stream, stcd, tm, intv, drp);
                std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
                std::time_t stamp = std::mktime(&t);
                silly_pptn pptn(stcd, stamp, intv, drp);
                pptn.index = index++;
                pptns.push_back(pptn);
            }
        }))
    {
        SLOG_ERROR(otl.err())
    }

    std::ofstream ofs(silly_pptn::FILE_NAME, std::ios::binary);
    for (auto& pptn : pptns)
    {
        ofs << pptn.serialize();
    }
    ofs.close();
    SLOG_INFO("{} 导出完成", silly_pptn::FILE_NAME);


    // TODO : 导出PPTN
    std::vector<silly_pptn> des_pptns;                  // 用于存储反序列化后的对象
    std::ifstream ifs(silly_pptn::FILE_NAME, std::ios::binary);  // 以二进制方式打开文件
    if (!ifs.is_open())
    {
        std::cout << "Failed to open file for reading." << std::endl;
        return;
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


}
void export_rsvr()
{
    SLOG_ERROR("未实现")
    exit(-1);
}
void export_river()
{
    SLOG_ERROR("未实现")
    exit(-1);
}