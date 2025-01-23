// Copyright (c) 2015-2022 Mathias Westerdahl
// For LICENSE (MIT), USAGE or HISTORY, see bottom of file
#pragma once
#ifndef JC_VORONOI_H
#define JC_VORONOI_H

#include <thiessen/jvc_struct.h>

/**
 * Uses malloc
 * If a clipper is not supplied, a default box clipper will be used
 * If rect is null, an automatic bounding box is calculated, with an extra padding of 10 units
 * All points will be culled against the bounding rect, and all edges will be clipped against it.
 */
extern void jcv_diagram_generate(int num_points, const silly_point* points, const silly_rect* rect, jcv_diagram* diagram);

typedef void* (*FJCVAllocFn)(void* userctx, size_t size);

typedef void (*FJCVFreeFn)(void* userctx, void* p);

// Same as above, but allows the client to use a custom allocator
extern void jcv_diagram_generate_useralloc(int num_points, const silly_point* points, const silly_rect* rect, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, jcv_diagram* diagram);

// Uses free (or the registered custom free function)
extern void jcv_diagram_free(jcv_diagram* diagram);

// Returns an array of sites, where each index is the same as the original input point array.
extern const jcv_site* jcv_diagram_get_sites(const jcv_diagram* diagram);

// Returns a linked list of all the voronoi edges
// excluding the ones that lie on the borders of the bounding box.
// For a full list of edges, you need to iterate over the sites, and their graph edges.
extern const jcv_edge* jcv_diagram_get_edges(const jcv_diagram* diagram);

// Iterates over a list of edges, skipping invalid edges (where p0==p1)
extern const jcv_edge* jcv_diagram_get_next_edge(const jcv_edge* edge);

// Creates an iterator over the delauney edges of a voronoi diagram
jcv_delauney_iter jcv_delauney_begin(const jcv_diagram* diagram);

// Steps the iterator and returns the next edge
// Returns 0 when there are no more edges
int jcv_delauney_next(jcv_delauney_iter* iter, jcv_delauney_edge* next);

#endif  // JC_VORONOI_H

#ifdef JC_VORONOI_IMPLEMENTATION
#undef JC_VORONOI_IMPLEMENTATION

#include <memory.h>

// INTERNAL FUNCTIONS

#if defined(_MSC_VER) && !defined(__cplusplus)
#define __inline
#endif

static const int JCV_DIRECTION_LEFT = 0;
static const int JCV_DIRECTION_RIGHT = 1;
static const double JCV_INVALID_VALUE = (double)-JCV_FLT_MAX;

// double

static double jcv_abs(double v)
{
    return (v < 0) ? -v : v;
}

static int double_eq(double a, double b)
{
    return jcv_abs(a - b) < double_TYPE_EPSILON;
}

static double double_to_int(double v)
{
    return (double)(long long)v;
}

// Only used for calculating the initial bounding box
static double jcv_floor(double v)
{
    double i = double_to_int(v);
    return (v < i) ? i - 1 : i;
}

// Only used for calculating the initial bounding box
static double jcv_ceil(double v)
{
    double i = double_to_int(v);
    return (v > i) ? i + 1 : i;
}

static double jcv_min(double a, double b)
{
    return a < b ? a : b;
}

static double jcv_max(double a, double b)
{
    return a > b ? a : b;
}

// silly_point

static int silly_point_cmp(const void* p1, const void* p2)
{
    const silly_point* s1 = (const silly_point*)p1;
    const silly_point* s2 = (const silly_point*)p2;
    return (s1->lttd != s2->lttd) ? (s1->lttd < s2->lttd ? -1 : 1) : (s1->lgtd < s2->lgtd ? -1 : 1);
}

static int silly_point_less(const silly_point* pt1, const silly_point* pt2)
{
    return (pt1->lttd == pt2->lttd) ? (pt1->lgtd < pt2->lgtd) : pt1->lttd < pt2->lttd;
}

static int silly_point_eq(const silly_point* pt1, const silly_point* pt2)
{
    return double_eq(pt1->lttd, pt2->lttd) && double_eq(pt1->lgtd, pt2->lgtd);
}

static int silly_point_on_box_edge(const silly_point* pt, const silly_point* min, const silly_point* max)
{
    return pt->lgtd == min->lgtd || pt->lttd == min->lttd || pt->lgtd == max->lgtd || pt->lttd == max->lttd;
}

// corners

static const int JCV_EDGE_LEFT = 1;
static const int JCV_EDGE_RIGHT = 2;
static const int JCV_EDGE_BOTTOM = 4;
static const int JCV_EDGE_TOP = 8;

static const int JCV_CORNER_NONE = 0;
static const int JCV_CORNER_TOP_LEFT = 1;
static const int JCV_CORNER_BOTTOM_LEFT = 2;
static const int JCV_CORNER_BOTTOM_RIGHT = 3;
static const int JCV_CORNER_TOP_RIGHT = 4;

static int jcv_get_edge_flags(const silly_point* pt, const silly_point* min, const silly_point* max)
{
    int flags = 0;
    if (pt->lgtd == min->lgtd)
        flags |= JCV_EDGE_LEFT;
    else if (pt->lgtd == max->lgtd)
        flags |= JCV_EDGE_RIGHT;
    if (pt->lttd == min->lttd)
        flags |= JCV_EDGE_BOTTOM;
    else if (pt->lttd == max->lttd)
        flags |= JCV_EDGE_TOP;
    return flags;
}

static int jcv_edge_flags_to_corner(int edge_flags)
{
#define TEST_FLAGS(_FLAGS, _RETVAL) \
    if ((_FLAGS) == edge_flags)     \
    return _RETVAL
    TEST_FLAGS(JCV_EDGE_TOP | JCV_EDGE_LEFT, JCV_CORNER_TOP_LEFT);
    TEST_FLAGS(JCV_EDGE_TOP | JCV_EDGE_RIGHT, JCV_CORNER_TOP_RIGHT);
    TEST_FLAGS(JCV_EDGE_BOTTOM | JCV_EDGE_LEFT, JCV_CORNER_BOTTOM_LEFT);
    TEST_FLAGS(JCV_EDGE_BOTTOM | JCV_EDGE_RIGHT, JCV_CORNER_BOTTOM_RIGHT);
#undef TEST_FLAGS
    return 0;
}

static int jcv_is_corner(int corner)
{
    return corner != 0;
}

static int jcv_corner_rotate_90(int corner)
{
    corner--;
    corner = (corner + 1) % 4;
    return corner + 1;
}

static silly_point jcv_corner_to_point(int corner, const silly_point* min, const silly_point* max)
{
    silly_point p;
    if (corner == JCV_CORNER_TOP_LEFT)
    {
        p.lgtd = min->lgtd;
        p.lttd = max->lttd;
    }
    else if (corner == JCV_CORNER_TOP_RIGHT)
    {
        p.lgtd = max->lgtd;
        p.lttd = max->lttd;
    }
    else if (corner == JCV_CORNER_BOTTOM_LEFT)
    {
        p.lgtd = min->lgtd;
        p.lttd = min->lttd;
    }
    else if (corner == JCV_CORNER_BOTTOM_RIGHT)
    {
        p.lgtd = max->lgtd;
        p.lttd = min->lttd;
    }
    else
    {
        p.lgtd = JCV_INVALID_VALUE;
        p.lttd = JCV_INVALID_VALUE;
    }
    return p;
}

static double silly_point_dist_sq(const silly_point* pt1, const silly_point* pt2)
{
    double diffx = pt1->lgtd - pt2->lgtd;
    double diffy = pt1->lttd - pt2->lttd;
    return diffx * diffx + diffy * diffy;
}

static double silly_point_dist(const silly_point* pt1, const silly_point* pt2)
{
    return (double)(JCV_SQRT(silly_point_dist_sq(pt1, pt2)));
}

// Structs

#pragma pack(push, 1)

typedef struct jcv_halfedge_
{
    jcv_edge* edge;
    struct jcv_halfedge_* left;
    struct jcv_halfedge_* right;
    silly_point vertex;
    double y;
    int direction;  // 0=left, 1=right
    int pqpos;
} jcv_halfedge;

typedef struct jcv_memoryblock_
{
    size_t sizefree;
    struct jcv_memoryblock_* next;
    char* memory;
} jcv_memoryblock;

typedef int (*FJCVPriorityQueuePrint)(const void* node, int pos);

typedef struct jcv_priorityqueue_
{
    // Implements a binary heap
    int maxnumitems;
    int numitems;
    void** items;
} jcv_priorityqueue;

struct jcv_context_internal_
{
    void* mem;
    jcv_edge* edges;
    jcv_halfedge* beachline_start;
    jcv_halfedge* beachline_end;
    jcv_halfedge* last_inserted;
    jcv_priorityqueue* eventqueue;

    jcv_site* sites;
    jcv_site* bottomsite;
    int numsites;
    int currentsite;
    int _padding;

    jcv_memoryblock* memblocks;
    jcv_edge* edgepool;
    jcv_halfedge* halfedgepool;
    void** eventmem;
    void* memctx;  // Given by the user
    FJCVAllocFn alloc;
    FJCVFreeFn free;

    silly_rect rect;
};

#pragma pack(pop)

void jcv_diagram_free(jcv_diagram* d)
{
    jcv_context_internal* internal = d->internal;
    void* memctx = internal->memctx;
    FJCVFreeFn freefn = internal->free;
    while (internal->memblocks)
    {
        jcv_memoryblock* p = internal->memblocks;
        internal->memblocks = internal->memblocks->next;
        freefn(memctx, p);
    }

    freefn(memctx, internal->mem);
}

const jcv_site* jcv_diagram_get_sites(const jcv_diagram* diagram)
{
    return diagram->internal->sites;
}

const jcv_edge* jcv_diagram_get_edges(const jcv_diagram* diagram)
{
    jcv_edge e;
    e.next = diagram->internal->edges;
    return jcv_diagram_get_next_edge(&e);
}

const jcv_edge* jcv_diagram_get_next_edge(const jcv_edge* edge)
{
    const jcv_edge* e = edge->next;
    while (e != 0 && silly_point_eq(&e->pos[0], &e->pos[1]))
    {
        e = e->next;
    }
    return e;
}

jcv_delauney_iter jcv_delauney_begin(const jcv_diagram* diagram)
{
    jcv_delauney_iter iter;
    iter.current = 0;
    iter.sentinel = jcv_diagram_get_edges(diagram);
    return iter;
}

int jcv_delauney_next(jcv_delauney_iter* iter, jcv_delauney_edge* next)
{
    if (iter->sentinel)
    {
        iter->current = iter->sentinel;
        iter->sentinel = 0;
    }
    else
    {
        // Note: If we use the raw edges, we still get a proper delauney triangulation
        // However, the result looks less relevant to the cells contained within the bounding box
        // E.g. some cells that look isolated from each other, suddenly still are connected,
        // because they share an edge outside of the bounding box
        iter->current = jcv_diagram_get_next_edge(iter->current);
    }

    while (iter->current && (iter->current->sites[0] == 0 || iter->current->sites[1] == 0))
    {
        iter->current = jcv_diagram_get_next_edge(iter->current);
    }

    if (!iter->current)
        return 0;

    next->edge = iter->current;
    next->sites[0] = next->edge->sites[0];
    next->sites[1] = next->edge->sites[1];
    next->pos[0] = next->sites[0]->p;
    next->pos[1] = next->sites[1]->p;
    return 1;
}

static void* jcv_align(void* value, size_t alignment)
{
    return (void*)(((uintptr_t)value + (alignment - 1)) & ~(alignment - 1));
}

static void* jcv_alloc(jcv_context_internal* internal, size_t size)
{
    if (!internal->memblocks || internal->memblocks->sizefree < (size + sizeof(void*)))
    {
        size_t blocksize = 16 * 1024;
        jcv_memoryblock* block = (jcv_memoryblock*)internal->alloc(internal->memctx, blocksize);
        size_t offset = sizeof(jcv_memoryblock);
        block->sizefree = blocksize - offset;
        block->next = internal->memblocks;
        block->memory = ((char*)block) + offset;
        internal->memblocks = block;
    }
    void* p_raw = internal->memblocks->memory;
    void* p_aligned = jcv_align(p_raw, sizeof(void*));
    size += (uintptr_t)p_aligned - (uintptr_t)p_raw;
    internal->memblocks->memory += size;
    internal->memblocks->sizefree -= size;
    return p_aligned;
}

static jcv_edge* jcv_alloc_edge(jcv_context_internal* internal)
{
    return (jcv_edge*)jcv_alloc(internal, sizeof(jcv_edge));
}

static jcv_halfedge* jcv_alloc_halfedge(jcv_context_internal* internal)
{
    if (internal->halfedgepool)
    {
        jcv_halfedge* edge = internal->halfedgepool;
        internal->halfedgepool = internal->halfedgepool->right;
        return edge;
    }

    return (jcv_halfedge*)jcv_alloc(internal, sizeof(jcv_halfedge));
}

static jcv_graphedge* jcv_alloc_graphedge(jcv_context_internal* internal)
{
    return (jcv_graphedge*)jcv_alloc(internal, sizeof(jcv_graphedge));
}

static void* jcv_alloc_fn(void* memctx, size_t size)
{
    (void)memctx;
    return malloc(size);
}

static void jcv_free_fn(void* memctx, void* p)
{
    (void)memctx;
    free(p);
}

// jcv_edge

static int jcv_is_valid(const silly_point* p)
{
    return (p->lgtd != JCV_INVALID_VALUE || p->lttd != JCV_INVALID_VALUE) ? 1 : 0;
}

static void jcv_edge_create(jcv_edge* e, jcv_site* s1, jcv_site* s2)
{
    e->next = 0;
    e->sites[0] = s1;
    e->sites[1] = s2;
    e->pos[0].lgtd = JCV_INVALID_VALUE;
    e->pos[0].lttd = JCV_INVALID_VALUE;
    e->pos[1].lgtd = JCV_INVALID_VALUE;
    e->pos[1].lttd = JCV_INVALID_VALUE;

    // Create line equation between S1 and S2:
    // double a = -1 * (s2->p.lttd - s1->p.lttd);
    // double b = s2->p.lgtd - s1->p.lgtd;
    // //double c = -1 * (s2->p.lgtd - s1->p.lgtd) * s1->p.lttd + (s2->p.lttd - s1->p.lttd) * s1->p.lgtd;
    //
    // // create perpendicular line
    // double pa = b;
    // double pb = -a;
    // //double pc = pa * s1->p.lgtd + pb * s1->p.lttd;
    //
    // // Move to the mid point
    // double mx = s1->p.lgtd + dx * double(0.5);
    // double my = s1->p.lttd + dy * double(0.5);
    // double pc = ( pa * mx + pb * my );

    double dx = s2->p.lgtd - s1->p.lgtd;
    double dy = s2->p.lttd - s1->p.lttd;
    int dx_is_larger = (dx * dx) > (dy * dy);  // instead of fabs

    // Simplify it, using dx and dy
    e->c = dx * (s1->p.lgtd + dx * (double)0.5) + dy * (s1->p.lttd + dy * (double)0.5);

    if (dx_is_larger)
    {
        e->a = (double)1;
        e->b = dy / dx;
        e->c /= dx;
    }
    else
    {
        e->a = dx / dy;
        e->b = (double)1;
        e->c /= dy;
    }
}

static jcv_edge* jcv_edge_new(jcv_context_internal* internal, jcv_site* s1, jcv_site* s2)
{
    jcv_edge* e = jcv_alloc_edge(internal);
    jcv_edge_create(e, s1, s2);
    return e;
}

// jcv_halfedge

static void jcv_halfedge_link(jcv_halfedge* edge, jcv_halfedge* newedge)
{
    newedge->left = edge;
    newedge->right = edge->right;
    edge->right->left = newedge;
    edge->right = newedge;
}

static void jcv_halfedge_unlink(jcv_halfedge* he)
{
    he->left->right = he->right;
    he->right->left = he->left;
    he->left = 0;
    he->right = 0;
}

static jcv_halfedge* jcv_halfedge_new(jcv_context_internal* internal, jcv_edge* e, int direction)
{
    jcv_halfedge* he = jcv_alloc_halfedge(internal);
    he->edge = e;
    he->left = 0;
    he->right = 0;
    he->direction = direction;
    he->pqpos = 0;
    // These are set outside
    // he->lttd
    // he->vertex
    return he;
}

static void jcv_halfedge_delete(jcv_context_internal* internal, jcv_halfedge* he)
{
    he->right = internal->halfedgepool;
    internal->halfedgepool = he;
}

static jcv_site* jcv_halfedge_leftsite(const jcv_halfedge* he)
{
    return he->edge->sites[he->direction];
}

static jcv_site* jcv_halfedge_rightsite(const jcv_halfedge* he)
{
    return he->edge ? he->edge->sites[1 - he->direction] : 0;
}

static int jcv_halfedge_rightof(const jcv_halfedge* he, const silly_point* p)
{
    const jcv_edge* e = he->edge;
    const jcv_site* topsite = e->sites[1];

    int right_of_site = (p->lgtd > topsite->p.lgtd) ? 1 : 0;
    if (right_of_site && he->direction == JCV_DIRECTION_LEFT)
        return 1;
    if (!right_of_site && he->direction == JCV_DIRECTION_RIGHT)
        return 0;

    double dxp, dyp, dxs, t1, t2, t3, yl;

    int above;
    if (e->a == (double)1)
    {
        dyp = p->lttd - topsite->p.lttd;
        dxp = p->lgtd - topsite->p.lgtd;
        int fast = 0;
        if ((!right_of_site & (e->b < (double)0)) | (right_of_site & (e->b >= (double)0)))
        {
            above = dyp >= e->b * dxp;
            fast = above;
        }
        else
        {
            above = (p->lgtd + p->lttd * e->b) > e->c;
            if (e->b < (double)0)
                above = !above;
            if (!above)
                fast = 1;
        }
        if (!fast)
        {
            dxs = topsite->p.lgtd - e->sites[0]->p.lgtd;
            above = e->b * (dxp * dxp - dyp * dyp) < dxs * dyp * ((double)1 + (double)2 * dxp / dxs + e->b * e->b);
            if (e->b < (double)0)
                above = !above;
        }
    }
    else  // e->b == 1
    {
        yl = e->c - e->a * p->lgtd;
        t1 = p->lttd - yl;
        t2 = p->lgtd - topsite->p.lgtd;
        t3 = yl - topsite->p.lttd;
        above = t1 * t1 > (t2 * t2 + t3 * t3);
    }
    return (he->direction == JCV_DIRECTION_LEFT ? above : !above);
}

// Keeps the priority queue sorted with events sorted in ascending order
// Return 1 if the edges needs to be swapped
static int jcv_halfedge_compare(const jcv_halfedge* he1, const jcv_halfedge* he2)
{
    return (he1->y == he2->y) ? he1->vertex.lgtd > he2->vertex.lgtd : he1->y > he2->y;
}

static int jcv_halfedge_intersect(const jcv_halfedge* he1, const jcv_halfedge* he2, silly_point* out)
{
    const jcv_edge* e1 = he1->edge;
    const jcv_edge* e2 = he2->edge;

    double d = e1->a * e2->b - e1->b * e2->a;
    if (((double)-JCV_EDGE_INTERSECT_THRESHOLD < d && d < (double)JCV_EDGE_INTERSECT_THRESHOLD))
    {
        return 0;
    }
    out->lgtd = (e1->c * e2->b - e1->b * e2->c) / d;
    out->lttd = (e1->a * e2->c - e1->c * e2->a) / d;

    const jcv_edge* e;
    const jcv_halfedge* he;
    if (silly_point_less(&e1->sites[1]->p, &e2->sites[1]->p))
    {
        he = he1;
        e = e1;
    }
    else
    {
        he = he2;
        e = e2;
    }

    int right_of_site = out->lgtd >= e->sites[1]->p.lgtd;
    if ((right_of_site && he->direction == JCV_DIRECTION_LEFT) || (!right_of_site && he->direction == JCV_DIRECTION_RIGHT))
    {
        return 0;
    }

    return 1;
}

// Priority queue

static int jcv_pq_moveup(jcv_priorityqueue* pq, int pos)
{
    jcv_halfedge** items = (jcv_halfedge**)pq->items;
    jcv_halfedge* node = items[pos];

    for (int parent = (pos >> 1); pos > 1 && jcv_halfedge_compare(items[parent], node); pos = parent, parent = parent >> 1)
    {
        items[pos] = items[parent];
        items[pos]->pqpos = pos;
    }

    node->pqpos = pos;
    items[pos] = node;
    return pos;
}

static int jcv_pq_maxchild(jcv_priorityqueue* pq, int pos)
{
    int child = pos << 1;
    if (child >= pq->numitems)
        return 0;
    jcv_halfedge** items = (jcv_halfedge**)pq->items;
    if ((child + 1) < pq->numitems && jcv_halfedge_compare(items[child], items[child + 1]))
        return child + 1;
    return child;
}

static int jcv_pq_movedown(jcv_priorityqueue* pq, int pos)
{
    jcv_halfedge** items = (jcv_halfedge**)pq->items;
    jcv_halfedge* node = items[pos];

    int child = jcv_pq_maxchild(pq, pos);
    while (child && jcv_halfedge_compare(node, items[child]))
    {
        items[pos] = items[child];
        items[pos]->pqpos = pos;
        pos = child;
        child = jcv_pq_maxchild(pq, pos);
    }

    items[pos] = node;
    items[pos]->pqpos = pos;
    return pos;
}

static void jcv_pq_create(jcv_priorityqueue* pq, int capacity, void** buffer)
{
    pq->maxnumitems = capacity;
    pq->numitems = 1;
    pq->items = buffer;
}

static int jcv_pq_empty(jcv_priorityqueue* pq)
{
    return pq->numitems == 1 ? 1 : 0;
}

static int jcv_pq_push(jcv_priorityqueue* pq, void* node)
{
    assert(pq->numitems < pq->maxnumitems);
    int n = pq->numitems++;
    pq->items[n] = node;
    return jcv_pq_moveup(pq, n);
}

static void* jcv_pq_pop(jcv_priorityqueue* pq)
{
    void* node = pq->items[1];
    pq->items[1] = pq->items[--pq->numitems];
    jcv_pq_movedown(pq, 1);
    return node;
}

static void* jcv_pq_top(jcv_priorityqueue* pq)
{
    return pq->items[1];
}

static void jcv_pq_remove(jcv_priorityqueue* pq, jcv_halfedge* node)
{
    if (pq->numitems == 1)
        return;
    int pos = node->pqpos;
    if (pos == 0)
        return;

    jcv_halfedge** items = (jcv_halfedge**)pq->items;

    items[pos] = items[--pq->numitems];
    if (jcv_halfedge_compare(node, items[pos]))
        jcv_pq_moveup(pq, pos);
    else
        jcv_pq_movedown(pq, pos);
    node->pqpos = pos;
}

// internal functions

static jcv_site* jcv_nextsite(jcv_context_internal* internal)
{
    return (internal->currentsite < internal->numsites) ? &internal->sites[internal->currentsite++] : 0;
}

static jcv_halfedge* jcv_get_edge_above_x(jcv_context_internal* internal, const silly_point* p)
{
    // Gets the arc on the beach line at the x coordinate (i.e. right above the new site event)

    // A good guess it's close by (Can be optimized)
    jcv_halfedge* he = internal->last_inserted;
    if (!he)
    {
        if (p->lgtd < (internal->rect.right - internal->rect.left) / 2)
            he = internal->beachline_start;
        else
            he = internal->beachline_end;
    }

    //
    if (he == internal->beachline_start || (he != internal->beachline_end && jcv_halfedge_rightof(he, p)))
    {
        do
        {
            he = he->right;
        } while (he != internal->beachline_end && jcv_halfedge_rightof(he, p));

        he = he->left;
    }
    else
    {
        do
        {
            he = he->left;
        } while (he != internal->beachline_start && !jcv_halfedge_rightof(he, p));
    }

    return he;
}

static int jcv_check_circle_event(const jcv_halfedge* he1, const jcv_halfedge* he2, silly_point* vertex)
{
    jcv_edge* e1 = he1->edge;
    jcv_edge* e2 = he2->edge;
    if (e1 == 0 || e2 == 0 || e1->sites[1] == e2->sites[1])
    {
        return 0;
    }

    return jcv_halfedge_intersect(he1, he2, vertex);
}

static void jcv_site_event(jcv_context_internal* internal, jcv_site* site)
{
    jcv_halfedge* left = jcv_get_edge_above_x(internal, &site->p);
    jcv_halfedge* right = left->right;
    jcv_site* bottom = jcv_halfedge_rightsite(left);
    if (!bottom)
        bottom = internal->bottomsite;

    jcv_edge* edge = jcv_edge_new(internal, bottom, site);
    edge->next = internal->edges;
    internal->edges = edge;

    jcv_halfedge* edge1 = jcv_halfedge_new(internal, edge, JCV_DIRECTION_LEFT);
    jcv_halfedge* edge2 = jcv_halfedge_new(internal, edge, JCV_DIRECTION_RIGHT);

    jcv_halfedge_link(left, edge1);
    jcv_halfedge_link(edge1, edge2);

    internal->last_inserted = right;

    silly_point p;
    if (jcv_check_circle_event(left, edge1, &p))
    {
        jcv_pq_remove(internal->eventqueue, left);
        left->vertex = p;
        left->y = p.lttd + silly_point_dist(&site->p, &p);
        jcv_pq_push(internal->eventqueue, left);
    }
    if (jcv_check_circle_event(edge2, right, &p))
    {
        edge2->vertex = p;
        edge2->y = p.lttd + silly_point_dist(&site->p, &p);
        jcv_pq_push(internal->eventqueue, edge2);
    }
}

// https://cp-algorithms.com/geometry/oriented-triangle-area.html
static double jcv_determinant(const silly_point* a, const silly_point* b, const silly_point* c)
{
    return (b->lgtd - a->lgtd) * (c->lttd - a->lttd) - (b->lttd - a->lttd) * (c->lgtd - a->lgtd);
}

static double jcv_calc_sort_metric(const jcv_site* site, const jcv_graphedge* edge)
{
    // We take the average of the two points, since we can better distinguish between very small edges
    double half = 1 / (double)2;
    double x = (edge->pos[0].lgtd + edge->pos[1].lgtd) * half;
    double y = (edge->pos[0].lttd + edge->pos[1].lttd) * half;
    double diffy = y - site->p.lttd;
    double angle = JCV_ATAN2(diffy, x - site->p.lgtd);
    if (diffy < 0)
        angle = angle + 2 * JCV_PI;
    return (double)angle;
}

static int jcv_graphedge_eq(jcv_graphedge* a, jcv_graphedge* b)
{
    return double_eq(a->angle, b->angle) && silly_point_eq(&a->pos[0], &b->pos[0]) && silly_point_eq(&a->pos[1], &b->pos[1]);
}

static void jcv_sortedges_insert(jcv_site* site, jcv_graphedge* edge)
{
    // Special case for the head end
    jcv_graphedge* prev = 0;
    if (site->edges == 0 || site->edges->angle >= edge->angle)
    {
        edge->next = site->edges;
        site->edges = edge;
    }
    else
    {
        // Locate the node before the point of insertion
        jcv_graphedge* current = site->edges;
        while (current->next != 0 && current->next->angle < edge->angle)
        {
            current = current->next;
        }
        prev = current;
        edge->next = current->next;
        current->next = edge;
    }

    // check to avoid duplicates
    if (prev && jcv_graphedge_eq(prev, edge))
    {
        prev->next = edge->next;
    }
    else if (edge->next && jcv_graphedge_eq(edge, edge->next))
    {
        edge->next = edge->next->next;
    }
}

static void jcv_finishline(jcv_context_internal* internal, jcv_edge* e)
{
    // Make sure the graph edges are CCW
    int flip = jcv_determinant(&e->sites[0]->p, &e->pos[0], &e->pos[1]) > (double)0 ? 0 : 1;

    for (int i = 0; i < 2; ++i)
    {
        jcv_graphedge* ge = jcv_alloc_graphedge(internal);

        ge->edge = e;
        ge->next = 0;
        ge->neighbor = e->sites[1 - i];
        ge->pos[flip] = e->pos[i];
        ge->pos[1 - flip] = e->pos[1 - i];
        ge->angle = jcv_calc_sort_metric(e->sites[i], ge);

        jcv_sortedges_insert(e->sites[i], ge);
    }
}

static void jcv_endpos(jcv_context_internal* internal, jcv_edge* e, const silly_point* p, int direction)
{
    e->pos[direction] = *p;

    if (!jcv_is_valid(&e->pos[1 - direction]))
        return;

    jcv_finishline(internal, e);
}

static void jcv_create_corner_edge(jcv_context_internal* internal, const jcv_site* site, jcv_graphedge* current, jcv_graphedge* gap)
{
    gap->neighbor = 0;
    gap->pos[0] = current->pos[1];

    if (current->pos[1].lgtd < internal->rect.right && current->pos[1].lttd == internal->rect.bottom)
    {
        gap->pos[1].lgtd = internal->rect.right;
        gap->pos[1].lttd = internal->rect.bottom;
    }
    else if (current->pos[1].lgtd > internal->rect.left && current->pos[1].lttd == internal->rect.top)
    {
        gap->pos[1].lgtd = internal->rect.left;
        gap->pos[1].lttd = internal->rect.top;
    }
    else if (current->pos[1].lttd > internal->rect.bottom && current->pos[1].lgtd == internal->rect.left)
    {
        gap->pos[1].lgtd = internal->rect.left;
        gap->pos[1].lttd = internal->rect.bottom;
    }
    else if (current->pos[1].lttd < internal->rect.top && current->pos[1].lgtd == internal->rect.right)
    {
        gap->pos[1].lgtd = internal->rect.right;
        gap->pos[1].lttd = internal->rect.top;
    }

    gap->angle = jcv_calc_sort_metric(site, gap);
}

static jcv_edge* jcv_create_gap_edge(jcv_context_internal* internal, jcv_site* site, jcv_graphedge* ge)
{
    jcv_edge* edge = jcv_alloc_edge(internal);
    edge->pos[0] = ge->pos[0];
    edge->pos[1] = ge->pos[1];
    edge->sites[0] = site;
    edge->sites[1] = 0;
    edge->a = edge->b = edge->c = 0;
    edge->next = internal->edges;
    internal->edges = edge;
    return edge;
}

void jcv_boxshape_fillgaps(jcv_context_internal* allocator, jcv_site* site)
{
    // They're sorted CCW, so if the current->pos[1] != next->pos[0], then we have a gap
    jcv_graphedge* current = site->edges;
    if (!current)
    {
        // No edges, then it should be a single cell
        assert(allocator->numsites == 1);

        jcv_graphedge* gap = jcv_alloc_graphedge(allocator);
        gap->neighbor = 0;
        gap->pos[0] = clipper->min;
        gap->pos[1].lgtd = clipper->right;
        gap->pos[1].lttd = clipper->bottom;
        gap->angle = jcv_calc_sort_metric(site, gap);
        gap->next = 0;
        gap->edge = jcv_create_gap_edge(allocator, site, gap);

        current = gap;
        site->edges = gap;
    }

    jcv_graphedge* next = current->next;
    if (!next)
    {
        jcv_graphedge* gap = jcv_alloc_graphedge(allocator);
        jcv_create_corner_edge(allocator, site, current, gap);
        gap->edge = jcv_create_gap_edge(allocator, site, gap);

        gap->next = current->next;
        current->next = gap;
        current = gap;
        next = site->edges;
    }

    while (current && next)
    {
        int current_edge_flags = jcv_get_edge_flags(&current->pos[1], &clipper->min, &clipper->max);
        if (current_edge_flags && !silly_point_eq(&current->pos[1], &next->pos[0]))
        {
            // Cases:
            //  Current and Next on the same border
            //  Current on one border, and Next on another border
            //  Current on the corner, Next on the border
            //  Current on the corner, Next on another border (another corner in between)

            int next_edge_flags = jcv_get_edge_flags(&next->pos[0], &clipper->min, &clipper->max);
            if (current_edge_flags & next_edge_flags)
            {
                // Current and Next on the same border
                jcv_graphedge* gap = jcv_alloc_graphedge(allocator);
                gap->neighbor = 0;
                gap->pos[0] = current->pos[1];
                gap->pos[1] = next->pos[0];
                gap->angle = jcv_calc_sort_metric(site, gap);
                gap->edge = jcv_create_gap_edge(allocator, site, gap);

                gap->next = current->next;
                current->next = gap;
            }
            else
            {
                // Current and Next on different borders
                int corner_flag = jcv_edge_flags_to_corner(current_edge_flags);
                if (corner_flag)
                {
                    // we are already at one corner, so we need to find the next one
                    corner_flag = jcv_corner_rotate_90(corner_flag);
                }
                else
                {
                    // we are on the middle of a border
                    // we need to find the adjacent corner, following the borders CCW
                    if (current_edge_flags == JCV_EDGE_TOP)
                    {
                        corner_flag = JCV_CORNER_TOP_LEFT;
                    }
                    else if (current_edge_flags == JCV_EDGE_LEFT)
                    {
                        corner_flag = JCV_CORNER_BOTTOM_LEFT;
                    }
                    else if (current_edge_flags == JCV_EDGE_BOTTOM)
                    {
                        corner_flag = JCV_CORNER_BOTTOM_RIGHT;
                    }
                    else if (current_edge_flags == JCV_EDGE_RIGHT)
                    {
                        corner_flag = JCV_CORNER_TOP_RIGHT;
                    }
                }
                silly_point corner = jcv_corner_to_point(corner_flag, &clipper->min, &clipper->max);

                jcv_graphedge* gap = jcv_alloc_graphedge(allocator);
                gap->neighbor = 0;
                gap->pos[0] = current->pos[1];
                gap->pos[1] = corner;
                gap->angle = jcv_calc_sort_metric(site, gap);
                gap->edge = jcv_create_gap_edge(allocator, site, gap);

                gap->next = current->next;
                current->next = gap;
            }
        }

        current = current->next;
        if (current)
        {
            next = current->next;
            if (!next)
                next = site->edges;
        }
    }
}

// Since the algorithm leaves gaps at the borders/corner, we want to fill them

static void jcv_circle_event(jcv_context_internal* internal)
{
    jcv_halfedge* left = (jcv_halfedge*)jcv_pq_pop(internal->eventqueue);

    jcv_halfedge* leftleft = left->left;
    jcv_halfedge* right = left->right;
    jcv_halfedge* rightright = right->right;
    jcv_site* bottom = jcv_halfedge_leftsite(left);
    jcv_site* top = jcv_halfedge_rightsite(right);

    silly_point vertex = left->vertex;
    jcv_endpos(internal, left->edge, &vertex, left->direction);
    jcv_endpos(internal, right->edge, &vertex, right->direction);

    internal->last_inserted = rightright;

    jcv_pq_remove(internal->eventqueue, right);
    jcv_halfedge_unlink(left);
    jcv_halfedge_unlink(right);
    jcv_halfedge_delete(internal, left);
    jcv_halfedge_delete(internal, right);

    int direction = JCV_DIRECTION_LEFT;
    if (bottom->p.lttd > top->p.lttd)
    {
        jcv_site* temp = bottom;
        bottom = top;
        top = temp;
        direction = JCV_DIRECTION_RIGHT;
    }

    jcv_edge* edge = jcv_edge_new(internal, bottom, top);
    edge->next = internal->edges;
    internal->edges = edge;

    jcv_halfedge* he = jcv_halfedge_new(internal, edge, direction);
    jcv_halfedge_link(leftleft, he);
    jcv_endpos(internal, edge, &vertex, JCV_DIRECTION_RIGHT - direction);

    silly_point p;
    if (jcv_check_circle_event(leftleft, he, &p))
    {
        jcv_pq_remove(internal->eventqueue, leftleft);
        leftleft->vertex = p;
        leftleft->lttd = p.lttd + silly_point_dist(&bottom->p, &p);
        jcv_pq_push(internal->eventqueue, leftleft);
    }
    if (jcv_check_circle_event(he, rightright, &p))
    {
        he->vertex = p;
        he->lttd = p.lttd + silly_point_dist(&bottom->p, &p);
        jcv_pq_push(internal->eventqueue, he);
    }
}

void jcv_diagram_generate(int num_points, const silly_point* points, const silly_rect* rect, jcv_diagram* d)
{
    jcv_diagram_generate_useralloc(num_points, points, rect, 0, jcv_alloc_fn, jcv_free_fn, d);
}

typedef union jcv_cast_align_struct_
{
    char* charp;
    void** voidpp;
} jcv_cast_align_struct;

static void silly_rect_union(silly_rect* rect, const silly_point* p)
{
    rect->left = jcv_min(rect->left, p->lgtd);
    rect->bottom = jcv_min(rect->bottom, p->lttd);
    rect->right = jcv_max(rect->right, p->lgtd);
    rect->top = jcv_max(rect->top, p->lttd);
}

static void silly_rect_round(silly_rect* rect)
{
    rect->left = jcv_floor(rect->left);
    rect->bottom = jcv_floor(rect->bottom);
    rect->right = jcv_ceil(rect->right);
    rect->top = jcv_ceil(rect->top);
}

static void silly_rect_inflate(silly_rect* rect, double amount)
{
    rect->left -= amount;
    rect->bottom -= amount;
    rect->right += amount;
    rect->top += amount;
}

static int jcv_prune_duplicates(jcv_context_internal* internal, silly_rect* rect)
{
    int num_sites = internal->numsites;
    jcv_site* sites = internal->sites;

    silly_rect r;
    r.left = r.bottom = JCV_FLT_MAX;
    r.right = r.top = -JCV_FLT_MAX;

    int offset = 0;
    // Prune duplicates first
    for (int i = 0; i < num_sites; i++)
    {
        const jcv_site* s = &sites[i];
        // Remove duplicates, to avoid anomalies
        if (i > 0 && silly_point_eq(&s->p, &sites[i - 1].p))
        {
            offset++;
            continue;
        }

        sites[i - offset] = sites[i];

        silly_rect_union(&r, &s->p);
    }
    internal->numsites -= offset;
    if (rect)
    {
        *rect = r;
    }
    return offset;
}

static int jcv_prune_not_in_shape(jcv_context_internal* internal, silly_rect* rect)
{
    int num_sites = internal->numsites;
    jcv_site* sites = internal->sites;

    silly_rect r;
    r.left = r.bottom = JCV_FLT_MAX;
    r.right = r.top = -JCV_FLT_MAX;

    int offset = 0;
    for (int i = 0; i < num_sites; i++)
    {
        const jcv_site* s = &sites[i];

        if (!internal->clipper.test_fn(&internal->clipper, s->p))
        {
            offset++;
            continue;
        }

        sites[i - offset] = sites[i];

        silly_rect_union(&r, &s->p);
    }
    internal->numsites -= offset;
    if (rect)
    {
        *rect = r;
    }
    return offset;
}

static jcv_context_internal* jcv_alloc_internal(int num_points, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn)
{
    // Interesting limits from Euler's equation
    // Slide 81: https://courses.cs.washington.edu/courses/csep521/01au/lectures/lecture10slides.pdf
    // Page 3: https://sites.cs.ucsb.edu/~suri/cs235/Voronoi.pdf
    size_t eventssize = (size_t)(num_points * 2) * sizeof(void*);  // beachline can have max 2*n-5 parabolas
    size_t sitessize = (size_t)num_points * sizeof(jcv_site);
    size_t memsize = sizeof(jcv_priorityqueue) + eventssize + sitessize + sizeof(jcv_context_internal) + 16u;  // 16 bytes padding for alignment

    char* originalmem = (char*)allocfn(userallocctx, memsize);
    memset(originalmem, 0, memsize);

    // align memory
    char* mem = (char*)jcv_align(originalmem, sizeof(void*));

    jcv_context_internal* internal = (jcv_context_internal*)mem;
    mem += sizeof(jcv_context_internal);
    internal->mem = originalmem;
    internal->memctx = userallocctx;
    internal->alloc = allocfn;
    internal->free = freefn;

    mem = (char*)jcv_align(mem, sizeof(void*));
    internal->sites = (jcv_site*)mem;
    mem += sitessize;

    mem = (char*)jcv_align(mem, sizeof(void*));
    internal->eventqueue = (jcv_priorityqueue*)mem;
    mem += sizeof(jcv_priorityqueue);
    assert(((uintptr_t)mem & (sizeof(void*) - 1)) == 0);

    jcv_cast_align_struct tmp;
    tmp.charp = mem;
    internal->eventmem = tmp.voidpp;

    assert((mem + eventssize) <= (originalmem + memsize));

    return internal;
}

void jcv_diagram_generate_useralloc(int num_points, const silly_point* points, const silly_rect* rect, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, jcv_diagram* d)
{
    if (d->internal)
        jcv_diagram_free(d);

    jcv_context_internal* internal = jcv_alloc_internal(num_points, userallocctx, allocfn, freefn);

    internal->beachline_start = jcv_halfedge_new(internal, 0, 0);
    internal->beachline_end = jcv_halfedge_new(internal, 0, 0);

    internal->beachline_start->left = 0;
    internal->beachline_start->right = internal->beachline_end;
    internal->beachline_end->left = internal->beachline_start;
    internal->beachline_end->right = 0;

    internal->last_inserted = 0;

    int max_num_events = num_points * 2;  // beachline can have max 2*n-5 parabolas
    jcv_pq_create(internal->eventqueue, max_num_events, (void**)internal->eventmem);

    internal->numsites = num_points;
    jcv_site* sites = internal->sites;

    for (int i = 0; i < num_points; ++i)
    {
        sites[i].p = points[i];
        sites[i].edges = 0;
        sites[i].index = i;
    }

    qsort(sites, (size_t)num_points, sizeof(jcv_site), silly_point_cmp);


    silly_rect tmp_rect;
    tmp_rect.left = tmp_rect.bottom = JCV_FLT_MAX;
    tmp_rect.right = tmp_rect.top = -JCV_FLT_MAX;
    jcv_prune_duplicates(internal, &tmp_rect);

    // Prune using the test second

    internal->rect = rect ? *rect : tmp_rect;

    d->min = internal->rect.min;
    d->max = internal->rect.max;
    d->numsites = internal->numsites;
    d->internal = internal;

    internal->bottomsite = jcv_nextsite(internal);

    jcv_priorityqueue* pq = internal->eventqueue;
    jcv_site* site = jcv_nextsite(internal);

    int finished = 0;
    while (!finished)
    {
        silly_point lowest_pq_point;
        if (!jcv_pq_empty(pq))
        {
            jcv_halfedge* he = (jcv_halfedge*)jcv_pq_top(pq);
            lowest_pq_point.lgtd = he->vertex.lgtd;
            lowest_pq_point.lttd = he->lttd;
        }

        if (site != 0 && (jcv_pq_empty(pq) || silly_point_less(&site->p, &lowest_pq_point)))
        {
            jcv_site_event(internal, site);
            site = jcv_nextsite(internal);
        }
        else if (!jcv_pq_empty(pq))
        {
            jcv_circle_event(internal);
        }
        else
        {
            finished = 1;
        }
    }

    for (jcv_halfedge* he = internal->beachline_start->right; he != internal->beachline_end; he = he->right)
    {
        jcv_finishline(internal, he->edge);
    }

}

#endif  // JC_VORONOI_IMPLEMENTATION
