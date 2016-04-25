/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: model.c 339 2014-02-26 07:52:28Z spanel $
 *
 * Opravy a modifikace:
 * - 2014/02: Pridana podpora pro texturovaci souradnice.
 * - 2016/02: Pridana podpora pro klicove snimky a nacitani tex. souradnic.
 */

#include "model.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


/*****************************************************************************/

S_Model * modCreate()
{
    S_Model * model = (S_Model *)malloc(sizeof(S_Model));
    IZG_CHECK(model, "Cannot allocate enough memory");

    model->vertices = cvecCreateEmpty();
    model->normals = cvecCreateEmpty();
    model->triangles = trivecCreateEmpty();
    model->trinormals = cvecCreateEmpty();
    return model;
}

/*****************************************************************************/

S_Model * modLoad( const char * filename )
{
    FILE        * f;
    int         n, t, k, i, j, v0, v1, v2;
    S_Model     * model;
    S_Coords    * vertex, *normal;
    S_Coords    a, b, c, tex0, tex1, tex2;
    S_Triangle  * triangle;
    int         fv0, fv1, fv2;

    /* otevri vstupni soubor */
    if ( ( f = fopen( filename, "r" ) ) == NULL )
    {
        return NULL;
    }

    /* nacteni hlavicky */
    if ( fscanf( f, "tri %d.%d\n", &v0, &v1 ) != 2 )
    {
        return NULL;
    }

    /* nacteni poctu vrcholu */
    if ( fscanf( f, "vertices %d\n", &n ) != 1 || n < 0 )
    {
        return NULL;
    }

    /* nacteni poctu trojuhelniku */
    if ( fscanf( f, "faces %d\n", &t ) != 1 || t < 0 )
    {
        return NULL;
    }

    /* nacteni poctu klicovych snimku */
    if ( fscanf( f, "frames %d\n", &k ) != 1 || k <= 0 )
    {
        /* jediny snimek (zpetna kompatibilita) */
        k = 1;
    }

    /* vytvor novy model */
    model = modCreate();
    model->frames = k;
    model->verticesPerFrame = n / k;
    cvecResize( model->vertices, n );
    cvecResize( model->normals, n );
    trivecResize( model->triangles, t );
    cvecResize( model->trinormals, t * k );

    /* nacteni vrcholu a soucasne vymazani vsech normal ve vrcholech */
    for ( i = 0; i < n; ++i )
    {
        vertex = cvecGetPtr( model->vertices, i );
        if ( fscanf( f, "%lf %lf %lf\n", &vertex->x, &vertex->y, &vertex->z ) != 3 )
        {
            modRelease( &model );
            return NULL;
        }

        /* vymazani odpovidajici normaly */
        normal = cvecGetPtr( model->normals, i );
        normal->x = 0;
        normal->y = 0;
        normal->z = 0;
    }

    /* nacteni trojuhelniku a akumulace jejich vypoctene normaly ve vrcholech */
    for ( i = 0; i < t; ++i )
    {
        if ( fscanf( f, "%d %d %d\n", &v0, &v1, &v2 ) != 3
            || v0 < 0 || v0 >= n
            || v1 < 0 || v1 >= n
            || v2 < 0 || v2 >= n )
        {
            modRelease( &model );
            return NULL;
        }


        /* novy trojuhelnik - pro vsechny klicove snimky stejne trojuhelniky (pouze posun indexu) */
        trivecGet( model->triangles, i ) = makeTriangle( v0, v1, v2, i );

        /* pro vsechny klicove snimky spocitame normalu aktualniho trojuhelniku a souvisejicich vrcholu */
        for ( j = 0; j < k; ++j )
        {
            /* indexy vrcholu aktualniho trojuhelniku v aktualnim klicovem snimku */
            fv0 = v0 + model->verticesPerFrame * j;
            fv1 = v1 + model->verticesPerFrame * j;
            fv2 = v2 + model->verticesPerFrame * j;

            /* vektor v0 -> v1 */
            a.x = cvecGet( model->vertices, fv1 ).x - cvecGet( model->vertices, fv0 ).x;
            a.y = cvecGet( model->vertices, fv1 ).y - cvecGet( model->vertices, fv0 ).y;
            a.z = cvecGet( model->vertices, fv1 ).z - cvecGet( model->vertices, fv0 ).z;

            /* vektor v1 -> v2 */
            b.x = cvecGet( model->vertices, fv2 ).x - cvecGet( model->vertices, fv0 ).x;
            b.y = cvecGet( model->vertices, fv2 ).y - cvecGet( model->vertices, fv0 ).y;
            b.z = cvecGet( model->vertices, fv2 ).z - cvecGet( model->vertices, fv0 ).z;

            /* vypocet normaly trojuhelniku */
            c.x = a.y * b.z - a.z * b.y;
            c.y = a.z * b.x - a.x * b.z;
            c.z = a.x * b.y - a.y * b.x;
            coordsNormalize( &c );

            /* ulozeni normaly */
            cvecGet( model->trinormals, i + j * t ) = c;

            /* akumulace normaly ve vrcholech trojuhelniku (~interpolace normal ve vrcholech) */
            normal = cvecGetPtr( model->normals, fv0 );
            normal->x += c.x;
            normal->y += c.y;
            normal->z += c.z;

            normal = cvecGetPtr( model->normals, fv1 );
            normal->x += c.x;
            normal->y += c.y;
            normal->z += c.z;

            normal = cvecGetPtr( model->normals, fv2 );
            normal->x += c.x;
            normal->y += c.y;
            normal->z += c.z;
        }
    }

    /* normalizace normal */
    for ( i = 0; i < n; ++i )
    {
        normal = cvecGetPtr( model->normals, i );
        coordsNormalize( normal );
    }

    /* nacteni texturovacich souradnic - pokud nejsme na konci souboru (zpetna kompatibilita) */
    if ( feof( f ) == 0 ) {
        tex0.z = 0;
        tex1.z = 0;
        tex2.z = 0;
        for ( i = 0; i < t; ++i )
        {
            if ( fscanf( f, "%lf %lf %lf %lf %lf %lf\n", &tex0.x, &tex0.y, &tex1.x, &tex1.y, &tex2.x, &tex2.y ) != 6 )
            {
                modRelease( &model );
                return NULL;
            }
            triangle = trivecGetPtr( model->triangles, i );
            triangle->t[ 0 ] = tex0;
            triangle->t[ 1 ] = tex1;
            triangle->t[ 2 ] = tex2;
        }
    }

    /* vse nacteno v poradku... */
    return model;
}

/*****************************************************************************/

void modRelease(S_Model **ppModel)
{
    if( ppModel && *ppModel )
    {
        cvecRelease(&(*ppModel)->vertices);
        cvecRelease(&(*ppModel)->normals);
        trivecRelease(&(*ppModel)->triangles);
        cvecRelease(&(*ppModel)->trinormals);

        free(*ppModel);
        *ppModel = NULL;
    }
}


/*****************************************************************************/
/*****************************************************************************/
