/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/2 14:04
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_FORMAT_H
#define SILLY_UTILS_SILLY_FORMAT_H
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <fmt/os.h>
#include <fmt/color.h>

namespace silly_format = fmt;

/* 示例
 * */

/*#include <fmt/core.h>

int main() {
    fmt::print("Hello, world!\n");
}

std::string s = fmt::format("The answer is {}.", 42);
// s == "The answer is 42."

std::string s = fmt::format("I'd rather be {1} than {0}.", "right", "happy");
// s == "I'd rather be happy than right."

#include <fmt/chrono.h>

int main() {
    auto now = std::chrono::system_clock::now();
    fmt::print("Date and time: {}\n", now);
    fmt::print("Time: {:%H:%M}\n", now);
}

#include <vector>
#include <fmt/ranges.h>

int main() {
    std::vector<int> v = {1, 2, 3};
    fmt::print("{}\n", v);
}

#include <fmt/os.h>

int main() {
    auto out = fmt::output_file("guide.txt");
    out.print("Don't {}", "Panic");
}

#include <fmt/color.h>

int main() {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
               "Hello, {}!\n", "world");
    fmt::print(fg(fmt::color::floral_white) | bg(fmt::color::slate_gray) |
                   fmt::emphasis::underline, "Olá, {}!\n", "Mundo");
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "你好{}！\n", "世界");
}*/

#endif //SILLY_UTILS_SILLY_FORMAT_H
