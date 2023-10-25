//
// Created by dell on 2023/10/24.
//

#include "silly_spatialite.h"

int silly_spatialite::test()
{
    gaiaGeomCollPtr geo_pt = NULL;
    gaiaGeomCollPtr geo_ln = NULL;
    gaiaGeomCollPtr geo_pg = NULL;
    gaiaGeomCollPtr geo_mpt = NULL;
    gaiaGeomCollPtr geo_mln = NULL;
    gaiaGeomCollPtr geo_mpg = NULL;
    gaiaGeomCollPtr geo_coll = NULL;
    gaiaLinestringPtr line;
    gaiaPolygonPtr polyg;
    gaiaRingPtr ring;
    gaiaOutBuffer wkt;
    int ret;
    sqlite3* handle;
    void* cache;

    ret = sqlite3_open_v2("E:test_sp.db", &handle, SQLITE_OPEN_READONLY, NULL);
    if (ret != SQLITE_OK)
    {
        printf("cannot open '%s': %s\n", ":memory:",
            sqlite3_errmsg(handle));
        sqlite3_close(handle);
        return -1;
    }
    cache = spatialite_alloc_connection();
    spatialite_init_ex(handle, cache, 0);


#ifndef OMIT_GEOS               /* GEOS must be enabled */

    /*
    Step #1
    creating and checking a POINT Geometry
    */

    /* we'll allocate a Geometry object */
    geo_pt = gaiaAllocGeomColl();
    /* then we insert a POINT, directly passing its coordinates */
    gaiaAddPointToGeomColl(geo_pt, 1.5, 2.75);
    /* now we'll print the main attributes for this geometry */
    printf("step#1: %s\t\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_pt)),
        gaiaDimension(geo_pt), gaiaIsValid_r(cache, geo_pt));
    geometry_printout(geo_pt);



    /*
    Step #2
    creating and checking a LINESTRING Geometry
    */
    geo_ln = gaiaAllocGeomColl();
    /* then we insert a  LINESTRING, specifying how many vertices it contains */
    line = gaiaAddLinestringToGeomColl(geo_ln, 5);
    /*
    we've got a pointer referencing the linestring we've just inserted
    now we'll set coordinates for each vertex
    */
    gaiaSetPoint(line->Coords, 0, 1.0, 1.0);
    gaiaSetPoint(line->Coords, 1, 2.0, 1.0);
    gaiaSetPoint(line->Coords, 2, 2.0, 2.0);
    gaiaSetPoint(line->Coords, 3, 100.0, 2.0);
    gaiaSetPoint(line->Coords, 4, 100.0, 100.0);


    printf("\nstep#2: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_ln)),
        gaiaDimension(geo_ln), gaiaIsValid_r(cache, geo_ln));
    geometry_printout(geo_ln);



    /*
    Step #3
    creating and checking a POLYGON Geometry
    */
    geo_pg = gaiaAllocGeomColl();
    /*
    then we insert a  POLYGON, specifying:
    - how many vertices have to be allocated for the Exterior Ring
    - how many Interior Rings it has
    */
    polyg = gaiaAddPolygonToGeomColl(geo_pg, 5, 2);
    /*
    we've got a pointer referencing the polygon we've just inserted
    now we'll get a pointer referencing its Exterior ring
    */
    ring = polyg->Exterior;


    /* now we'll set coordinates for each Exterior ring vertex */
    gaiaSetPoint(ring->Coords, 0, 0.0, 0.0);
    gaiaSetPoint(ring->Coords, 1, 50.0, 0.0);
    gaiaSetPoint(ring->Coords, 2, 50.0, 50.0);
    gaiaSetPoint(ring->Coords, 3, 0.0, 50.0);
    /* please note: a Ring is a CLOSED figure, so last and first vertex have to be coincident */
    gaiaSetPoint(ring->Coords, 4, 0.0, 0.0);


    /*
    we'll now get a pointer referencing the FIRST interior ring,
    specifying how vertices have to be allocated
    */
    ring = gaiaAddInteriorRing(polyg, 0, 5);
    /* then setting coordinates for each Interior ring vertex */
    gaiaSetPoint(ring->Coords, 0, 40.0, 40.0);
    gaiaSetPoint(ring->Coords, 1, 41.0, 40.0);
    gaiaSetPoint(ring->Coords, 2, 41.0, 41.0);
    gaiaSetPoint(ring->Coords, 3, 40.0, 41.0);
    gaiaSetPoint(ring->Coords, 4, 40.0, 40.0);


    /*
    we'll now get a pointer referencing the SECOND interior ring,
    specifying how vertices have to be allocated
    */
    ring = gaiaAddInteriorRing(polyg, 1, 5);
    /* then setting coordinates for each Interior ring vertex */
    gaiaSetPoint(ring->Coords, 0, 30.0, 30.0);
    gaiaSetPoint(ring->Coords, 1, 31.0, 30.0);
    gaiaSetPoint(ring->Coords, 2, 31.0, 31.0);
    gaiaSetPoint(ring->Coords, 3, 30.0, 31.0);
    gaiaSetPoint(ring->Coords, 4, 30.0, 30.0);


    printf("\nstep#3: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_pg)),
        gaiaDimension(geo_pg), gaiaIsValid_r(cache, geo_pg));
    geometry_printout(geo_pg);



    /*
    Step #4
    creating and checking a MULTIPOINT Geometry
    */
    geo_mpt = gaiaAllocGeomColl();
    /* then we'll insert some POINTs */
    gaiaAddPointToGeomColl(geo_mpt, 5.0, 5.0);
    gaiaAddPointToGeomColl(geo_mpt, 15.0, 5.0);
    gaiaAddPointToGeomColl(geo_mpt, 5.0, 15.0);
    gaiaAddPointToGeomColl(geo_mpt, 25.0, 5.0);
    gaiaAddPointToGeomColl(geo_mpt, 5.0, 25.0);
    printf("\nstep#4: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_mpt)),
        gaiaDimension(geo_mpt), gaiaIsValid_r(cache, geo_mpt));
    geometry_printout(geo_mpt);



    /*
    Step #5
    creating and checking a MULTILINESTRING Geometry
    */
    geo_mln = gaiaAllocGeomColl();

    /* then we'll insert two LINESTRINGs */
    line = gaiaAddLinestringToGeomColl(geo_mln, 2);
    gaiaSetPoint(line->Coords, 0, 30.0, 10.0);
    gaiaSetPoint(line->Coords, 1, 10.0, 30.0);

    line = gaiaAddLinestringToGeomColl(geo_mln, 2);
    gaiaSetPoint(line->Coords, 0, 40.0, 50.0);
    gaiaSetPoint(line->Coords, 1, 50.0, 40.0);

    printf("\nstep#5: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_mln)),
        gaiaDimension(geo_mln), gaiaIsValid_r(cache, geo_mln));
    geometry_printout(geo_mln);



    /*
    Step #6
    creating and checking a MULTIPOLYGON Geometry
    */
    geo_mpg = gaiaAllocGeomColl();

    /* then we'll insert two POLYGONs */
    polyg = gaiaAddPolygonToGeomColl(geo_mpg, 5, 0);
    ring = polyg->Exterior;
    gaiaSetPoint(ring->Coords, 0, 60.0, 60.0);
    gaiaSetPoint(ring->Coords, 1, 70.0, 60.0);
    gaiaSetPoint(ring->Coords, 2, 70.0, 70.0);
    gaiaSetPoint(ring->Coords, 3, 60.0, 70.0);
    gaiaSetPoint(ring->Coords, 4, 60.0, 60.0);

    polyg = gaiaAddPolygonToGeomColl(geo_mpg, 5, 0);
    ring = polyg->Exterior;
    gaiaSetPoint(ring->Coords, 0, 80.0, 80.0);
    gaiaSetPoint(ring->Coords, 1, 90.0, 80.0);
    gaiaSetPoint(ring->Coords, 2, 90.0, 90.0);
    gaiaSetPoint(ring->Coords, 3, 80.0, 90.0);
    gaiaSetPoint(ring->Coords, 4, 80.0, 80.0);

    printf("\nstep#6: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_mpg)),
        gaiaDimension(geo_mpg), gaiaIsValid_r(cache, geo_mpg));
    geometry_printout(geo_mpg);



    /*
    Step #7
    creating and checking a GEOMETRYCOLLECTION Geometry
    */
    geo_coll = gaiaAllocGeomColl();

    /* then we'll insert two POINTs */
    gaiaAddPointToGeomColl(geo_coll, 100.0, 100.0);
    gaiaAddPointToGeomColl(geo_coll, 100.0, 0.0);

    /* then we'll insert two LINESTRINGs */
    line = gaiaAddLinestringToGeomColl(geo_coll, 2);
    gaiaSetPoint(line->Coords, 0, 130.0, 110.0);
    gaiaSetPoint(line->Coords, 1, 110.0, 130.0);

    line = gaiaAddLinestringToGeomColl(geo_coll, 2);
    gaiaSetPoint(line->Coords, 0, 140.0, 150.0);
    gaiaSetPoint(line->Coords, 1, 150.0, 140.0);

    /* then we'll insert two POLYGONs */
    polyg = gaiaAddPolygonToGeomColl(geo_coll, 5, 0);
    ring = polyg->Exterior;
    gaiaSetPoint(ring->Coords, 0, 160.0, 160.0);
    gaiaSetPoint(ring->Coords, 1, 170.0, 160.0);
    gaiaSetPoint(ring->Coords, 2, 170.0, 170.0);
    gaiaSetPoint(ring->Coords, 3, 160.0, 170.0);
    gaiaSetPoint(ring->Coords, 4, 160.0, 160.0);

    polyg = gaiaAddPolygonToGeomColl(geo_coll, 5, 0);
    ring = polyg->Exterior;
    gaiaSetPoint(ring->Coords, 0, 180.0, 180.0);
    gaiaSetPoint(ring->Coords, 1, 190.0, 180.0);
    gaiaSetPoint(ring->Coords, 2, 190.0, 190.0);
    gaiaSetPoint(ring->Coords, 3, 180.0, 190.0);
    gaiaSetPoint(ring->Coords, 4, 180.0, 180.0);

    printf("\nstep#7: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_coll)),
        gaiaDimension(geo_coll), gaiaIsValid_r(cache, geo_coll));
    geometry_printout(geo_coll);




    /*
    Step #8
    printing each geometry as Well Known Text (WKT)
    */

    printf("\nstep#8: checking WKT representations\n");

    /* first we'll get the WKT corresponding to geometry */
    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_pt);
    /* we have to check wkt is not NULL */
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        /* printing the WKT */
        printf("\n%s\n", wkt.Buffer);

        /* finally freeing the wkt temporary storage allocation */
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_ln);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_pg);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_mpt);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_mln);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_mpg);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_coll);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }

#else
    printf("no GEOS support available: skipping any test\n");
#endif /* GEOS enabled/disabled */

    /*
    memory cleanup
    we have to destroy each object using temporary storage before exit
    */
    if (geo_pt)
        gaiaFreeGeomColl(geo_pt);
    if (geo_ln)
        gaiaFreeGeomColl(geo_ln);
    if (geo_pg)
        gaiaFreeGeomColl(geo_pg);
    if (geo_mpt)
        gaiaFreeGeomColl(geo_mpt);
    if (geo_mln)
        gaiaFreeGeomColl(geo_mln);
    if (geo_mpg)
        gaiaFreeGeomColl(geo_mpg);
    if (geo_coll)
        gaiaFreeGeomColl(geo_coll);
    sqlite3_close(handle);
    spatialite_cleanup_ex(cache);
    spatialite_shutdown();
    return 0;
}
