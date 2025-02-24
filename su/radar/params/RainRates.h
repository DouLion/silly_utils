/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-24
 * @file: RainRates.h
 * @description: RainRates 类声明
 * @version: v1.0.1 2025-02-24 dou li yang
 */
#ifndef SILLY_UTILS_RAINRATES_H
#define SILLY_UTILS_RAINRATES_H

class RainRates
{
  public:

    bool Read(const std::string& file);
    bool Write(const std::string& file);

  private:
    double RATE_snr_threshold = 3;
    bool RATE_apply_median_filter_to_DBZ = true;
    int RATE_DBZ_median_filter_len = 5;
    bool RATE_apply_median_filter_to_ZDR = true;
    int RATE_ZDR_median_filter_len = 5;
    double RATE_min_valid_rate = 0.1;
    double RATE_max_valid_rate = 250;
    double RATE_max_valid_dbz = 53;
    double RATE_brightband_dbz_correction = -10;
    double RATE_zh_aa = 16.2;
    double RATE_zh_bb = 0.229;
    double RATE_zh_aa_snow = 3.97772;
    double RATE_zh_bb_snow = 0.184498;
    double RATE_zzdr_aa = 14.95;
    double RATE_zzdr_bb = 0.249373;
    double RATE_zzdr_cc = -0.01;
    double RATE_kdp_aa = 25.768815;
    double RATE_kdp_bb = 0.801745;
    double RATE_kdpzdr_aa = 88.04587;
    double RATE_kdpzdr_bb = 0.712033;
    double RATE_kdpzdr_cc = -0.991849;
    double RATE_pid_rate_kdp_threshold = 0.3;
    double RATE_hybrid_dbz_threshold = 40;
    double RATE_hybrid_kdp_threshold = 0.3;
    double RATE_hybrid_zdr_threshold = 0.5;
    double RATE_hidro_dbz_threshold = 38;
    double RATE_hidro_kdp_threshold = 0.3;
    double RATE_hidro_zdr_threshold = 0.5;
    double RATE_bringi_dbz_threshold = 40;
    double RATE_bringi_kdp_threshold = 0.15;
    double RATE_bringi_zdr_threshold = 0.1;
    
};

#endif  // SILLY_UTILS_RAINRATES_H
