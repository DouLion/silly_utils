#pragma once 
#define BOOST_TEST_MAIN                 // 必须定义主测试套件，必须位于头文件之前
#define BOOST_TEST_STATIC_LINK

#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
using boost::unit_test_framework::test_suite;
#if IS_WIN32
#include <vld.h>
#endif

//#include "units/test_pyramid.hpp"
//#include "units/test_file.hpp"
//#include "units/test_geo.hpp"
//#include "units/test_compress.hpp"
//#include "units/test_system.hpp"
// #include "units/test_jpeg.hpp"
//#include "units/test_database.hpp"
//#include "units/test_triangular.h"
//#include "units/test_tcp.hpp"
//
//#include "units/test_compress.hpp"

#include "units/test_matrix.hpp"


