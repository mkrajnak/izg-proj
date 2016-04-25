/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: bmp.h 416 2015-03-10 08:07:46Z spanel $
 *
 * Opravy a modifikace:
 * - 2016/2 (MM): Nacitani BMP pres SDL knihovnu
 */

#ifndef Bmp_H
#define Bmp_H

/******************************************************************************
 * Includes
 */

#include "color.h"
#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Prace s grafickym formatem BMP
 */

/* Ulozeni obrazoveho bufferu do souboru ve formatu BMP.
 * filename - jmeno souboru
 * buffer - ukazatel na ukladana obrazova data
 * width, height - sirka a vyska obrazu
 * funkce vraci 0 - OK, -1 - chyba */
int saveBitmap(const char *filename, S_RGBA *buffer, int width, int height);

/* Nacteni textury ve formatu BMP.
* filename - jmeno souboru
* width, height - ukazatel na 
* funkce vraci ukazatel na nactena data (S_RGBA pole) */
S_RGBA * loadBitmap( const char * filename, int * width, int * height );

/* Ziskani uint32 hodnoty pixelu v surface na souradnicich x, y */
Uint32 getpixel( SDL_Surface *surface, int x, int y );

#ifdef __cplusplus
}
#endif

#endif // Bmp_H

/*****************************************************************************/
/*****************************************************************************/
