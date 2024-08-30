#pragma once
#include <datetime/silly_posix_time.h>
#ifdef SILLY_UTILS_BOOST_ENABLE
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <su_marco.h>

namespace simple_time
{
typedef boost::posix_time::ptime PosixTime;
typedef boost::posix_time::time_duration TimeDuration;

static PosixTime NowPosixTime()
{
    return boost::posix_time::second_clock::local_time();
}
static PosixTime TimeFromString(const std::string& str)
{
    PosixTime _ptime;
    try
    {
        _ptime = boost::posix_time::time_from_string(str);
    }
    catch (const std::exception& e)
    {
        SU_ERROR_PRINT("%s", e.what())
    }
    return _ptime;
}

static std::string TimeToFormatString(const PosixTime& time, const std::string& format = DATE_FORMAT_5)
{
    std::string retStr;
    try
    {
        boost::posix_time::time_facet* tf = new boost::posix_time::time_facet(format.c_str());
        std::stringstream ss;
        ss.imbue(std::locale(std::cout.getloc(), tf));
        ss << time;
        retStr = ss.str();
    }
    catch (const std::exception& e)
    {
        SU_ERROR_PRINT("%s", e.what())
    }

    return retStr;
}
}  // namespace simple_time

using namespace simple_time;

#endif
