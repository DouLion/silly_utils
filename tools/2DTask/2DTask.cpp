/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-15
 * @file: 2DTask.c
 * @description: 2DTask实现
 * @version: v1.0.1 2024-10-15 dou li yang
 */
#pragma once

#include <log/silly_log.h>
#include <files/silly_file.h>
#include <network/websocket/silly_websocket_client.h>
#include <network/http/silly_http_client.h>
std::string port = "9001";
std::string root = "./";
std::string proj;
std::string plan;
// 计算项目根目录
std::string svrr;

const static std::string WAIT = "Wait";
const static std::string START = "Model initialized";
const static std::string FINISH = "Model initialized and simulation complete";
const static std::string EXTENSION = ".tzx_task";
#define SLEEP_SECONDS(sec)  std::this_thread::sleep_for(std::chrono::seconds(sec))

void message_action(const std::string& msg);
void latest_task();
void remove_task();

int main(int argc, char** argv)
{
    if (!silly_log::instance().init(argc, argv))
    {
        std::cerr << "日志模块初始化失败" << std::endl;
        return -1;
    }
    std::string url = "ws://127.0.0.1:";
    url.append(port);

    while(1)
    {
        silly_http_client http;
        if(!http.get("192.168.0.9:3000/server/path", svrr) || svrr.empty())
        {
            SLOG_ERROR("获取运行根目录失败")
            SLEEP_SECONDS(5);
            continue;
        }
        SLOG_INFO("运行根目录: {}", svrr)
        silly_websocket_client wsc;
        wsc.on_receive(message_action);

        if (!wsc.connect(url))
        {
            SLOG_ERROR("连接失败: {}", url)
            SLOG_ERROR("错误信息:\n {}", wsc.err())
            SLEEP_SECONDS(10);
            continue;
        }
        std::thread t(&silly_websocket_client::run, &wsc);
        t.detach();
        while (1) // 改为ws是否断开连接
        {
            if(plan.empty() && proj.empty())
            {
                latest_task();
                if(!plan.empty() && !proj.empty())
                {
                    std::string msg = svrr;
                    msg.append(",./").append(proj).append("/cfg/").append(plan + ".cfg");
                    SLOG_INFO("发送任务信息: {}", msg)
                    //wsc.send("");
                }
                else{
                    SLOG_INFO("未检查到新任务")
                }
            }


          SLEEP_SECONDS(10);
        }

        SLEEP_SECONDS(5);
    }


    return 0;
}



void message_action(const std::string& msg)
{
    SLOG_INFO("接受到信息: {}", msg);
    if (msg == WAIT)
    {
        SLOG_INFO("系统忙,稍后计算: {} {}", proj, plan)
    }
    else if (msg == START)
    {
        SLOG_INFO("开始计算: {} {}", proj, plan)
    }
    else if (msg == FINISH)
    {
        remove_task();
    }
}
void latest_task()
{
    plan = "";
    proj = "";
    std::map<size_t, std::string> time_filename;
    auto sfp_p = std::filesystem::path(root);
    if (!std::filesystem::exists(sfp_p))
    {
        return;
    }
    for(auto it: std::filesystem::directory_iterator(sfp_p))
    {
        if(it.is_directory())
        {
            continue;
        }
        if(it.path().extension() != EXTENSION)
        {
            continue;
        }
        size_t tm = silly_file::last_modify_stamp_ms(it.path().string());
        std::string filename = it.path().filename().stem().string();
        time_filename[tm] = filename;
    }
    if(time_filename.empty())
    {
        return;
    }
    std::string filename = time_filename.rbegin()->second;
    size_t pos = filename.find(".");
    if(pos == std::string::npos)
    {
        return;
    }
    proj = filename.substr(0, pos);
    plan = filename.substr(proj.size()+1);
    SLOG_INFO("检查到新任务: {} {}", proj, plan)

}

void remove_task()
{
    SLOG_INFO("计算完成, 移除任务: {} {}", proj, plan);
    std::string filename = proj + "_"+plan + EXTENSION;
    try{
        std::filesystem::remove(std::filesystem::path(root).append(filename));
    }
    catch(...)
    {
        SLOG_ERROR("移除任务失败: {}", filename);
    }

    plan = "";
    proj = "";
}