/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-24
 * @file: RadxGrid.h
 * @description: RadxGrid 类声明
 * @version: v1.0.1 2025-02-24 dou li yang
 */
#ifndef SILLY_UTILS_RADXGRID_H
#define SILLY_UTILS_RADXGRID_H
#include <radar/params/RadxCommon.h>
namespace radar
{

typedef enum output_format_t
{
    CF_NETCDF = 0,
    ZEBRA_NETCDF = 1,
    MDV = 2,
    CEDRIC = 3
};

typedef enum interp_mode_t
{
    INTERP_MODE_CART = 0,
    INTERP_MODE_PPI = 1,
    INTERP_MODE_POLAR = 2,
    INTERP_MODE_CART_REORDER = 3,
    INTERP_MODE_CART_SAT = 4
};

typedef enum projection_t
{
    PROJ_LATLON = 0,
    PROJ_LAMBERT_CONF = 3,
    PROJ_MERCATOR = 4,
    PROJ_POLAR_STEREO = 5,
    PROJ_FLAT = 8,
    PROJ_OBLIQUE_STEREO = 12,
    PROJ_TRANS_MERCATOR = 15,
    PROJ_ALBERS = 16,
    PROJ_LAMBERT_AZIM = 17,
    PROJ_VERT_PERSP = 18
};

typedef enum interp_transform_t
{
    TRANSFORM_DB_TO_LINEAR = 0,
    TRANSFORM_DB_TO_LINEAR_AND_BACK = 1,
    TRANSFORM_LINEAR_TO_DB = 2,
    TRANSFORM_LINEAR_TO_DB_AND_BACK = 3
};

typedef enum logical_t
{
    LOGICAL_AND = 0,
    LOGICAL_OR = 1
};

typedef enum netcdf_style_t
{
    CLASSIC = 0,
    NC64BIT = 1,
    NETCDF4_CLASSIC = 2,
    NETCDF4 = 3
};

// struct typedefs

typedef struct grid_z_geom_t
{
    int nz;
    double minz;
    double dz;
};

typedef struct grid_xy_geom_t
{
    int nx = 0;
    int ny = 0;
    double minx = 0.0;
    double miny = 0.0;
    double dx = 0.0;
    double dy = 0.0;
};

typedef struct select_field_t
{
    std::string input_name;
    tdrp_bool_t process_this_field;
};

typedef struct transform_field_t
{
    std::string input_name;
    std::string output_name;
    std::string output_units;
    interp_transform_t transform;
};

typedef struct fold_field_t
{
    std::string input_name;
    tdrp_bool_t field_folds;
    tdrp_bool_t use_global_nyquist;
    double fold_limit_lower;
    double fold_limit_upper;
};

typedef struct discrete_field_t
{
    std::string input_name;
    tdrp_bool_t is_discrete;
};

typedef struct rename_field_t
{
    std::string input_name;
    std::string output_name;
};

typedef struct angle_fields_t
{
    std::string azimuth_field_name;
    std::string elevation_field_name;
    std::string alpha_field_name;
    std::string beta_field_name;
    std::string gamma_field_name;
};

typedef struct censoring_field_t
{
    std::string name;
    double min_valid_value;
    double max_valid_value;
    logical_t combination_method;
};

typedef struct bound_field_t
{
    std::string input_name;
    double min_value;
    double max_value;
};
class RadxGrid
{
  public:
    bool Read(const std::string &path);
    bool Write(const std::string &path);

    void InputDir(const std::string &dir);
    void OutputDir(const std::string &dir);

    void GridXYGeom(const double &lon, const double &lat, const double &radius, const double &delta);

    void AddSelectField(const select_field_t &field);

  private:
    debug_t debug = DEBUG_OFF;

    std::string input_dir = ".";

    mode_t mode = FILELIST;

    std::string output_dir;

    output_format_t output_format = CF_NETCDF;

    interp_mode_t interp_mode = INTERP_MODE_PPI;

    tdrp_bool_t use_nearest_neighbor = pFALSE;

    int min_nvalid_for_interp = 3;

    tdrp_bool_t use_fixed_angle_for_interpolation = pFALSE;

    tdrp_bool_t use_fixed_angle_for_data_limits = pTRUE;

    double beam_width_fraction_for_data_limit_extension = 0.5;

    grid_z_geom_t grid_z_geom = {1, 0.5, 0.5};

    tdrp_bool_t specify_individual_z_levels = pFALSE;

    std::vector<double> _z_level_array = {0.5, 1, 1.5, 2, 2.5, 3, 4, 5, 6, 7, 8, 9, 10};

    grid_xy_geom_t grid_xy_geom;

    tdrp_bool_t center_grid_on_radar = pTRUE;

    projection_t grid_projection = PROJ_LATLON;

    double grid_rotation = 0.0;

    double grid_origin_lat = 0.0;

    double grid_origin_lon = 0.0;

    tdrp_bool_t auto_remap_flat_to_latlon = pFALSE;

    double grid_lat1 = 0.0;

    double grid_lat2 = 0.0;

    double grid_central_scale = 1.0;

    double grid_tangent_lat = 0.0;

    double grid_tangent_lon = 0.0;

    tdrp_bool_t grid_pole_is_north = pTRUE;

    double grid_persp_radius = 35786.0;

    double grid_false_northing = 0.0;

    double grid_false_easting = 0.0;

    tdrp_bool_t grid_set_offset_origin = pFALSE;

    double grid_offset_origin_latitude = 0.0;

    double grid_offset_origin_longitude = 0.0;

    tdrp_bool_t select_fields = pTRUE;

    std::vector<select_field_t> _selected_fields;

    tdrp_bool_t transform_fields_for_interpolation = pFALSE;

    std::vector<transform_field_t> _transform_fields{{"DBZ", "DBZ", "dBZ", TRANSFORM_DB_TO_LINEAR_AND_BACK}};

    tdrp_bool_t set_fold_limits = pFALSE;

    std::vector<fold_field_t> _folded_fields = {{"VEL", pTRUE, pTRUE, -25, 25}};

    tdrp_bool_t override_nyquist;

    double nyquist_velocity;

    tdrp_bool_t set_discrete_fields;

    discrete_field_t *_discrete_fields;
    int discrete_fields_n;

    tdrp_bool_t rename_fields;

    rename_field_t *_renamed_fields;
    int renamed_fields_n;

    tdrp_bool_t output_angle_fields;

    angle_fields_t angle_fields;

    tdrp_bool_t output_range_field;

    std::string range_field_name;

    tdrp_bool_t output_height_field;

    std::string height_field_name;

    tdrp_bool_t output_coverage_field;

    std::string coverage_field_name;

    tdrp_bool_t output_time_field;

    std::string time_field_name;

    tdrp_bool_t interp_time_field;

    tdrp_bool_t output_debug_fields;

    tdrp_bool_t apply_censoring;

    censoring_field_t *_censoring_fields;
    int censoring_fields_n;

    int censoring_min_valid_run;

    tdrp_bool_t override_standard_pseudo_earth_radius;

    double pseudo_earth_radius_ratio;

    tdrp_bool_t aggregate_sweep_files_on_read;

    tdrp_bool_t ignore_idle_scan_mode_on_read;

    tdrp_bool_t remove_rays_with_antenna_transitions;

    int transition_nrays_margin;

    tdrp_bool_t remove_long_range_rays;

    tdrp_bool_t remove_short_range_rays;

    tdrp_bool_t trim_surveillance_sweeps_to_360deg;

    tdrp_bool_t override_fixed_angle_with_mean_measured_angle;

    tdrp_bool_t reorder_sweeps_by_ascending_angle;

    tdrp_bool_t compute_sweep_angles_from_vcp_tables;

    tdrp_bool_t apply_time_offset;

    double time_offset_secs;

    tdrp_bool_t set_max_range;

    double max_range_km;

    tdrp_bool_t set_elevation_angle_limits;

    double lower_elevation_angle_limit;

    double upper_elevation_angle_limit;

    tdrp_bool_t set_azimuth_angle_limits;

    double lower_azimuth_angle_limit;

    double upper_azimuth_angle_limit;

    tdrp_bool_t check_fixed_angle_error;

    double max_fixed_angle_error;

    tdrp_bool_t check_number_of_sweeps;

    double min_number_of_sweeps;

    tdrp_bool_t override_volume_number;

    int starting_volume_number;

    tdrp_bool_t autoincrement_volume_number;

    tdrp_bool_t override_radar_location;

    double radar_latitude_deg;

    double radar_longitude_deg;

    double radar_altitude_meters;

    tdrp_bool_t override_instrument_name;

    std::string instrument_name;

    tdrp_bool_t override_site_name;

    std::string site_name;

    tdrp_bool_t override_beam_width;

    double beam_width_deg_h;

    double beam_width_deg_v;

    tdrp_bool_t override_gate_geometry;

    double start_range_km;

    double gate_spacing_km;

    double azimuth_correction_deg;

    double elevation_correction_deg;

    tdrp_bool_t specify_output_filename;

    std::string output_filename;

    tdrp_bool_t name_file_from_start_time;

    std::string netcdf_file_prefix;

    std::string netcdf_file_suffix;

    tdrp_bool_t use_iso8601_filename_convention;

    tdrp_bool_t netcdf_compressed;

    int netcdf_compression_level;

    netcdf_style_t netcdf_style;

    tdrp_bool_t netcdf_include_latlon_arrays;

    tdrp_bool_t netcdf_output_mdv_attributes;

    tdrp_bool_t netcdf_output_mdv_chunks;

    std::string ncf_title;

    std::string ncf_institution;

    std::string ncf_references;

    std::string ncf_source;

    std::string ncf_history;

    std::string ncf_comment;

    std::string instance;

    int procmap_register_interval;

    tdrp_bool_t free_memory_between_files;

    tdrp_bool_t use_multiple_threads;

    int n_compute_threads;

    tdrp_bool_t sat_data_invert_in_range;

    tdrp_bool_t sat_data_set_range_geom_from_fields;

    tdrp_bool_t write_search_matrix_files;

    std::string search_matrix_dir;

    tdrp_bool_t identify_convective_stratiform_split;

    std::string conv_strat_dbz_field_name;

    double conv_strat_min_valid_height;

    double conv_strat_max_valid_height;

    double conv_strat_min_valid_dbz;

    double conv_strat_texture_radius_km;

    double conv_strat_min_valid_fraction_for_texture;

    double conv_strat_min_convectivity_for_convective;

    double conv_strat_max_convectivity_for_stratiform;

    int conv_strat_min_overlap_for_convective_clumps;

    tdrp_bool_t conv_strat_write_partition;

    tdrp_bool_t conv_strat_write_max_texture;

    tdrp_bool_t conv_strat_write_convective_dbz;

    tdrp_bool_t conv_strat_write_debug_fields;

    int reorder_npoints_search;

    double reorder_search_radius_km;

    tdrp_bool_t reorder_scale_search_radius_with_range;

    double reorder_nominal_range_for_search_radius_km;

    double reorder_z_search_ratio;

    tdrp_bool_t reorder_bound_grid_point_vertically;

    double reorder_min_valid_wt_ratio;

    int reorder_blocks_nrows;

    int reorder_blocks_ncols;

    int reorder_min_nvalid_for_interp;

    tdrp_bool_t reorder_weighted_interpolation;

    tdrp_bool_t bound_fields;

    bound_field_t *_bounded_fields;
    int bounded_fields_n;

    tdrp_bool_t use_echo_orientation;

    double synthetic_rhis_start_az;

    double synthetic_rhis_delta_az;

    std::string echo_orientation_dbz_field_name;

    int echo_orientation_n_points_sdev_h;

    int echo_orientation_n_points_sdev_v;

  private:
};

};  // namespace radar

#endif  // SILLY_UTILS_RADXGRID_H
