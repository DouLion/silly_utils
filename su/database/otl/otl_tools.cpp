//
// Created by dell on 2023/5/17.
//

#include <boost/date_time/posix_time/posix_time.hpp>
#include "otl_tools.h"


OtlConnOption otl_tools::conn_opt_from_json(const Json::Value &root) {
    OtlConnOption retOpt;
    if (root.isMember(OTL_OPT_JSON_TYPE))
    {
        retOpt.type = GetType(root[OTL_OPT_JSON_TYPE].asString());
    }
    else
    {
        return retOpt;
    }

    if (root.isMember(OTL_OPT_JSON_IP))
    {
        retOpt.ip = root[OTL_OPT_JSON_IP].asString();
    }
    else
    {
        return retOpt;
    }
    if (root.isMember(OTL_OPT_JSON_PORT))
    {
        retOpt.port = root[OTL_OPT_JSON_PORT].asInt();
    }
    else
    {
        return retOpt;
    }
    if (root.isMember(OTL_OPT_JSON_DRIVER))
    {
        retOpt.driver = root[OTL_OPT_JSON_DRIVER].asString();
    }
    else
    {
        return retOpt;
    }
    if (root.isMember(OTL_OPT_JSON_SCHEMA))
    {
        retOpt.schema = root[OTL_OPT_JSON_SCHEMA].asString();
    }
    else
    {
        return retOpt;
    }
    if (root.isMember(OTL_OPT_JSON_USER))
    {
        retOpt.user = root[OTL_OPT_JSON_USER].asString();
    }
    else
    {
        return retOpt;
    }
    if (root.isMember(OTL_OPT_JSON_PASSWORD))
    {
        retOpt.password = root[OTL_OPT_JSON_PASSWORD].asString();
    }
    else
    {
        return retOpt;
    }

    if (root.isMember(OTL_OPT_JSON_DSN))
    {
        retOpt.dsn = root[OTL_OPT_JSON_DSN].asString();
    }

    return retOpt;
}

otl_datetime otl_tools::otl_time_from_string(const std::string &str) {
    otl_datetime odt;
    auto datetime = boost::posix_time::time_from_string(str);
    auto date = datetime.date();
    odt.year = date.year();
    odt.month = date.month();
    odt.day = date.day();
    auto time = datetime.time_of_day();
    odt.hour = time.hours();
    odt.minute = time.minutes();
    odt.second = time.seconds();
    odt.fraction = time.fractional_seconds();
    return odt;
}

std::string otl_tools::otl_time_to_string(const otl_datetime &tm) {
    char datetime_buff[32] = {};
    sprintf(datetime_buff, "%04d-%02d-%02d %02d:%02d", tm.year, tm.month, tm.day, tm.hour, tm.minute);

    return datetime_buff;
}
