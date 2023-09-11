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

#ifndef SILLY_UTILS_JVC_STRUCT_H
#define SILLY_UTILS_JVC_STRUCT_H

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cfloat>

#include <cassert>

#ifdef USING_DOUBLE
#ifndef JCV_REAL_TYPE
#define JCV_REAL_TYPE double
#endif
#else
#ifndef JCV_REAL_TYPE
#define JCV_REAL_TYPE float
#endif
#endif

#ifndef JCV_REAL_TYPE_EPSILON
#define JCV_REAL_TYPE_EPSILON FLT_EPSILON
#endif

#ifndef JCV_ATAN2
#define JCV_ATAN2(_Y_, _X_) atan2f(_Y_, _X_)
#endif

#ifndef JCV_SQRT
#define JCV_SQRT(_X_)       sqrtf(_X_)
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

typedef struct jcv_point_ jcv_point;
typedef struct jcv_rect_ jcv_rect;
typedef struct jcv_site_ jcv_site;
typedef struct jcv_edge_ jcv_edge;
typedef struct jcv_graphedge_ jcv_graphedge;
typedef struct jcv_delauney_edge_ jcv_delauney_edge;
typedef struct jcv_delauney_iter_ jcv_delauney_iter;
typedef struct jcv_diagram_ jcv_diagram;
typedef struct jcv_clipper_ jcv_clipper;
typedef struct jcv_context_internal_ jcv_context_internal;

typedef JCV_REAL_TYPE jcv_real;

/// Tests if a point is inside the final shape
typedef int (*jcv_clip_test_point_fn)(const jcv_clipper* clipper, const jcv_point p);

/** Given an edge, and the clipper, calculates the e->pos[0] and e->pos[1]
 * Returns 0 if not successful
 */
typedef int (*jcv_clip_edge_fn)(const jcv_clipper* clipper, jcv_edge* e);

/** Given the clipper, the site and the last edge,
 * closes any gaps in the polygon by adding new edges that follow the bounding shape
 * The internal context is use when allocating new edges.
 */
typedef void (*jcv_clip_fillgap_fn)(const jcv_clipper* clipper, jcv_context_internal* allocator, jcv_site* s);

#pragma pack(push, 1)

struct jcv_point_
{
	jcv_real x;
	jcv_real y;
};

struct jcv_graphedge_
{
	struct jcv_graphedge_* next;
	struct jcv_edge_* edge;
	struct jcv_site_* neighbor;
	jcv_point pos[2];
	jcv_real angle;
};

struct jcv_site_
{
	jcv_point p;
	int index;  // Index into the original list of points
	jcv_graphedge* edges;  // The half edges owned by the cell
};

// The coefficients a, b and c are from the general line equation: ax * by + c = 0
struct jcv_edge_
{
	struct jcv_edge_* next;
	jcv_site* sites[2];
	jcv_point pos[2];
	jcv_real a;
	jcv_real b;
	jcv_real c;
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
	jcv_point pos[2];
};

struct jcv_rect_
{
	jcv_point min;
	jcv_point max;
};

struct jcv_clipper_
{
	jcv_clip_test_point_fn test_fn;
	jcv_clip_edge_fn clip_fn;
	jcv_clip_fillgap_fn fill_fn;
	jcv_point min;        // The bounding rect min
	jcv_point max;        // The bounding rect max
	void* ctx;        // User defined context
};

struct jcv_diagram_
{
	jcv_context_internal* internal;
	int numsites;
	jcv_point min;
	jcv_point max;
};

#pragma pack(pop)



#endif //SILLY_UTILS_JVC_STRUCT_H
