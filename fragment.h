/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: fragment.h 416 2015-03-10 08:07:46Z spanel $
 *
 * Opravy a modifikace:
 * -
 */

#ifndef Fragment_H
#define Fragment_H

/******************************************************************************
 * Includes
 */

#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Definice typu a fci pro reprezentaci vykreslovaneho fragmentu
 */

/* Struktura reprezentujici fragment */
typedef struct S_Frag
{
    S_RGBA      color;      /* barva fragmentu/pixelu */
    double      depth;      /* vzdálenost od kamery */
    int         next;       /* index nasledujiciho fragmentu v seznamu nebo -1 */
} S_Frag;


/* Vytvori novy fragment */
IZG_INLINE S_Frag makeFrag(S_RGBA color, double depth, int next)
{
    S_Frag frag;
    frag.color = color;
    frag.depth = depth;
    frag.next = next;
    return frag;
}


/******************************************************************************
 * Definice funkci a maker pro praci s vectorem fragmentu
 */

USE_VECTOR_OF(S_Frag, fragvec);

#define fragvecGet(pVec, i)     (*fragvecGetPtr((pVec), (i)))
#define S_FragVec               S_Vector


#ifdef __cplusplus
}
#endif

#endif // Fragment_H

/*****************************************************************************/
/*****************************************************************************/
