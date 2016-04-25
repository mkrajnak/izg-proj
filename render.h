/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: render.h 416 2015-03-10 08:07:46Z spanel $
 *
 * Opravy a modifikace:
 * - 2016/02 (MM): Pridana podpora vykresleni jednotlivych klicovych snimku.
 */

#ifndef Render_H
#define Render_H

/******************************************************************************
 * Includes
 */

#include "color.h"
#include "coords.h"
#include "model.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 * Sada funkci, ktere bude treba preimplementovat ve vasem studentskem
 * rendereru (viz. student.h a student.cpp)
 */

/* forward declaration
 * abychom mohli pouzivat pointer na strukturu S_Renderer,
 * i kdyz struktura neni definovana... */
typedef struct S_Renderer S_Renderer;

/* ukazatel na funkci pro zruseni struktury rendereru, dealokaci pameti, apod. */
typedef void (* F_release)(S_Renderer **ppRenderer);

/* ukazatel na funkci pro inicializaci frame bufferu */
typedef void (* F_createBuffers)(S_Renderer *pRenderer, int width, int height);

/* ukazatel na funkci, ktera vymaze frame buffer pred kreslenim */
typedef void (* F_clearBuffers)(S_Renderer *pRenderer);

/* ukazatel na funkci pro vykresleni i-teho trojuhelniku modelu */
typedef void (* F_projectTriangle)(S_Renderer *pRenderer, S_Model *pModel, int i, float n);

/* ukazatel na funkci pro vypocet osvetlovaciho modelu pro zadany bod a normalu */
typedef S_RGBA (* F_calcReflectance)(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal);


/******************************************************************************
 * Jadro jednoducheho SW renderingu jeste jednodussi sceny
 */

/* Typ/ID rendereru */
extern const int DEFAULT_RENDERER;

/* Jadro SW renderingu - frame buffer, nastaveni kamery, zdroj svetla, atd. */
struct S_Renderer
{
    /* typ rendereru: 0 - original, 1 - studentsky */
    int         type;

    /* frame buffer */
    S_RGBA      * frame_buffer;     /* ukazatel na pamet RGBA hodnot vlastniho frame bufferu */
    int         frame_w, frame_h;   /* velikost frame bufferu, tj. velikost okna na obrazovce */

    /* depth buffer */
    double      * depth_buffer;     /* ukazatel na pamet depth bufferu */    
    double      max_depth;          /* maximalni hodnota hloubky sceny */

    /* nastaveni kamery */
    int         camera_dist;        /* kamera lezi na ose Z v bode (0, 0, -camera_dist) */

    /* jednoduchy trackball pro manipulaci se scenou */
    double      scene_rot_x;        /* rotace sceny kolem osy X */
    double      scene_rot_y;        /* rotace sceny kolem osy Y */
    double      scene_move_z;       /* zoom sceny (posun sceny po ose Z) */
    double      scene_move_x;       /* posuv sceny v ose X */
    double      scene_move_y;       /* posuv sceny v ose Y */

    /* parametry materialu pro osvetlovaci model (vzdy pro RGB slozky, hodnoty 0..1) */
    S_Material  mat_ambient;        /* ambientni (rozptylena) slozka materialu */
    S_Material  mat_diffuse;        /* difuzni (matna) slozka materialu */
    S_Material  mat_specular;       /* spekularni (odleskova) slozka materialu */

    /* pozice zdroje svetla */
    S_Coords    light_position;     /* pozice svetla ve scene */

    /* parametry zdroje svetla (vzdy pro RGB slozky, hodnoty 0..1) */
    S_Light     light_ambient;      /* ambientni slozka svetla */
    S_Light     light_diffuse;      /* difuzni slozka svetla */
    S_Light     light_specular;     /* barva odlesku */

    /* ukazatele na funkce, ktere lze upravit pro novy typ rendereru */
    F_release           releaseFunc;            /* zruseni rendereru a uvolneni pameti */
    F_createBuffers     createBuffersFunc;      /* inicializace frame bufferu */
    F_clearBuffers      clearBuffersFunc;       /* vymazani frame bufferu */
    F_projectTriangle   projectTriangleFunc;    /* vykresleni trojuhelniku */
    F_calcReflectance   calcReflectanceFunc;    /* osvetlovaci model */
};


/* Funkce vytvori renderer a nainicializuje jej volanim fce renInit() */
S_Renderer * renCreate();

/* Funkce nainicializuje renderer do "default" stavu */
void renInit(S_Renderer *pRenderer);

/* Funkce vraci identifikator typu rendereru */
int renType(S_Renderer *pRenderer);

/* Funkce korektne zrusi renderer a uvolni pamet */
void renRelease(S_Renderer **ppRenderer);

/* Inicializuje frame buffer */
void renCreateBuffers(S_Renderer *pRenderer, int width, int height);

/* Funkce vymaze frame buffer pred kreslenim */
void renClearBuffers(S_Renderer *pRenderer);


/* Zmeni parametry materialu, ktere jsou pouzivany pri kresleni */
void renMatAmbient(S_Renderer *pRenderer, const S_Material *pParams);
void renMatDiffuse(S_Renderer *pRenderer, const S_Material *pParams);
void renMatSpecular(S_Renderer *pRenderer, const S_Material *pParams);

/* Nastaveni pozice svetelneho zdroje */
void renLightPosition(S_Renderer *pRenderer, const S_Coords *pPosition);

/* Nastaveni parametru zdroje svetla */
void renLightAmbient(S_Renderer *pRenderer, const S_Light *pParams);
void renLightDiffuse(S_Renderer *pRenderer, const S_Light *pParams);
void renLightSpecular(S_Renderer *pRenderer, const S_Light *pParams);


/* Rotace sceny podle X a Y osy
 * dx, dy - zmena polohy kursoru na obrazovce v pixelech */
void renSceneRotXY(S_Renderer *pRenderer, int dx, int dy);

/* Posuv sceny podel Z osy
 * dy - zmena polohy kursoru na obrazovce v pixelech */
void renSceneMoveZ(S_Renderer *pRenderer, int dy);

/* Posuv sceny v rovine XY
 * dx - zmena polohy v ose X v pixelech 
 * dy - zmena polohy v ose Y v pixelech */
void renSceneMoveXY(S_Renderer *pRenderer, int dx, int dy);

/* Prida do model matice transformace, ktere manipuluji se scenou */
void renSetupTrackball(S_Renderer *pRenderer);

/* Nastavi perspektivni projekci do projection matice */
void renSetupProjection(S_Renderer *pRenderer);


/* Funkce vyhodnoti Lambertuv osvetlovaci model pro zadany bod
 * a vraci barvu, kterou se bude kreslit do frame bufferu
 * Pokud je aktualne nastaven polopruhledny material, vynasobi funkce RGB
 * slozky barvy alpha slozkou a vraci: (R * A/255, G * A/255, B * A/255, A)
 * point - souradnice bodu v prostoru
 * normal - normala k povrchu v danem bode */
S_RGBA renLambertianReflectance(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal);

/* Funkce otestuje zda je zadana "ploska" privracena ke kamere, tudiz viditelna
 * point - bod lezici na plosce
 * normal - normala plosky */
int renCalcVisibility(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal);


/* Vykresli i-ty trojuhelnik modelu n-teho klicoveho snimku
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku
 * n - index klicoveho snimku
 */
void renProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i, float n);


/******************************************************************************
 * Uzitecna makra
 */

/* Makro pro primy pristup k hodnote pixelu ve frame bufferu */
#define PIXEL(pRenderer, x, y) \
    (*((pRenderer)->frame_buffer + (y) * (pRenderer)->frame_w + (x)))

/* Makro pro primy pristup k hodnote v depth bufferu */
#define DEPTH(pRenderer, x, y) \
    (*((pRenderer)->depth_buffer + (y) * (pRenderer)->frame_w + (x)))


#ifdef __cplusplus
}
#endif

#endif /* Render_H */

/*****************************************************************************/
/*****************************************************************************/
