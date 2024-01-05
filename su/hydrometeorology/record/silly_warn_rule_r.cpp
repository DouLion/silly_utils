//
// Created by dell on 2023/12/15.
//

#include "silly_warn_rule_r.h"
#include "hydrometeorology/silly_record_tools.h"

bool sliiy_warn_rule_record::serialize(unsigned char** c_in, size_t& len)
{
    char buff[SILLY_WARN_RULE_R_LENGTH] = {0};
    buff[0] = SILLY_WARN_RULE_R_BEG_CHAR;
    char code_buff[SILLY_WARN_RULE_CODE_LENGTH] = {0};
    size_t i = 1;
    memcpy(buff + i, code, SILLY_WARN_RULE_CODE_LENGTH);
    unsigned int ymd = 0, hms = 0;
    if (!silly_record_tools::compress_time_format(atm, ymd, hms))
    {
        return false;
    }
    i += SILLY_WARN_RULE_CODE_LENGTH;
    SU_MEMCPY_AUTO_INC(buff, i, ymd);
    SU_MEMCPY_AUTO_INC(buff, i, hms);
    char c_intv = (char)(intv * 2);
    char c_grade = (char)grade;

    SU_MEMCPY_AUTO_INC(buff, i, c_intv);
    SU_MEMCPY_AUTO_INC(buff, i, c_grade);
    SU_MEMCPY_AUTO_INC(buff, i, drrp);
    SU_MEMCPY_AUTO_INC(buff, i, swc);
    SU_MEMCPY_AUTO_INC(buff, i, drp);

    *c_in = (unsigned char*)malloc(SILLY_WARN_RULE_R_LENGTH);
    if (!c_in)
    {
        return false;
    }
    memcpy(*c_in, buff, SILLY_WARN_RULE_R_LENGTH);
    len = SILLY_WARN_RULE_R_LENGTH;
    return true;
}

bool sliiy_warn_rule_record::unserialize(const unsigned char* c_in, const size_t& len)
{
    return false;
}

bool sliiy_warn_rule_record::operator==(const sliiy_warn_rule_record& other) const
{
    return false;
}

sliiy_warn_rule_record::sliiy_warn_rule_record(const std::string& atm, float intv, int grade, float swc, float drrp, float drp) : atm(atm), intv(intv), grade(grade), swc(swc), drrp(drrp), drp(drp)
{
}
