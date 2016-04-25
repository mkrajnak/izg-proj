/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: vector_types.h 416 2015-03-10 08:07:46Z spanel $
 *
 * Opravy a modifikace:
 * -
 */

#ifndef VectorTypes_H
#define VectorTypes_H

/******************************************************************************
 * Includes
 */

#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Definice funkci a maker pro praci s vektory nekterych zakladnich typu
 */

/* vektor intu - ivec */
USE_VECTOR_OF(int, ivec);

#define ivecGet(pVec, i)    (*ivecGetPtr((pVec), (i)))
#define S_IntVec            S_Vector


/* vektor doublu - dvec */
USE_VECTOR_OF(double, dvec);

#define dvecGet(pVec, i)    (*dvecGetPtr((pVec), (i)))
#define S_DoubleVec         S_Vector


#ifdef __cplusplus
}
#endif

#endif /* VectorTypes_H */

/*****************************************************************************/
/*****************************************************************************/
