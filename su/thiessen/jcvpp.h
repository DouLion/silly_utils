///*
// * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
// * reserved.
// * @website: http://www.tianzhixiang.com.cn/
// * @author: dou li yang
// * @date: 2023/9/11 14:03
// * @version: 1.0.1
// * @software: silly_utils
// * @description:
// */
//#pragma once
//
//#ifndef SILLY_UTILS_JCVPP_H
//#define SILLY_UTILS_JCVPP_H
//
//#include <cmath>
//#include <cstddef>
//#include <cstdlib>
//#include <cstdint>
//#include <cfloat>
//
//#include <cassert>
//
//#ifdef USING_DOUBLE
//#ifndef JCV_REAL_TYPE
//#define JCV_REAL_TYPE double
//#endif
//#else
//#ifndef JCV_REAL_TYPE
//#define JCV_REAL_TYPE float
//#endif
//#endif
//
//#ifndef JCV_REAL_TYPE_EPSILON
//#define JCV_REAL_TYPE_EPSILON FLT_EPSILON
//#endif
//
//#ifndef JCV_ATAN2
//#define JCV_ATAN2(_Y_, _X_) atan2f(_Y_, _X_)
//#endif
//
//#ifndef JCV_SQRT
//#define JCV_SQRT(_X_)       sqrtf(_X_)
//#endif
//
//#ifndef JCV_PI
//#define JCV_PI 3.14159265358979323846264338327950288f
//#endif
//
//#ifndef JCV_FLT_MAX
//#define JCV_FLT_MAX 3.402823466e+38F
//#endif
//
//#ifndef JCV_EDGE_INTERSECT_THRESHOLD
// // Fix for Issue #40
//#define JCV_EDGE_INTERSECT_THRESHOLD 1.0e-10F
//#endif
//
//typedef JCV_REAL_TYPE jcv_real;
//
//typedef struct jcv_point_ jcv_point;
//typedef struct jcv_rect_ jcv_rect;
//typedef struct jcv_site_ jcv_site;
//typedef struct jcv_edge_ jcv_edge;
//typedef struct jcv_graphedge_ jcv_graphedge;
//typedef struct jcv_delauney_edge_ jcv_delauney_edge;
//typedef struct jcv_delauney_iter_ jcv_delauney_iter;
//typedef struct jcv_diagram_ jcv_diagram;
//typedef struct jcv_clipper_ jcv_clipper;
//typedef struct jcv_context_internal_ jcv_context_internal;
//
///// Tests if a point is inside the final shape
//typedef int (*jcv_clip_test_point_fn)(const jcv_clipper* clipper, const jcv_point p);
//
///** Given an edge, and the clipper, calculates the e->pos[0] and e->pos[1]
// * Returns 0 if not successful
// */
//typedef int (*jcv_clip_edge_fn)(const jcv_clipper* clipper, jcv_edge* e);
//
///** Given the clipper, the site and the last edge,
// * closes any gaps in the polygon by adding new edges that follow the bounding shape
// * The internal context is use when allocating new edges.
// */
//typedef void (*jcv_clip_fillgap_fn)(const jcv_clipper* clipper, jcv_context_internal* allocator, jcv_site* s);
//
//
///**
// * Uses malloc
// * If a clipper is not supplied, a default box clipper will be used
// * If rect is null, an automatic bounding box is calculated, with an extra padding of 10 units
// * All points will be culled against the bounding rect, and all edges will be clipped against it.
// */
//
//
//typedef void* (*FJCVAllocFn)(void* userctx, size_t size);
//
//typedef void (*FJCVFreeFn)(void* userctx, void* p);
//
//
//// Creates an iterator over the delauney edges of a voronoi diagram
//jcv_delauney_iter jcv_delauney_begin(const jcv_diagram* diagram);
//
//// Steps the iterator and returns the next edge
//// Returns 0 when there are no more edges
//int jcv_delauney_next(jcv_delauney_iter* iter, jcv_delauney_edge* next);
//
//
//#pragma pack(push, 1)
//
//struct jcv_point_
//{
//	jcv_real x;
//	jcv_real y;
//};
//
//struct jcv_graphedge_
//{
//	struct jcv_graphedge_* next;
//	struct jcv_edge_* edge;
//	struct jcv_site_* neighbor;
//	jcv_point pos[2];
//	jcv_real angle;
//};
//
//struct jcv_site_
//{
//	jcv_point p;
//	int index;  // Index into the original list of points
//	jcv_graphedge* edges;  // The half edges owned by the cell
//};
//
//// The coefficients a, b and c are from the general line equation: ax * by + c = 0
//struct jcv_edge_
//{
//	struct jcv_edge_* next;
//	jcv_site* sites[2];
//	jcv_point pos[2];
//	jcv_real a;
//	jcv_real b;
//	jcv_real c;
//};
//
//struct jcv_delauney_iter_
//{
//	const jcv_edge* sentinel;
//	const jcv_edge* current;
//};
//
//struct jcv_delauney_edge_
//{
//	const jcv_edge* edge;
//	const jcv_site* sites[2];
//	jcv_point pos[2];
//};
//
//struct jcv_rect_
//{
//	jcv_point min;
//	jcv_point max;
//};
//
//struct jcv_clipper_
//{
//	jcv_clip_test_point_fn test_fn;
//	jcv_clip_edge_fn clip_fn;
//	jcv_clip_fillgap_fn fill_fn;
//	jcv_point min;        // The bounding rect min
//	jcv_point max;        // The bounding rect max
//	void* ctx;        // User defined context
//};
//
//struct jcv_diagram_
//{
//	jcv_context_internal* internal;
//	int numsites;
//	jcv_point min;
//	jcv_point max;
//};
//
//
//
//class jcvpp
//{
//public:
//	static void jcv_diagram_generate(int num_points, const jcv_point* points, const jcv_rect* rect, const jcv_clipper* clipper, jcv_diagram* diagram);
//	// For the default clipper
//	static int jcv_boxshape_test(const jcv_clipper* clipper, const jcv_point p);
//
//	static int jcv_boxshape_clip(const jcv_clipper* clipper, jcv_edge* e);
//
//	static void jcv_boxshape_fillgaps(const jcv_clipper* clipper, jcv_context_internal* allocator, jcv_site* s);
//	// Same as above, but allows the client to use a custom allocator
//	static void jcv_diagram_generate_useralloc(int num_points, const jcv_point* points, const jcv_rect* rect, const jcv_clipper* clipper, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, jcv_diagram* diagram);
//
//	// Uses free (or the registered custom free function)
//	static void jcv_diagram_free(jcv_diagram* diagram);
//
//	// Returns an array of sites, where each index is the same as the original input point array.
//	static const jcv_site* jcv_diagram_get_sites(const jcv_diagram* diagram);
//
//	// Returns a linked list of all the voronoi edges
//	// excluding the ones that lie on the borders of the bounding box.
//	// For a full list of edges, you need to iterate over the sites, and their graph edges.
//	static const jcv_edge* jcv_diagram_get_edges(const jcv_diagram* diagram);
//
//	// Iterates over a list of edges, skipping invalid edges (where p0==p1)
//	static const jcv_edge* jcv_diagram_get_next_edge(const jcv_edge* edge);
//};
//#pragma pack(pop)
//
//#endif //SILLY_UTILS_JCVPP_H
