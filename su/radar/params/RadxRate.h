/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-24
 * @file: RadxRate.h
 * @description: RadxRate 类声明
 * @version: v1.0.1 2025-02-24 dou li yang
 */
#ifndef SILLY_UTILS_RADXRATE_H
#define SILLY_UTILS_RADXRATE_H
#include <radar/params/RadxCommon.h>
class RadxRate
{

  /*public:

    bool Read(const std::string& file);
    bool Write(const std::string& file);
  private:
    std::string debug = "DEBUG_OFF";
    int n_compute_threads = 4;
    std::string mode = "FILELIST";
    std::string input_dir = ".";
    std::string input_file_search_ext = "";
    std::string input_file_search_substr = "";
    std::string start_time = "2015 06 26 00 00 00";
    std::string end_time = "2015 06 26 12 00 00";
    bool SNR_available = true;
    std::string SNR_field_name = "SNR";
    double noise_dbz_at_100km = 0;
    std::string DBZ_field_name = "DBZ";
    std::string ZDR_field_name = "ZDR";
    std::string PHIDP_field_name = "PHIDP";
    std::string RHOHV_field_name = "RHOHV";
    bool LDR_available = false;
    std::string LDR_field_name = "LDR";
    std::string KDP_params_file_path = "/usr/local/tzx_pro_2023/svrs/TzxRadar/param/HN1/RadxKdp.new";
    std::string PID_params_file_path = "/usr/local/tzx_pro_2023/svrs/TzxRadar/param/HN1/RadxPid.new";
    bool PID_use_KDP_self_consistency = false;
    bool PID_use_attenuation_corrected_fields = false;
    std::string RATE_params_file_path = "/usr/local/tzx_pro_2023/svrs/TzxRadar/param/HN1/rain_rates.new";
    bool RATE_use_KDP_self_consistency = true;
    bool RATE_use_attenuation_corrected_fields = false;
    std::vector<OutputFiled> output_fields = {{"RATE_ZH", "RATE_ZH", "precip_rate_from_z", "precip_rate_from_z", "mm/hr", true},
                                              {"RATE_ZH_SNOW", "RATE_ZH_SNOW", "precip_rate_from_z_for_snow", "precip_rate_from_z_for_snow", "mm/hr", true},
                                              {"RATE_Z_ZDR", "RATE_Z_ZDR", "precip_rate_from_z_and_zdr", "precip_rate_from_z_and_zdr", "mm/hr", true},
                                              {"RATE_KDP", "RATE_KDP", "precip_rate_from_kdp", "precip_rate_from_kdp", "mm/hr", true},
                                              {"RATE_KDP_ZDR", "RATE_KDP_ZDR", "precip_rate_from_kdp_and_zdr", "precip_rate_from_kdp_and_zdr", "mm/hr", true},
                                              {"RATE_HYBRID", "RATE_HYBRID", "precip_rate_hybrid_of_zh_zzdr_kdp_and_kdpzdr", "precip_rate_hybrid_of_zh_zzdr_kdp_and_kdpzdr", "mm/hr", true},
                                              {"RATE_PID", "RATE_PID", "precip_rate_based_on_pid", "precip_rate_based_on_pid", "mm/hr", true},
                                              {"PID", "PID", "particle_id", "hydrometeor_type", "", true},
                                              {"PID_INTEREST", "PID_INTEREST", "final_interest_value_for_pid_decision", "final_interest_value_for_pid_decision", "", false},
                                              {"TEMP_FOR_PID", "TEMP_FOR_PID", "temperature_for_computing_pid", "temperature", "C", false},
                                              {"KDP", "KDP", "specific_differential_phase", "specific_differential_phase_hv", "deg/km", true},
                                              {"KDP_SC", "KDP_SC", "kdp_conditioned_using_ZZDR_self_consistency", "specific_differential_phase_hv", "deg/km", false},
                                              {"ZDR_ATTEN_CORRECTION", "ZDR_ATTEN_CORRECTION", "correction_to_zdr_for_attenuation", "zdr_attenuation_correction", "dB", false},
                                              {"DBZ_ATTEN_CORRECTED", "DBZ_ATTEN_CORRECTED", "dbz_corrected_for_attenuation", "dbz_corrected_for_attenuation", "dBZ", true},
                                              {"ZDR_ATTEN_CORRECTED", "ZDR_ATTEN_CORRECTED", "zdr_corrected_for_attenuation", "zdr_corrected_for_attenuation", "dB", true}};
    std::string output_encoding = "OUTPUT_ENCODING_INT16";
    bool copy_selected_input_fields_to_output = true;
    std::vector<CopyField> copy_fields = {{"DBZ", "DBZ", false}, {"REF", "REF", false}, {"VEL", "VEL", false}, {"SW", "WIDTH", false}, {"ZDR", "ZDR", false}, {"PHI", "PHIDP", false}, {"RHO", "RHOHV", false}};
    bool KDP_write_debug_fields = false;
    bool PID_write_debug_fields = false;
    std::string output_dir = "/usr/local/tzx_pro_2023/svrs/TzxRadar/wp/PCALC/HN1/rate";
    std::string output_filename_mode = START_AND_END_TIMES;
    bool append_day_dir_to_output_dir = true;
    bool append_year_dir_to_output_dir = false;
    std::string output_format = OUTPUT_FORMAT_CFRADIAL;
    bool override_standard_pseudo_earth_radius = false;
    double pseudo_earth_radius_ratio = 1.33333;
    std::string instance = "test";
    int procmap_register_interval = 60;
    int max_realtime_data_age_secs = 300;*/
};

#endif  // SILLY_UTILS_RADXRATE_H
