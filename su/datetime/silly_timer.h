/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/3/1 18:19
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_TIMER_H
#define SILLY_UTILS_SILLY_TIMER_H
#include <su_marco.h>

class silly_timer
{
  public:
    /// <summary>
    /// 构造函数,初始化start,并且开始计时
    /// </summary>
    silly_timer();

    /// <summary>
    /// 重新开始计时
    /// </summary>
    void restart();

    /// <summary>
    /// 获取经过从开始计时经过的毫秒数
    /// </summary>
    /// <returns></returns>
    double elapsed_ms();

  private:
    std::chrono::time_point<std::chrono::system_clock> m_start;
    std::chrono::time_point<std::chrono::system_clock> m_end;
};
#define SILLY_TIMER_COST_MS(timer) std::cout << SU_FILE_NAME <<":"<<  __LINE__ << "\t" << timer.elapsed_ms() << " ms" << std::endl;
#define SILLY_TIMER_COST_SEC(timer) std::cout << SU_FILE_NAME <<":"<<  __LINE__ << "\t" << timer.elapsed_ms() / 1000 << " sec" << std::endl;
#endif  // SILLY_UTILS_SILLY_TIMER_H
