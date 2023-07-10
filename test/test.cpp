#include <iostream>
#include <otl/otl_tools.h>
#include <compress/bz2_wrapper.h>
#include <files/TFF_FileUtils.h>
#include <assert.h>
#include <bzlib.h>
#include <vld.h>

int main()
{

	std::string decompress_path = bz2_wrapper::decompress("E:/SL_CHINA_20230607_0000_060-1hourAccCOTREC.LatLon.bz2");
	return 0;
}