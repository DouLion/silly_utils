//
// Created by dell on 2024/3/1.
//

#include "silly_timer.h"

silly_timer::silly_timer()
{
    m_start = std::chrono::high_resolution_clock::now();
}

void silly_timer::restart()
{
    m_start = std::chrono::high_resolution_clock::now();
}

double silly_timer::elapsed_ms()
{
    auto tmp = std::chrono::high_resolution_clock::now();
    return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(tmp - m_start).count());
}
