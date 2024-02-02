//
// Created by dell on 2024/2/2.
//

#include "silly_algorithm.h"

std::vector<std::string> silly_string_algo::split(const std::string &str, const char &c) {
    std::vector<std::string> results;
    size_t pos1 = 0;
    size_t pos0 = 0;
    size_t total = str.size();
    std::string n_str = str;

    while(pos1 < total)
    {
        if(n_str[pos1] == c)
        {
            std::string tmp = n_str.substr(pos0, pos1 - pos0);
            results.push_back(tmp);
            pos0 = pos1+1;
        }
        pos1++;
    }
    if(pos0 != total) {
        results.push_back(str.substr(pos0, total - pos0));
    }
    return results;
}

std::vector<std::string> silly_string_algo::split(const std::string &str, const std::string &s) {
    std::vector<std::string> results;
    size_t pos_s = s.size();
    size_t pos_s0 =0;
    size_t post_se = 0;
    size_t pos1 = 0;
    size_t pos0 = 0;
    size_t total = str.size() - pos_s;

    while(pos1 < total)
    {
        pos_s0 = 0;
        while(pos_s0 < pos_s && str[pos1 + pos_s0] == s[pos_s0])
        {

            pos_s0++;
        }
        if(pos_s0 == pos_s) // 完全匹配上
        {
            std::string tmp = str.substr(pos0, pos1 - pos0);
            results.push_back(tmp);
            pos1 = pos1 + pos_s;
            pos0 = pos1;
        }
        pos1++;
    }
    pos_s0 = 0;
    while(pos_s0 < pos_s && str[pos1 + pos_s0] == s[pos_s0])
    {
        pos_s0++;
    }
    if(pos_s0 == pos_s) // 完全匹配上
    {
        std::string tmp = str.substr(pos0, pos1 - pos0);
        results.push_back(tmp);
        pos1 = pos1 + pos_s;
        pos0 = pos1;
    }else
    {
        if(pos0 != pos1)
        {
            results.push_back(str.substr(pos0, str.size() - pos0));
        }
    }


    return results;
}

std::string silly_string_algo::replace(const std::string &src, const std::string &find, const std::string rep) {
    return std::string();
}
