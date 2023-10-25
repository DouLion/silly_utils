//
// Created by dell on 2023/10/24.
//

#ifndef SILLY_UTILS_SILLY_SPATIALITE_H
#define SILLY_UTILS_SILLY_SPATIALITE_H
#include <geos_c.h>

#include <sqlite3.h>
#include <spatialite/gaiageo.h>
#include <spatialite.h>

static char* geom_type(int type)
{
    /* utility function returning corresponding GeometryType as a string */
    static char* name = "EMPTY / NULL GEOMETRY";
    if (type == GAIA_POINT)
        name = "POINT";
    if (type == GAIA_LINESTRING)
        name = "LINESTRING";
    if (type == GAIA_POLYGON)
        name = "POLYGON";
    if (type == GAIA_MULTIPOINT)
        name = "MULTIPOINT";
    if (type == GAIA_MULTILINESTRING)
        name = "MULTILINESTRING";
    if (type == GAIA_MULTIPOLYGON)
        name = "MULTIPOLYGON";
    if (type == GAIA_GEOMETRYCOLLECTION)
        name = "GEOMETRYCOLLECTION";
    return name;
}

static void geometry_printout(gaiaGeomCollPtr geom)
{
    /* utility function printing a generic Geometry object */
    gaiaPointPtr pt;
    gaiaLinestringPtr ln;
    gaiaPolygonPtr pg;
    gaiaRingPtr rng;
    int n_pts = 0;
    int n_lns = 0;
    int n_pgs = 0;
    int cnt;
    int iv;
    int ir;
    double x;
    double y;


    /* we'll now count how many POINTs are there */
    pt = geom->FirstPoint;
    while (pt)
    {
        n_pts++;
        pt = pt->Next;
    }
    /* we'll now count how many LINESTRINGs are there */
    ln = geom->FirstLinestring;
    while (ln)
    {
        n_lns++;
        ln = ln->Next;
    }
    /* we'll now count how many POLYGONs are there */
    pg = geom->FirstPolygon;
    while (pg)
    {
        n_pgs++;
        pg = pg->Next;
    }

    if (n_pts)
    {
        /* printing POINTs coordinates */
        cnt = 0;
        pt = geom->FirstPoint;
        while (pt)
        {
            /* we'll now scan the linked list of POINTs */
            printf("\t\t\tPOINT %d/%d x=%1.4f y=%1.4f\n",
                cnt, n_pts, pt->X, pt->Y);
            cnt++;
            pt = pt->Next;
        }
    }

    if (n_lns)
    {
        /* printing LINESTRINGs coordinates */
        cnt = 0;
        ln = geom->FirstLinestring;
        while (ln)
        {
            /* we'll now scan the linked list of LINESTRINGs */
            printf("\t\t\tLINESTRING %d/%d has %d vertices\n",
                cnt, n_lns, ln->Points);
            for (iv = 0; iv < ln->Points; iv++)
            {
                /* we'll now retrieve coordinates for each vertex */
                gaiaGetPoint(ln->Coords, iv, &x, &y);
                printf("\t\t\t\tvertex %d/%d x=%1.4f y=%1.4f\n",
                    iv, ln->Points, x, y);
            }
            cnt++;
            ln = ln->Next;
        }
    }

    if (n_pgs)
    {
        /* printing POLYGONs coordinates */
        cnt = 0;
        pg = geom->FirstPolygon;
        while (pg)
        {
            /* we'll now scan the linked list of POLYGONs */
            printf("\t\t\tPOLYGON %d/%d has %d hole%c\n",
                cnt, n_pgs, pg->NumInteriors,
                (pg->NumInteriors == 1) ? ' ' : 's');

            /*
            now we'll print out the Exterior ring
            [surely a POLYGON has an Exterior ring
            */
            rng = pg->Exterior;
            printf("\t\t\t\tExteriorRing has %d vertices\n", rng->Points);
            for (iv = 0; iv < rng->Points; iv++)
            {
                /* we'll now retrieve coordinates for each vertex */
                gaiaGetPoint(rng->Coords, iv, &x, &y);
                printf("\t\t\t\t\tvertex %d/%d x=%1.4f y=%1.4f\n",
                    iv, rng->Points, x, y);
            }

            for (ir = 0; ir < pg->NumInteriors; ir++)
            {
                /*
                a POLYGON can contain an arbitrary number of Interior rings (including zero)
                */
                rng = pg->Interiors + ir;
                printf("\t\t\t\tInteriorRing %d/%d has %d vertices\n",
                    ir, pg->NumInteriors, rng->Points);
                for (iv = 0; iv < rng->Points; iv++)
                {
                    /* we'll now retrieve coordinates for each vertex */
                    gaiaGetPoint(rng->Coords, iv, &x, &y);
                    printf
                    ("\t\t\t\t\tvertex %d/%d x=%1.4f y=%1.4f\n",
                        iv, rng->Points, x, y);
                }
            }

            cnt++;
            pg = pg->Next;
        }
    }
}

class silly_spatialite {
public:
    int test();
};


#endif //SILLY_UTILS_SILLY_SPATIALITE_H
