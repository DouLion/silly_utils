#include <iostream>
#include <database/otl/otl_tools.h>
#include <compress/bz2_wrapper.h>
#include <files/TFF_FileUtils.h>
#include <assert.h>
#include <bzlib.h>
#include "marco.h"
#include <triangular/bowyer_watson_delaunay.hpp>
//#include <vld.h>


struct ZAx
{
	int a;
	std::string c;
};
struct ZAe
{
	int a;
	std::string c;
	ZAx f;
};

int main()
{
	std::vector<delaunay::Point<double>> points;
	delaunay::triangulate(points);
	std::string decompress_path;//bz2_wrapper::decompress("E:/SL_CHINA_20230607_0000_060-1hourAccCOTREC.LatLon.bz2");
	std::filesystem::path tt("E:/SL_CHINA_20230607_0000_060-1hourAccCOTREC.LatLon.bz2");
	std::cout << tt.extension().string() << std::endl;
	std::string a = SU_VAR_NAME(decompress_path);

	ZAx fff;
	std::cout << SU_VAR_NAME(fff.f) << std::endl;
	std::cout << a << std::endl;
	std::cout << a << std::endl;
	return 0;
}