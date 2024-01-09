//
// Created by dell on 2024/1/3.实现
//

#include "silly_dll.h"
#include "encode/silly_encode.h"

SU_HANDLER silly_dll::load(const std::string& path)
{
    SU_HANDLER h_ret = nullptr;
#if IS_WIN32
#ifdef UNICODE
    std::wstring ws_path = silly_encode::cxx11_string_wstring(path);
    h_ret = LoadLibraryExW(ws_path.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
    h_ret = LoadLibraryA(path.c_str());
#endif
#else
    h_ret = dlopen(path.c_str(), RTLD_LAZY);
#endif
    return h_ret;
}

bool silly_dll::close(SU_HANDLER& handler)
{
    bool b_ret = false;
#if IS_WIN32
    b_ret = FreeLibrary(handler);
#else
    b_ret = dlclose(handler);
#endif
    return b_ret;
}

SU_FUNC_ADDR silly_dll::func(SU_HANDLER handler, const std::string& name)
{
    SU_FUNC_ADDR func_ret = nullptr;
    if (handler)
    {
#if IS_WIN32
        func_ret = (void*)GetProcAddress(handler, name.c_str());
#else
        func_ret =  dlsym(handler, name.c_str());
#endif
    }
    return func_ret;
}
