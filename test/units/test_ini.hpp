
#ifndef SILLY_UTILS_TEST_INI_HPP
#define SILLY_UTILS_TEST_INI_HPP

#define BOOST_TEST_INCLUDED

#include <boost/test/unit_test.hpp>
#include "ini/silly_boost_ini_parser.h"
#include <filesystem>

#include "ini/silly_boost_ini_parser.h"
#include "ini/silly_simple_ini_parser.h"

BOOST_AUTO_TEST_SUITE(TestFiles)



BOOST_AUTO_TEST_CASE(SIMPLE_INI)		// mmap文件读取
{
	std::cout << "\r\n\r\n****************" << "SIMPLE_INI" << "****************" << std::endl;

	std::filesystem::path ini(DEFAULT_SU_DATA_DIR);
	ini += "/ini/IsoInfo.Ini";

	std::string section_1 = "BASE";
	std::string section_2 = "DynamicLayer.dbsource";
	std::string section_3 = "SXWeather";

	std::string property_i = "size";
	std::string property_d = "showxt";
	std::string property_f = "showyt";
	std::string property_l = "image_format";
	std::string property_b = "use_rain_server";
	std::string property_str = "log4cxx_property_file";

	std::string property_str_2 = "boundary_db_conn_str";
	std::string property_i_2 = "boundary_db_type";
	std::string property_s_3 = "url";

	silly_simple_ini_parser su_simple_ini;

	su_simple_ini.load(ini.string());
	int i_v = su_simple_ini.read_int(section_1, property_i);
	bool b_v = su_simple_ini.read_bool(section_1, property_b);
	float f_v = su_simple_ini.read_float(section_1, property_f);
	double d_v = su_simple_ini.read_double(section_1, property_d);
	long l_v = su_simple_ini.read_long(section_1, property_l);
	std::string s_v = su_simple_ini.read(section_1, property_str);
	std::string s_v_2 = su_simple_ini.read(section_2, property_str_2);

	std::string section_4 = "abc.456";
	std::string property_s_4 = "def";

	su_simple_ini.write(section_4, property_s_4, "456789",";注释");
	su_simple_ini.save();


	int e = 0;
	int f = 0;
	int g = 0;

};



BOOST_AUTO_TEST_CASE(BOOST_INI)		// mmap文件读取
{
	std::cout << "\r\n\r\n****************" << "BOOST_INI" << "****************" << std::endl;
	
	std::filesystem::path ini(DEFAULT_SU_DATA_DIR);
	ini += "/ini/IsoInfo.Ini";



	//try
	//{
	//	boost::property_tree::ptree pt;
	//	boost::property_tree::ini_parser::read_ini(ini.string(), pt);
	//	for (const auto& section : pt) {
	//		std::cout << "[" << section.first << "]\n";
	//		for (const auto& key : section.second) {
	//			std::cout << key.first << " = " << key.second.get_value<std::string>() << "\n";
	//		}
	//	}
	//}
	//catch (const boost::property_tree::ini_parser_error& e)
	//{
	//	std::cout << "error: " << e.what() << std::endl;
	//}




	std::string section_1 = "BASE";
	std::string section_2 = "DynamicLayer.dbsource";
	std::string section_3 = "SXWeather";

	std::string property_i = "size";
	std::string property_d = "showxt";
	std::string property_f = "showyt";
	std::string property_l = "image_format";
	std::string property_b = "use_rain_server";
	std::string property_str = "log4cxx_property_file";

	std::string property_str_2 = "boundary_db_conn_str";
	std::string property_i_2 = "boundary_db_type";
	std::string property_s_3 = "url";

	silly_boost_ini_parser su_boot_ini;

	su_boot_ini.load(ini.string());
	int i_v = su_boot_ini.read_int(section_1, property_i);
	bool b_v = su_boot_ini.read_bool(section_1, property_b);
	float f_v = su_boot_ini.read_float(section_1, property_f);
	double d_v = su_boot_ini.read_double(section_1, property_d);
	long l_v = su_boot_ini.read_long(section_1, property_l);
	std::string s_v = su_boot_ini.read(section_1, property_str);
	std::string s_v_2 = su_boot_ini.read(section_2, property_str_2);

	std::string section_4 = "abc.123";
	std::string property_s_4 = "def";

	su_boot_ini.write(section_4, property_s_4, "abcd");
	su_boot_ini.save();


	int e = 0;
	int f = 0;
	int g = 0;

};




















BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_INI_HPP

