#pragma once
#include <log/silly_log.h>
#include <log/loguru/loguru.hpp>
#include <filesystem>
static char* SILLY_TZX_LOG_CHAR = " \n _________\n|  _   _  |               \n|_/ | | \\_|____   _   __  \n    | |   [_   ] [ \\ [  ] \n   _| |_   .' /_  > '  <  \n  |_____| [_____][__]`\\_]\n";
bool silly_log::init(silly_log_opt opt, enum_log_type type)
{
    SFP_TZX
    char* log_file = "./logs/tzx.log";
    if (!opt.path.empty())
    {
        log_file = (char*)(opt.path.c_str());
    }
    std::filesystem::create_directories(std::filesystem::path(log_file).parent_path());
    char* argv[2];
    argv[0] = log_file;
    argv[1] = nullptr;
    int argc = 1;
    switch (type)
    {
        case enum_log_type::eltLoguru:
            loguru::init(argc, argv);
            loguru::add_file(argv[0], loguru::Append, loguru::Verbosity_MAX);
            break;
        default:
            break;
    }
    return true;
}
