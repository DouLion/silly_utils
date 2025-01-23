/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/9/11 14:36
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once
#include <geo/silly_geo.h>

#ifndef SILLY_UTILS_JVC_STRUCT_H
#define SILLY_UTILS_JVC_STRUCT_H

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cfloat>

#include <cassert>

#ifndef double_TYPE_EPSILON
#define double_TYPE_EPSILON FLT_EPSILON
#endif

#ifndef JCV_ATAN2
#define JCV_ATAN2(_Y_, _X_) atan2f(_Y_, _X_)
#endif

#ifndef JCV_SQRT
#define JCV_SQRT(_X_) sqrtf(_X_)
#endif

#ifndef JCV_PI
#define JCV_PI 3.14159265358979323846264338327950288f
#endif

#ifndef JCV_FLT_MAX
#define JCV_FLT_MAX 3.402823466e+38F
#endif

#ifndef JCV_EDGE_INTERSECT_THRESHOLD
// Fix for Issue #40
#define JCV_EDGE_INTERSECT_THRESHOLD 1.0e-10F
#endif

typedef struct jcv_site_ jcv_site;
typedef struct jcv_edge_ jcv_edge;
typedef struct jcv_graphedge_ jcv_graphedge;
typedef struct jcv_delauney_edge_ jcv_delauney_edge;
typedef struct jcv_delauney_iter_ jcv_delauney_iter;
typedef struct jcv_diagram_ jcv_diagram;
typedef struct jcv_context_internal_ jcv_context_internal;


#pragma pack(push, 1)

struct jcv_graphedge_
{
    struct jcv_graphedge_* next;
    struct jcv_edge_* edge;
    struct jcv_site_* neighbor;
    silly_point pos[2];
    double angle;
};
typedef struct jcv_site_ jcv_site;
struct jcv_site_
{
    silly_point p;
    int index;             // Index into the original list of points
    jcv_graphedge* edges;  // The half edges owned by the cell
};

// The coefficients a, b and c are from the general line equation: ax * by + c = 0
struct jcv_edge_
{
    struct jcv_edge_* next;
    jcv_site* sites[2];
    silly_point pos[2];
    double a;
    double b;
    double c;
};

struct jcv_delauney_iter_
{
    const jcv_edge* sentinel;
    const jcv_edge* current;
};

struct jcv_delauney_edge_
{
    const jcv_edge* edge;
    const jcv_site* sites[2];
    silly_point pos[2];
};

struct jcv_rect_
{
    silly_point min;
    silly_point max;
};

struct jcv_diagram_
{
    jcv_context_internal* internal;
    int numsites;
    silly_point min;
    silly_point max;
};

#pragma pack(pop)

#endif  // SILLY_UTILS_JVC_STRUCT_H
