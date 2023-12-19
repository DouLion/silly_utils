/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/15 11:35
 * @version: 1.0.1
 * @description: 动态指标记录
 */
#ifndef SILLY_UTILS_SILLY_WARN_RULE_R_H
#define SILLY_UTILS_SILLY_WARN_RULE_R_H
#include <iostream>

class sliiy_warn_rule_record
{
public:
	// 网格点ID
	unsigned int pid;
	// 分析时间
	std::string atm;
	// intv
	float intv;
	// 等级
	int grade;
	// 土壤含水率
	float swc;
	// 雨量阈值
	float drrp;
	// 备注和时间戳不记录




};

#endif //SILLY_UTILS_SILLY_WARN_RULE_R_H
