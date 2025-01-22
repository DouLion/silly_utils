/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-22
 * @file: TzxNTP.c
 * @description: TzxNTP实现
 * @version: v1.0.1 2025-01-22 dou li yang
 */

#include <network/http/silly_http_client.h>
#include <json/silly_jsonpp.h>
#include <datetime/silly_posix_time.h>
using namespace silly;

int main(int argc, char** argv)
{
    silly_log::instance().init(argc, argv);

    while (1)
    {
        http::client cli;
        std::string resp;
        if (cli.get("10.43.1.205:8080/sys/time", resp))
        //resp = R"({"data":{"t":1737517303367,"currentTime":"22/Jan/2025:11:41:43 +0800"}})";
        {
            Json::Value root = jsonpp::loads(resp);
            Json::Value jvdata;
            SLOG_INFO(resp)
            if (jsonpp::check_member_object(root, "data", jvdata))
            {
                std::string strt = jvdata["t"].asString();
                time_t t = static_cast<time_t>(std::stod(strt.c_str()) / 1000);

                silly_posix_time ptm(t);
                std::string datetime = ptm.to_string();
                SLOG_INFO(datetime)

#if WIN32
                std::string date = datetime.substr(0, 10);
                std::string time = datetime.substr(11, 8);
                std::string cmd1 = "date";
                std::string cmd2 = "time";
                cmd1.append(" ").append(date);
                cmd2.append(" ").append(time);
                system(cmd1.c_str());
                system(cmd2.c_str());
#else

                std::string cmd = "date --set=\"";
                cmd.append(datetime).append("\"");
                //std::cout << cmd << std::endl;
                system(cmd.c_str());

#endif
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}