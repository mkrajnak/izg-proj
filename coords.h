/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: coords.h 34 2011-02-07 11:32:45Z spanel $
 *
 * Opravy a modifikace:
 * -
 */

#ifndef Coords_H
#define Coords_H

/******************************************************************************
 * Includes
 */

#include "vector.h"

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Definice typu a fci
 */

/* Souradnice vrcholu/vektoru ve 3D */
typedef struct S_Coords
{
    double  x, y, z;    /* souradnice vrcholu/vektoru */
} S_Coords;


/* Vytvori vrchol o zadanych souradnicich */
IZG_INLINE S_Coords makeCoords(double x, double y, double z)
{
    S_Coords coords;
    coords.x = x;
    coords.y = y;
    coords.z = z;
    return coords;
}

/* Normalizace vektoru na jednotkovou delku */
IZG_INLINE void coordsNormalize(S_Coords * v)
{
    double length, norm;
    IZG_ASSERT(v);

    length = v->x * v->x + v->y * v->y + v->z * v->z;
    if( length > 0.0 )
    {
        norm = 1.0 / sqrt(length);
        v->x *= norm;
        v->y *= norm;
        v->z *= norm;
    }
}


/******************************************************************************
 * Definice funkci a maker pro praci s vectorem souradnic
 */

USE_VECTOR_OF(S_Coords, cvec);

#define cvecGet(pVec, i)    (*cvecGetPtr((pVec), (i)))
#define S_CoordVec          S_Vector


#ifdef __cplusplus
}
#endif

#endif /* Coords_H */

/*****************************************************************************/
/*****************************************************************************/
