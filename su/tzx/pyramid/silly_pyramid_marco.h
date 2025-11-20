/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-11-20
 * @file: silly_pyramid_marco.h
 * @description: 金字塔文件的一些基本定义
 * @version: v1.0.1 2025-11-20 dou li yang
 */
#ifndef SILLY_PYRAMID_MARCO_H
#define SILLY_PYRAMID_MARCO_H
#include <su_marco.h>

// base
constexpr size_t SU_PYRAMID_HEAD_LEN = 4;       // 头长度
constexpr size_t SU_PYRAMID_VER_LEN = 4;   // 版本信息长度
constexpr size_t SU_PYRAMID_MAX_ZOOM = 25;  // 最大层级数
// index
constexpr size_t SU_PYRAMID_IDX_DATA_POS = 8;   // 记录data数据位置的数据大小
constexpr size_t SU_PYRAMID_IDX_DATA_LEN = 4;  // 记录data数据大小的数据的大小
constexpr size_t SU_PYRAMID_IDX_FIXED = 1024;
// info
const static size_t SU_PYRAMID_INF_SRC_LEN = 512;
const static size_t SU_PYRAMID_INF_PROJ_LEN = 1024;
const static size_t SU_PYRAMID_INF_BOUND_LEN = 128;
const static size_t SU_PYRAMID_INF_BT_LEN = 8;
const static size_t SU_PYRAMID_INF_ET_LEN = 8;
const static size_t SU_PYRAMID_INF_INTV_LEN = 4;
const static size_t SU_PYRAMID_INF_FMT_LEN = 12;
const static size_t SU_PYRAMID_INF_TOTAL_LEN = 5640;// 预留


constexpr char SU_PYRAMID_VERSION_1[SU_PYRAMID_VER_LEN] = {0x00, 0x01, 0x00, 0x00};  // 0x00010000;
constexpr char SU_PYRAMID_VERSION_2[SU_PYRAMID_VER_LEN] = {0x02, 0x00, 0x00, 0x00};  // 0x00020000;
constexpr char SU_PYRAMID_VERSION_11[SU_PYRAMID_VER_LEN] = {0x01, 0x00, 0x00, 0x00};  // 0x01000000;

const static std::string SU_PYRAMID_DAT_NAME = "TzxImage.data";
const static std::string SU_PYRAMID_INF_NAME = "TzxImage.info";
const static std::string SU_PYRAMID_IDX_NAME = "TzxImage.index";
const static std::string SU_PYRAMID_TXT_NAME = "TzxImage.fulltext";
// const static std::string SU_PYRAMID_DATA_NAME = "TzxImage.data";

#define PYRAMID_MATCH_VERSION(a, b) ((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2] && (a)[3] == (b)[3])


#endif  // SILLY_PYRAMID_MARCO_H
