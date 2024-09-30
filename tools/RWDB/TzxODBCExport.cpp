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
void export_stbprp();
void export_pptn(const std::string& btm, const std::string& etm);
void export_rsvr(const std::string& btm, const std::string& etm);
void export_river(const std::string& btm, const std::string& etm);

silly_otl otl;
std::string sql_select_stbprp = "select * from stbprp";
std::string sql_select_pptn = "select * from pptn";
std::string sql_select_rsvr = "select * from rsvr";
std::string sql_select_river = "select * from river";

int main(int argc, char** argv)
{
    init("./export.json");
    return 0;
}

void init(const std::string& file)
{
    SLOG_ERROR("未实现")
    exit(-1);
}
void export_stbprp();
void export_pptn(const std::string& btm, const std::string& etm)
{
    std::vector<silly_pptn> pptns;

    if (otl.select(sql_select_pptn, [&pptns](otl_stream* stream) {
            while (!stream->eof())
            {
                std::string stcd;
                double drp, intv;
                otl_datetime tm;
                otl_read_row(*stream, stcd, tm, intv, drp);
                std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month, tm.year};
                std::time_t stamp = std::mktime(&t);
                silly_pptn pptn(stcd, stamp, intv, drp);

                pptns.push_back(pptn);
            }
        }))
    {
        SLOG_ERROR(otl.err())
    }

    std::ofstream ofs("./pptn.bin", std::ios::binary);
    for (auto& pptn : pptns)
    {
        ofs << pptn.serialize();
    }
    ofs.close();
    SLOG_INFO("pptn.bin 导出完成")
}
void export_rsvr();
void export_river();