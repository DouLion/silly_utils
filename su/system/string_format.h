/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2022/9/2 17:23
 * @version: 1.0.1
 * @description:
 */
#pragma once
#ifndef _FORMAT_H_
#define _FORMAT_H_
#pragma once
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace TzxFormat
{
class ArgBase
{
  public:
    ArgBase()
    {
    }
    virtual ~ArgBase()
    {
    }
    virtual void Format(std::ostringstream& ss, const std::string& fmt) = 0;
};

template <class T>
class Arg : public ArgBase
{
  public:
    Arg(T arg) : m_arg(arg)
    {
    }
    virtual ~Arg()
    {
    }
    virtual void Format(std::ostringstream& ss, const std::string& fmt)
    {
        ss << m_arg;
    }

  private:
    T m_arg;
};

class ArgArray : public std::vector<ArgBase*>
{
  public:
    ArgArray()
    {
    }
    ~ArgArray()
    {
        std::for_each(begin(), end(), [](ArgBase* p) { delete p; });
    }
};

static void FormatItem(std::ostringstream& ss, const std::string& item, const ArgArray& args)
{
    int index = 0;
    int alignment = 0;
    std::string fmt;

    char* endptr = nullptr;
    index = strtol(&item[0], &endptr, 10);
    if (index < 0 || index >= args.size())
    {
        return;
    }

    if (*endptr == ',')
    {
        alignment = strtol(endptr + 1, &endptr, 10);
        if (alignment > 0)
        {
            ss << std::right << std::setw(alignment);
        }
        else if (alignment < 0)
        {
            ss << std::left << std::setw(-alignment);
        }
    }

    if (*endptr == ':')
    {
        fmt = endptr + 1;
    }

    args[index]->Format(ss, fmt);

    return;
}

template <class T>
static void Transfer(ArgArray& argArray, T t)
{
    argArray.push_back(new Arg<T>(t));
}

template <class T, typename... Args>
static void Transfer(ArgArray& argArray, T t, Args&&... args)
{
    Transfer(argArray, t);
    Transfer(argArray, args...);
}

template <typename... Args>
std::string Format(const std::string& format, Args&&... args)
{
    if (sizeof...(args) == 0)
    {
        return format;
    }

    ArgArray argArray;
    Transfer(argArray, args...);
    size_t start = 0;
    size_t pos = 0;
    std::ostringstream ss;
    while (true)
    {
        pos = format.find('{', start);
        if (pos == std::string::npos)
        {
            ss << format.substr(start);
            break;
        }

        ss << format.substr(start, pos - start);
        if (format[pos + 1] == '{')
        {
            ss << '{';
            start = pos + 2;
            continue;
        }

        start = pos + 1;
        pos = format.find('}', start);
        if (pos == std::string::npos)
        {
            ss << format.substr(start - 1);
            break;
        }

        FormatItem(ss, format.substr(start, pos - start), argArray);
        start = pos + 1;
    }

    return ss.str();
}
}  // namespace TzxFormat

using namespace TzxFormat;
#endif  //_FORMAT_H_
