/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: transform.c 264 2013-02-20 12:27:25Z spanel $
 *
 * Opravy a modifikace:
 * -
 */

#include "transform.h"

#include <math.h>


/****************************************************************************
 * Globalni promenne
 */

/* Transformacni matice objektu/sceny */
S_Matrix model;

/* Samostatna projekcni matice */
S_Matrix projection;

/* Aktualni velikost okna */
int frame_height = 0, frame_width = 0;


/****************************************************************************
 * Implementace fci
 */

void matUnit(S_Matrix *pMat)
{
    int i, j;

    IZG_ASSERT(pMat);

    for( j = 0; j < 4; ++j )
    {
        for( i = 0; i < 4; ++i )
        {
            pMat->data[j * 4 + i] = (i == j) ? 1.0 : 0.0;
        }
    }
}


/****************************************************************************
 * Operace nad model matici
 */

void trLoadIdentity()
{
    matUnit(&model);
}

/*****************************************************************************/

void trSetMatrix(const S_Matrix *pMat)
{
    IZG_ASSERT(pMat);

    MEMCOPY(model.data, pMat->data, 16 * sizeof(double));
}

/*****************************************************************************/

void trGetMatrix(S_Matrix *pMat)
{
    IZG_ASSERT(pMat);

    MEMCOPY(pMat->data, model.data, 16 * sizeof(double));
}

/*****************************************************************************/

void trMultMatrix(const S_Matrix *pMat)
{
    S_Matrix    temp;
    int         j, i, k;

    IZG_ASSERT(pMat);

    /* kopie puvodni matice */
    trGetMatrix(&temp);

    /* a tu je algoritmus nasobeni dvou matic, podle uciva stredni skoly */
    for( j = 0; j < 4; ++j )
    {
        for( i = 0; i < 4; ++i )
        {
            model.data[j * 4 + i] = 0.0;
            for( k = 0; k < 4; ++k )
            {
                model.data[j * 4 + i] += pMat->data[j * 4 + k] * temp.data[k * 4 + i];
            }
        }
    }
}

/*****************************************************************************/

void trTranslate(double dx, double dy, double dz)
{
    S_Matrix m;

    matUnit(&m);
    matGet(&m, 3, 0) = dx;
    matGet(&m, 3, 1) = dy;
    matGet(&m, 3, 2) = dz;

    trMultMatrix(&m);
}

/*****************************************************************************/

void trRotateY(double angle)
{
    S_Matrix    m;
    double      sina, cosa;

    /* aby se goniometricke funkce nepocitaly dvakrat, kdyz to staci jednou... */
    sina = sin(angle);
    cosa = cos(angle);

    matUnit(&m);
    matGet(&m, 0, 0) = cosa;
    matGet(&m, 0, 2) = sina;
    matGet(&m, 2, 0) = -sina;
    matGet(&m, 2, 2) = cosa;

    trMultMatrix(&m);
}

/*****************************************************************************/

void trRotateX(double angle)
{
    S_Matrix    m;
    double      sina, cosa;

    /* aby se goniometricke funkce nepocitaly dvakrat, kdyz to staci jednou... */
    sina = sin(angle);
    cosa = cos(angle);

    matUnit(&m);
    matGet(&m, 1, 1) = cosa;
    matGet(&m, 1, 2) = sina;
    matGet(&m, 2, 1) = -sina;
    matGet(&m, 2, 2) = cosa;

    trMultMatrix(&m);
}

/*****************************************************************************/

void trScale(double sx, double sy, double sz)
{
    S_Matrix m;

    matUnit(&m);
    matGet(&m, 0, 0) = sx;
    matGet(&m, 1, 1) = sy;
    matGet(&m, 2, 2) = sz;

    trMultMatrix(&m);
}


/****************************************************************************
 * Operace nad projection matici
 */

void trProjectionPerspective(double d, int width, int height)
{
    IZG_ASSERT(d > 0);

    matUnit(&projection);
    matGet(&projection, 2, 2) = 0.0;
    matGet(&projection, 2, 3) = 1.0 / d;

    /* schovame si velikost okna... */
    frame_width = width;
    frame_height = height;
}


/****************************************************************************
 * Funkce, ktere aplikuji nastavene transformacni matice
 */

void trTransformVertex(S_Coords *p2, const S_Coords *p)
{
    IZG_ASSERT(p2 && p);

    /* vynasobeni vektoru (x,y,z,1) transformacni matici */
    p2->x = p->x * matGet(&model, 0, 0) + p->y * matGet(&model, 1, 0) + p->z * matGet(&model, 2, 0) + matGet(&model, 3, 0);
    p2->y = p->x * matGet(&model, 0, 1) + p->y * matGet(&model, 1, 1) + p->z * matGet(&model, 2, 1) + matGet(&model, 3, 1);
    p2->z = p->x * matGet(&model, 0, 2) + p->y * matGet(&model, 1, 2) + p->z * matGet(&model, 2, 2) + matGet(&model, 3, 2);
}

/*****************************************************************************/

void trTransformVector(S_Coords *v2, const S_Coords *v)
{
    IZG_ASSERT(v2 && v);

    /* vynasobeni vektoru (x,y,z,0) transformacni matici */
    v2->x = v->x * matGet(&model, 0, 0) + v->y * matGet(&model, 1, 0) + v->z * matGet(&model, 2, 0);
    v2->y = v->x * matGet(&model, 0, 1) + v->y * matGet(&model, 1, 1) + v->z * matGet(&model, 2, 1);
    v2->z = v->x * matGet(&model, 0, 2) + v->y * matGet(&model, 1, 2) + v->z * matGet(&model, 2, 2);
}

/*****************************************************************************/

double trProjectVertex(int *u, int *v, const S_Coords *p)
{
    double xx, yy, ww, iww;

    IZG_ASSERT(u && v && p);

    /* vynasobeni vektoru (x,y,z,1) projekcni matici */
    xx = p->x;
    yy = p->y;
    ww = p->x * matGet(&projection, 0, 3) + p->y * matGet(&projection, 1, 3)
       + p->z * matGet(&projection, 2, 3) + matGet(&projection, 3, 3);

    /* homogenni souradnice */
    iww = 1.0 / ww;
    xx *= iww;
    yy *= iww;

    /* zaokrouhleni promitnutych souradnic a posun na stred obrazovky */
    *u = (int)(xx + (frame_width >> 1));
    *v = (int)(yy + (frame_height >> 1));

    /* vracime ctvrtou homogenni souradnici */
    return ww;
}


/*****************************************************************************/
/*****************************************************************************/
