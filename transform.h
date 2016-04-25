/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: transform.h 263 2013-02-19 08:55:24Z spanel $
 *
 * Opravy a modifikace:
 * -
 */

#ifndef Transform_H
#define Transform_H

/******************************************************************************
 * Includes
 */

#include "coords.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Definice typu a fci pro praci se 3D transformacemi
 */

/* Transformacni matice ve 3D */
typedef struct S_Matrix
{
    double  data[16];   /* Prvky matice 4x4 */
} S_Matrix;


/* Inicializuje zadanou matici na jednotkovou */
void matUnit(S_Matrix *pMat);

/* Vrati prvek matice, indexovano od nuly. */
IZG_INLINE double * matGetPtr(S_Matrix *pMat, int row, int col)
{
    IZG_ASSERT(pMat && row >= 0 && row < 4 && col >= 0 && col < 4);
    return pMat->data + row * 4 + col;
}

/* Nastavi prvek matice, indexovano od nuly. */
IZG_INLINE void matSet(S_Matrix *pMat, int row, int col, double value)
{
    IZG_ASSERT(pMat && row >= 0 && row < 4 && col >= 0 && col < 4);
    *(pMat->data + row * 4 + col) = value;
}

/* Makro pro pristup k prvkum matice */
#define matGet(pMat, row, col)   (*(matGetPtr((pMat), (row), (col))))


/******************************************************************************
 * Globalni transformacni matice, ktere slouzi pro manipulaci se scenou
 * a vykreslovanymi objekty. Matice se aplikuji na vrcholy a normaly objektu
 * pred jeho vykreslenim do sceny.
 */

/* Transformacni matice pro manipulaci s objekty/scenou ve 3D */
extern S_Matrix model;

/* Samostatna projekcni matice pro perspektivni projekci */
extern S_Matrix projection;


/******************************************************************************
 * Operace nad globalni transformacni matici model.
 */

/* Inicializuje model matici na jednotkovou */
void trLoadIdentity();

/* Nastavi model matici na zadanou hodnotu */
void trSetMatrix(const S_Matrix *pMat);

/* Vraci aktualni hodnotu model matice. Vysledek ulozi do matice pMat. */
void trGetMatrix(S_Matrix *pMat);

/* Skladani transformaci (aktualni model matici vynasobi zadanou matici) */
void trMultMatrix(const S_Matrix *pMat);

/* Vynasobi model matici translacni matici. */
void trTranslate(double dx, double dy, double dz);

/* Vynasobi model matici rotacni matici kolem osy Y */
void trRotateY(double angle);

/* vynasobi model matici rotacni matici kolem osy X */
void trRotateX(double angle);

/* Vynasobi model matici scale matici */
void trScale(double dx, double dy, double dz);


/******************************************************************************
 * Operace nad globalni transformacni matici projection.
 */

/* Nastavi projekcni matici, ktera zaridi PERSPEKTIVNI promitani.
 * d > 0 - vzdalenost kamery na z-tove ose
 * width, height - velikost aktualniho okna (frame bufferu) */
void trProjectionPerspective(double d, int width, int height);


/******************************************************************************
 * Funkce, ktere aplikuji nastavene transformace
 */

/* Transformuje bod ve 3D pomoci matice model
 * p - puvodni souradnice ve 3D
 * p2 - nove souradnice */
void trTransformVertex(S_Coords *p2, const S_Coords *p);

/* Transformuje vektor ve 3D pomoci matice model */
void trTransformVector(S_Coords *v2, const S_Coords *v);

/* Promitne bod ve 3D na obrazovku pomoci projection matice
 * Fce jako navratovou hodnotu vraci vahu w pred normalizaci souradnic
 * p - souradnice ve 3D
 * u,v -  souradnice na obrazovce */
double trProjectVertex(int *u, int *v, const S_Coords *p);


#ifdef __cplusplus
}
#endif

#endif // Transform_H

/*****************************************************************************/
/*****************************************************************************/
