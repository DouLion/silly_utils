//
// Created by dell on 2023/7/10.
//

#include "netcdf_utils.h"
#include <netcdf>


using namespace netCDF;
using namespace netCDF::exceptions;

bool netcdf_utils::read_netcdf(const std::string& path, const std::string& group, std::map<int, cv::Mat>& data, nc_info& info)
{
	// TODO: 这里不应该假定数据的类型
	float *nc_val_data = nullptr;
	float *nc_lon_data = nullptr;
	float *nc_lat_data = nullptr;
	bool ret_status = false;

	try {
		NcFile nc_file;
		nc_file.open(path, NcFile::read);
		NcGroup nc_group = nc_file.getGroup("/", NcGroup::GroupLocation::AllGrps);

		auto allVars = nc_group.getVars(netCDF::NcGroup::All);
		NcVar value_var = nc_group.getVar(group);

		std::vector<NcDim> ncDimVector = value_var.getDims();
		/*for (auto& nd : ncDimVector)
		{
			std::cout << nd.getName() << std::endl;
		}
		if (ncDimVector.size() != 3) {
			return false;
		}*/

		std::string time_name, lan_name, lon_name;
		size_t time_len;
		if (ncDimVector.size() == 2)
		{
			lan_name = ncDimVector[0].getName();
			lon_name = ncDimVector[1].getName();
			info.geo.y_size = ncDimVector[0].getSize();
			info.geo.x_size = ncDimVector[1].getSize();
		}
		if (ncDimVector.size() == 3)
		{
			time_name = ncDimVector[0].getName();
			lan_name = ncDimVector[1].getName();
			lon_name = ncDimVector[2].getName();
			time_len = ncDimVector[0].getSize();
			info.geo.y_size = ncDimVector[1].getSize();
			info.geo.x_size = ncDimVector[2].getSize();
		}
		if (ncDimVector.size() == 4)
		{
			time_name = ncDimVector[0].getName();
			lan_name = ncDimVector[2].getName();
			lon_name = ncDimVector[3].getName();
			time_len = ncDimVector[0].getSize();
			info.geo.y_size = ncDimVector[2].getSize();
			info.geo.x_size = ncDimVector[3].getSize();
		}
		float addOffset = 0;
		float scaleFactor = 1.0;
		float invalid_data = 0;
		std::map<std::string, NcVarAtt> extra_vars = value_var.getAtts();
		for (auto [key_, attr_] : extra_vars)
		{
			if (key_ == "_FillValue")
			{
				attr_.getValues(&invalid_data);
			}
			else if (key_ == "add_offset")
			{
				attr_.getValues(&addOffset);
			}
			else if (key_ == "scale_factor")
			{
				attr_.getValues(&scaleFactor);
			}
		}
		
		NcVar time_var, lat_var, lon_var;
		lat_var = nc_group.getVar(lan_name);
		if (lat_var.isNull()) {
			return false;
		}
		lon_var = nc_group.getVar(lon_name);
		if (lon_var.isNull()) {
			return false;
		}
		nc_val_data = (float *) malloc(time_len * info.geo.y_size * info.geo.x_size * sizeof(float));
		nc_lon_data = (float *) malloc(info.geo.x_size * sizeof(float));
		nc_lat_data = (float *) malloc(info.geo.y_size * sizeof(float));

		lon_var.getVar(nc_lon_data);
		lat_var.getVar(nc_lat_data);

		for (auto j = 0; j < info.geo.y_size; ++j) {
			info.geo.rect.g_bottom = std::min(info.geo.rect.g_bottom, (double)nc_lat_data[j]);
			info.geo.rect.g_top = std::max(info.geo.rect.g_top, (double)nc_lat_data[j]);
		}
		for (auto i = 0; i < info.geo.x_size; ++i) {
			info.geo.rect.g_left = std::min(info.geo.rect.g_left, (double)nc_lon_data[i]);
			info.geo.rect.g_right = std::max(info.geo.rect.g_right, (double)nc_lon_data[i]);
		}
		info.geo.x_delta = (info.geo.rect.g_right - info.geo.rect.g_left) / info.geo.x_size;
		info.geo.y_delta = (info.geo.rect.g_top - info.geo.rect.g_bottom) / info.geo.y_size;

		value_var.getVar(nc_val_data);
		for (int lvl = 0; lvl < time_len; lvl++) {
			cv::Mat mat_data(info.geo.x_size, info.geo.y_size, CV_32FC1);
			size_t lvlStart = lvl * info.geo.y_size * info.geo.x_size;
			for (int lat = 0; lat < info.geo.y_size; lat++) {
				for (int lon = 0; lon < info.geo.x_size; lon++) {
					if (invalid_data == nc_val_data[lvlStart + (info.geo.x_size - lon - 1) * info.geo.y_size + lat])
					{
						mat_data.ptr<float>(lon, lat)[0] = 0;
					}
					else
					{
						mat_data.ptr<float>(lon, lat)[0] = nc_val_data[lvlStart + (info.geo.x_size - lon - 1) * info.geo.y_size + lat] * scaleFactor + addOffset;
					}
					
				}
			}
			data.insert({lvl, mat_data});

		}
		ret_status = true;

	}
	catch (NcException &e) {
		std::cout << "FAILURE**************************\n";
		std::cout << e.what() << std::endl;
	}
	if (nc_val_data) {
		free(nc_val_data);
		nc_val_data = nullptr;
	}
	if (nc_lon_data) {
		free(nc_lon_data);
		nc_lon_data = nullptr;
	}
	if (nc_lat_data) {
		free(nc_lat_data);
		nc_lat_data = nullptr;
	}

	return ret_status;
}

bool netcdf_utils::write_netcdf(const std::string& path, const nc_info& info, const std::string& name, std::map<int, cv::Mat>& data)
{
	bool status = false;
	try
	{

		// Create the file. The Replace parameter tells netCDF to overwrite
		// this file, if it already exists.
		NcFile sfc(path, NcFile::replace);

		// Define the dimensions. NetCDF will hand back an ncDim object for
		// each.
		// NcDim timeDim = sfc.addDim(info.dims[0].name, info.dims[2].size);
		NcDim latDim = sfc.addDim(info.dims[0].name, info.dims[0].size);
		NcDim lonDim = sfc.addDim(info.dims[1].name, info.dims[1].size);
		float* lats = (float*)malloc(info.dims[0].size*sizeof(float));
		float* lons = (float*)malloc(info.dims[1].size * sizeof(float));
		for (int i = 0; i < info.dims[0].size; ++i)
		{
			lats[i] = (float)info.geo.rect.g_bottom + i * info.geo.y_delta;
		}
		for (int i = 0; i < info.dims[1].size; ++i)
		{
			lons[i] = (float)info.geo.rect.g_left + i * info.geo.x_delta;
		}



		// Define coordinate netCDF variables. They will hold the
		// coordinate information, that is, the latitudes and
		// longitudes. An pointer to a NcVar object is returned for
		// each.
		NcVar latVar = sfc.addVar(info.dims[0].name, ncFloat, latDim);//creates variable
		NcVar lonVar = sfc.addVar(info.dims[1].name, ncFloat, lonDim);

		// Write the coordinate variable data. This will put the latitudes
		// and longitudes of our data grid into the netCDF file.
		latVar.putVar(lats);
		lonVar.putVar(lons);

		// Define units attributes for coordinate vars. This attaches a
		// text attribute to each of the coordinate variables, containing
		// the units. Note that we are not writing a trailing NULL, just
		// "units", because the reading program may be fortran which does
		// not use null-terminated strings. In general it is up to the
		// reading C program to ensure that it puts null-terminators on
		// strings where necessary.
		for (auto [attr_name, attr_desc]: info.dims[0].attributes)
		{
			lonVar.putAtt(attr_name, attr_desc);
		}

		for (auto [attr_name, attr_desc] : info.dims[1].attributes)
		{
			latVar.putAtt(attr_name, attr_desc);
		}



		// Define the netCDF data variables.
		std::vector<NcDim> dims;
		dims.push_back(latDim);
		dims.push_back(lonDim);
		NcVar presVar = sfc.addVar(name, ncFloat, dims);
		// NcVar tempVar = sfc.addVar(TEMP_NAME, ncFloat, dims);

		// Define units attributes for vars.
		int total_size = info.dims[0].size * info.dims[1].size;
		presVar.putAtt("units", "mm");
		float* rains = (float*)malloc(total_size* sizeof(float));
		// tempVar.putAtt(UNITS, "celsius");

		// Write the pretend data. This will write our surface pressure and
		// surface temperature data. The arrays of data are the same size
		// as the netCDF variables we have defined.
		int curr_idx = 0;
		for (auto [idx, mat]: data)
		{
			for (int r = 0; r < mat.rows; ++r)
			{
				for (int c = 0; c < mat.cols; ++c)
				{
					rains[curr_idx + r * mat.cols + c] = mat.ptr<float>(mat.rows - r -1, c)[0];
				}
			}
		}
		presVar.putVar(rains);
		free(lats);
		free(lons);
		free(rains);
		//tempVar.putVar(tempOut);

		// The file is automatically closed by the destructor. This frees
		// up any internal netCDF resources associated with the file, and
		// flushes any buffers.

		//cout << "*** SUCCESS writing example file " << FILE_NAME << "!" << endl;
		status = true;
	}
	catch (NcException& e)
	{
		std::cout << "FAILURE**************************\n";
		std::cout << e.what() << std::endl;
		// return NC_ERR;
	}
	return status;
}

bool netcdf_utils::write_netcdf(const std::string& path, const nc_info& info, const std::string& name, cv::Mat data)
{
	bool status = false;
	try
	{

		// Create the file. The Replace parameter tells netCDF to overwrite
		// this file, if it already exists.
		NcFile sfc(path, NcFile::replace);

		// Define the dimensions. NetCDF will hand back an ncDim object for
		// each.
		// NcDim timeDim = sfc.addDim(info.dims[0].name, info.dims[2].size);
		NcDim latDim = sfc.addDim(info.dims[0].name, info.dims[0].size);
		NcDim lonDim = sfc.addDim(info.dims[1].name, info.dims[1].size);
		float* lats = (float*)malloc(info.dims[0].size*sizeof(float));
		float* lons = (float*)malloc(info.dims[1].size * sizeof(float));
		for (int i = 0; i < info.dims[0].size; ++i)
		{
			lats[i] = (float)info.geo.rect.g_bottom + i * info.geo.y_delta;
		}
		for (int i = 0; i < info.dims[1].size; ++i)
		{
			lons[i] = (float)info.geo.rect.g_left + i * info.geo.x_delta;
		}



		// Define coordinate netCDF variables. They will hold the
		// coordinate information, that is, the latitudes and
		// longitudes. An pointer to a NcVar object is returned for
		// each.
		NcVar latVar = sfc.addVar(info.dims[0].name, ncFloat, latDim);//creates variable
		NcVar lonVar = sfc.addVar(info.dims[1].name, ncFloat, lonDim);

		// Write the coordinate variable data. This will put the latitudes
		// and longitudes of our data grid into the netCDF file.
		latVar.putVar(lats);
		lonVar.putVar(lons);


		for (auto [attr_name, attr_desc]: info.dims[0].attributes)
		{
			lonVar.putAtt(attr_name, attr_desc);
		}

		for (auto [attr_name, attr_desc] : info.dims[1].attributes)
		{
			latVar.putAtt(attr_name, attr_desc);
		}

		// Define the netCDF data variables.
		std::vector<NcDim> dims;
		dims.push_back(latDim);
		dims.push_back(lonDim);
		NcVar presVar = sfc.addVar(name, ncFloat, dims);
		// NcVar tempVar = sfc.addVar(TEMP_NAME, ncFloat, dims);

		// Define units attributes for vars.
		int total_size = info.dims[0].size * info.dims[1].size;
		presVar.putAtt("units", "mm/h");
		float* rains = (float*)malloc(total_size* sizeof(float));
		// tempVar.putAtt(UNITS, "celsius");


		int curr_idx = 0;
		for (int r = 0; r < data.rows; ++r)
		{
			for (int c = 0; c < data.cols; ++c)
			{
				rains[curr_idx + r * data.cols + c] = data.ptr<float>(data.rows - r -1, c)[0];
			}
		}
		presVar.putVar(rains);
		free(lats);
		free(lons);
		free(rains);
		//tempVar.putVar(tempOut);

		status = true;
	}
	catch (NcException& e)
	{
		std::cout << "FAILURE**************************\n";
		std::cout << e.what() << std::endl;
		// return NC_ERR;
	}
	return status;
}
