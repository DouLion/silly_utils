//
// Created by dell on 2024/3/1.
//

#include "silly_timer.h"

suTimer::suTimer()
{
    m_start = std::chrono::system_clock::now();
}

void suTimer::restart()
{
    m_start = std::chrono::system_clock::now();
}

double suTimer::elapsed_ms() const
{
    const auto& tmp = std::chrono::system_clock::now();
    return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(tmp - m_start).count());
}
