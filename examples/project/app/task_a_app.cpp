#pragma once
#include <project_marco.h>

int main(int argc, char **argv)
{
    if(!silly_log::instance().init(argc, argv)) {
        std::cerr << "初始化日志失败" << std::endl;
            return -1;
    }
    SLOG_INFO("正在启动任务A")

    return 0;
}
