/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: color.h 416 2015-03-10 08:07:46Z spanel $
 *
 * Opravy a modifikace:
 * -
 */

#ifndef Color_H
#define Color_H

/******************************************************************************
 * Includes
 */

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Definice typu a fci pro reprezentaci RGBA barvy
 */

/* Struktura reprezentujici RGBA barvu */
typedef struct S_RGBA
{
    unsigned char   red, green, blue, alpha;
} S_RGBA;


/* Vytvori cernou barvu */
IZG_INLINE S_RGBA makeBlackColor()
{
    S_RGBA color;
    color.red = color.green = color.blue = 0;
    color.alpha = 255;
    return color;
}

/* Vytvori barvu o zadanych RGB hodnotach */
IZG_INLINE S_RGBA makeColor(unsigned char r, unsigned char g, unsigned char b)
{
    S_RGBA color;
    color.red = r;
    color.green = g;
    color.blue = b;
    color.alpha = 255;
    return color;
}

/* Vytvori barvu o zadanych RGBA hodnotach */
IZG_INLINE S_RGBA makeColorA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    S_RGBA color;
    color.red = r;
    color.green = g;
    color.blue = b;
    color.alpha = a;
    return color;
}

/* Konstanty barev pouzivane v programu */
extern const S_RGBA     COLOR_BLACK;
extern const S_RGBA     COLOR_BLUE;
extern const S_RGBA     COLOR_RED;


/******************************************************************************
 * Typy a fce pro popis zdroje svetla.
 */

/* Parametry zdroje svetla */
typedef struct S_Light
{
    double  red, green, blue;   /* jednotlive barevne slozky, hodnoty 0..1 */
} S_Light;


/* Vytvori strukturu nesouci nastaveni zdroje svetla */
IZG_INLINE S_Light makeLight(double r, double g, double b)
{
    S_Light light;
    light.red = r;
    light.green = g;
    light.blue = b;
    return light;
}


/******************************************************************************
 * Typy a fce pro popis materialu.
 */

/* Parametry materialu se zadavaji stejne jako parametry zdroje svetla */
typedef struct S_Material
{
    double  red, green, blue, alpha;   /* jednotlive barevne slozky a pruhlednost, hodnoty 0..1 */
} S_Material;


/* Vytvori strukturu nesouci nastaveni materialu */
IZG_INLINE S_Material makeMaterial(double r, double g, double b)
{
    S_Material mat;
    mat.red = r;
    mat.green = g;
    mat.blue = b;
    mat.alpha = 1.0;
    return mat;
}


/* Vytvori strukturu nesouci nastaveni castecne pruhledneho materialu */
IZG_INLINE S_Material makeMaterialA(double r, double g, double b, double a)
{
    S_Material mat;
    mat.red = r;
    mat.green = g;
    mat.blue = b;
    mat.alpha = a;
    return mat;
}

/* Konstanty materialu pouzivane v programu */
extern const S_Material     MAT_RED_AMBIENT;
extern const S_Material     MAT_RED_DIFFUSE;
extern const S_Material     MAT_RED_SPECULAR;


#ifdef __cplusplus
}
#endif

#endif // Color_H

/*****************************************************************************/
/*****************************************************************************/
