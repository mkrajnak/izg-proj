/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: main.c 416 2015-03-10 08:07:46Z spanel $
 *
 * *) Ovladani programu:
 *      mys          - rotace a zoom sceny
 *      kolecko mysi - zoom sceny
 *      'Esc', 'q'   - ukonceni programu
 *      'f'          - prepnuti mezi full screen a window mode (pouze linux)
 *      'p'          - prepne na studentsky renderer
 *      'o'          - originalni renderer
 *      's'          - ulozi screenshot frame bufferu
 *      sipky        - pohyb kamery
 *
 * Opravy a modifikace:
 * - 2016/02 (MM): Pridana podpora vykresleni jednotlivych klicovych snimku.
 * - 2015/03 (MS): moznost ulozeni screenshotu
 * - 2013/02 (MikitaM, MS): vylepsene ovladani, prepinani rendereru klavesou
 * - 2013/02 (MS): pridana podpora pro casovac
 * - 2013/02 (MS, xkonam00): oprava timer callbacku pro 64bit kompilaci
 */

/******************************************************************************
 * Includes
 */

#ifdef _WIN32
    #include <windows.h>
#endif

/* knihovna SDL */
#include "SDL.h"

/* nase veci... */
#include "render.h"
#include "model.h"
#include "transform.h"
#include "bmp.h"
#include "student.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>


/******************************************************************************
 * Prepnuti default rendereru na studentsky
 */

/* Je-li makro definovano vytvori se hned po startu vas studentsky renderer */
//#define USE_STUDENT_RENDERER


/******************************************************************************
 * Globalni konstanty a promenne
 */

/* titulek hlavniho okna je-li pouzit originalni renderer */
const char         * PROGRAM_TITLE = "IZG/2015 Project";

/* titulek hlavniho okna je-li zapnuty studentsky renderer */
const char         * PROGRAM_TITLE_STUDENT = "IZG/2015 Project -- Student";

/* standardni vstupni model */
const char         * DEFAULT_MODEL = "models/yoshi.tri";
/*const char         * DEFAULT_MODEL  = "models/marvin.tri";*/
/*const char         * DEFAULT_MODEL  = "models/ogro.tri";*/
/*const char         * DEFAULT_MODEL  = "models/doomguy.tri";*/
/*const char         * DEFAULT_MODEL  = "models/plane.tri";*/
/*const char         * DEFAULT_MODEL  = "models/valec.tri";*/
/*const char         * DEFAULT_MODEL  = "models/koule.tri";*/
/*const char         * DEFAULT_MODEL  = "models/skull.tri";*/
/*const char         * DEFAULT_MODEL  = "models/bunny.tri";*/

/* standardni vstupni textura */
char                 * DEFAULT_TEXTURE = "models/yoshi.bmp";
/*const char         * DEFAULT_TEXTURE  = "models/marvin.bmp";*/
/*const char         * DEFAULT_TEXTURE  = "models/ogro.bmp";*/
/*const char         * DEFAULT_TEXTURE  = "models/doomguy.bmp";*/
/*const char         * DEFAULT_TEXTURE  = "models/plane.bmp";*/

/* defaultni velikost okna */
const int          DEFAULT_WIDTH    = 500;
const int          DEFAULT_HEIGHT   = 500;

/* vykreslovany model */
S_Model             * polymodel     = NULL;

/* nas SW renderer */
S_Renderer          * renderer      = NULL;

/* kreslici buffer knihovny SDL */
SDL_Surface         * screen        = NULL;

/* pomocna promenna pro ukonceni aplikace */
int                 quit            = 0;

/* prodleva casovace v ms */
const int           TIMER_INTERVAL  = 33;

/* nase ID pro udalost casovace */
const int           IZG_TIMER_EVENT = 18;

/* jmeno souboru pri ulozeni screenshotu */
const char         * SCR_FILENAME   = "screen.bmp";


/******************************************************************************
 ******************************************************************************
 * Funkce pro vyrenderovani modelu slozeneho z trojuhelniku
 * n - snimek modelu pro vykresleni
 */

void renderModel(S_Renderer *pRenderer, S_Model *pModel, float n)
{
    int         i;
    
    IZG_ASSERT(pRenderer && pModel);
    
    /* pomoci rendereru postupne vykreslime vsechny trojuhelniky modelu */
    for( i = 0; i < trivecSize(pModel->triangles); ++i )
    {
        /* zavolame rutinu pro vykresleni */
        pRenderer->projectTriangleFunc(pRenderer, pModel, i, n);
    }
}

/******************************************************************************
 * Funkce pro vyrenderovani zakladni sceny, tj. vykresleni modelu
 */

void renderDefaultScene(S_Renderer *pRenderer, S_Model *pModel)
{
    /* test existence frame bufferu a modelu */
    IZG_ASSERT(pModel && pRenderer);

    /* nastavit projekcni matici */
    trProjectionPerspective(pRenderer->camera_dist, pRenderer->frame_w, pRenderer->frame_h);

    /* vycistit model matici */
    trLoadIdentity();

    /* nejprve nastavime posuv cele sceny od/ke kamere */
    trTranslate(0.0, 0.0, pRenderer->scene_move_z);

    /* nejprve nastavime posuv cele sceny v rovine XY */
    trTranslate(pRenderer->scene_move_x, pRenderer->scene_move_y, 0.0);

    /* natoceni cele sceny - jen ve dvou smerech - mys je jen 2D... :( */
    trRotateX(pRenderer->scene_rot_x);
    trRotateY(pRenderer->scene_rot_y);

    /* nastavime material */
    renMatAmbient(pRenderer, &MAT_RED_AMBIENT);
    renMatDiffuse(pRenderer, &MAT_RED_DIFFUSE);
    renMatSpecular(pRenderer, &MAT_RED_SPECULAR);

    /* a vykreslime nas model (ve vychozim stavu kreslime pouze snimek 0) */
    renderModel(pRenderer, pModel, 0);
}


/******************************************************************************
 ******************************************************************************
 * Funkce zajistujici prekresleni obsahu okna programu
 */

void draw(void)
{
    int     width, height, bpp, x, y;
    Uint8   * dst;
    S_RGBA  * src;

    /* test existence rendereru */
    IZG_ASSERT(renderer && screen && polymodel);

    /* vymazani frame bufferu */
    renderer->clearBuffersFunc(renderer);

    /* vykresleni sceny!!! */
    if( renType(renderer) == STUDENT_RENDERER )
    {
        renderStudentScene(renderer, polymodel);
    }
    else
    {
        renderDefaultScene(renderer, polymodel);
    }

    /* velikost bufferu */
    height = MIN(renderer->frame_h, screen->h);
    width = MIN(renderer->frame_w, screen->w);

    /* test typu pixelu */
    bpp = screen->format->BytesPerPixel;
    IZG_ASSERT(bpp == 4);

    /* kopie bufferu do obrazove pameti */
    SDL_LockSurface(screen);
    for( y = 0; y < height; ++y )
    {
        dst = (Uint8 *)screen->pixels + y * screen->pitch;
        src = renderer->frame_buffer + y * renderer->frame_w;
        for( x = 0; x < width; ++x, dst += bpp, ++src )
        {
            *((Uint32 *)dst) = SDL_MapRGB(screen->format, src->red, src->green, src->blue);
        }
    }
    SDL_UnlockSurface(screen);

    /* vymena zobrazovaneho a zapisovaneho bufferu */
    SDL_Flip(screen);
}


/******************************************************************************
 ******************************************************************************
 * Funkce volana pri startu aplikace
 */

void onInit(S_Renderer *pRenderer, S_Model *pModel)
{
    /* v tomto projektu nevyuzivame */
}

/******************************************************************************
 * Funkce reagujici na zmenu rozmeru okna
 * width, height - nova velikost okna
 */

void onResize(SDL_ResizeEvent *resize)
{
    /* test existence rendereru */
    IZG_ASSERT(renderer);

    /* nastaveni noveho frame bufferu v rendereru */
    renderer->createBuffersFunc(renderer, resize->w, resize->h);

    /* zmena velikosti obrazove pameti v SDL knihovne */
    SDL_SetVideoMode(resize->w, resize->h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
}

/******************************************************************************
 * Funkce reagujici na stisknuti klavesnice
 * key - udalost klavesnice
 */

void onKeyboard(SDL_KeyboardEvent *key)
{
    int width, height;

    /* test existence rendereru */
    IZG_ASSERT(renderer);

    /* vetveni podle stisknute klavesy */
    switch( key->keysym.sym )
    {
        /* ukonceni programu - klavesa Esc nebo q */
        case SDLK_ESCAPE:
        case SDLK_q:
            quit = 1;
            break;

        /* prepnuti do celoobrazovkoveho modu */
        case SDLK_f:
            SDL_WM_ToggleFullScreen(screen);
            break;

        /* prepnuti na studentsky renderer */
        case SDLK_p:
            /* zapamatovani aktualni velikosti frame bufferu */
            width = renderer->frame_w;
            height = renderer->frame_h;

            /* zruseni a znovu vytvoreni rendereru */
            renderer->releaseFunc(&renderer);
            renderer = studrenCreate();
            renderer->createBuffersFunc(renderer, width, height);

            /* zmena titulku okna */
            SDL_WM_SetCaption(PROGRAM_TITLE_STUDENT, 0);
            break;

        /* prepnuti na originalni renderer */
        case SDLK_o:
            width = renderer->frame_w;
            height = renderer->frame_h;
            renderer->releaseFunc(&renderer);
            renderer = renCreate();
            renderer->createBuffersFunc(renderer, width, height);
            SDL_WM_SetCaption(PROGRAM_TITLE, 0);
            break;

        /* posun kamery pomoci klaves */
        case SDLK_DOWN:
            renSceneMoveXY(renderer, 0, 1);
            break;
        case SDLK_UP:
            renSceneMoveXY(renderer, 0, -1);
            break;
        case SDLK_LEFT:
            renSceneMoveXY(renderer, -1, 0);
            break;
        case SDLK_RIGHT:
            renSceneMoveXY(renderer, 1, 0);
            break;

        /* ulozeni screenshotu */
        case SDLK_s:
            saveBitmap(SCR_FILENAME, renderer->frame_buffer, renderer->frame_w, renderer->frame_h);
            break;

        default:
            break;
    }
}

/******************************************************************************
 * Funkce reagujici na pohyb mysi
 * mouse - udalost mysi
 */

void onMouseMotion(SDL_MouseMotionEvent *mouse)
{
    if( mouse->state & SDL_BUTTON_LMASK )
    {
        /* rotace cele sceny */
        renSceneRotXY(renderer, mouse->yrel, mouse->xrel);
    }
    else if( mouse->state & SDL_BUTTON_RMASK )
    {
        /* posuv kamery */
        renSceneMoveZ(renderer, mouse->yrel);
    }
}

/******************************************************************************
 * Funkce reagujici na stisk tlacitka mysi a kolecko
 * mouse - udalost mysi
 */

void onMouseButton(SDL_MouseButtonEvent *mouse)
{
    /* zoom kamery */
    if( mouse->button == SDL_BUTTON_WHEELUP )
    {
        renSceneMoveZ(renderer, -1);
    }
    else if( mouse->button == SDL_BUTTON_WHEELDOWN )
    {
        renSceneMoveZ(renderer, 1);
    }
}


/******************************************************************************
 ******************************************************************************
 * Callback funkce casovace, ktera vygeneruje novou udalost SDL knihovny
 * Udalost bude odchycena v hlavni kreslici smycce
 */

Uint32 timerCallback(Uint32 interval, void *param)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = IZG_TIMER_EVENT;
    event.user.data1 = (void *)0;
    event.user.data2 = (void *)0;
    
    SDL_PushEvent(&event);

    return interval;
}


/******************************************************************************
 ******************************************************************************
 * Hlavni funkce programu
 * argc - pocet vstupnich parametru
 * argv - pole vstupnich parametru
 */

int main(int argc, char *argv[])
{
    SDL_Event       event;
    SDL_TimerID     timer = 0;

    /* nacteni modelu */
    polymodel = modLoad((argc >= 2) ? argv[1] : DEFAULT_MODEL);
    IZG_CHECK(polymodel, "Failed to read input model");

    /* nazev textury pro nacteni studentskym rendererem */
    TEXTURE_FILENAME = (argc >= 3) ? argv[2] : DEFAULT_TEXTURE;

    /* vytvoreni default SW rendereru */
#ifdef USE_STUDENT_RENDERER
    renderer = studrenCreate();
#else
    renderer = renCreate();
#endif /* USE_STUDENT_RENDERER */

    /* pocatecni velikost bufferu */
    renderer->createBuffersFunc(renderer, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    /* inicializace SDL knihovny */
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 )
    {
        IZG_SDL_ERROR("Could not initialize SDL library");
    }

    /* nastaveni titulku okna */
#ifdef USE_STUDENT_RENDERER
    SDL_WM_SetCaption(PROGRAM_TITLE_STUDENT, 0);
#else
    SDL_WM_SetCaption(PROGRAM_TITLE, 0);
#endif // USE_STUDENT_RENDERER

    /* screen */
    screen = SDL_SetVideoMode(DEFAULT_WIDTH, DEFAULT_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);

    /* enable Unicode translation */
    SDL_EnableUNICODE(1);

    /* dalsi inicializace */
    onInit(renderer, polymodel);

    /* nastavime timer, ktery bude volat zadanou callback fci */
    timer = SDL_AddTimer(TIMER_INTERVAL, timerCallback, NULL);

    /* kreslime dokud nenarazime na SDL_QUIT event */
    while( !quit )
    {
        /* vycteni udalosti */
        while( SDL_PollEvent(&event) )
        {
            switch( event.type )
            {
                /* udalost klavesnice */
                case SDL_KEYDOWN:
                    onKeyboard(&event.key);
                    break;

                /* zmena velikosti okna */
                case SDL_VIDEORESIZE:
                    onResize(&event.resize);
                    break;

                /* udalost mysi - pohyb */
                case SDL_MOUSEMOTION:
                    onMouseMotion(&event.motion);
                    break;

                /* udalost mysi - tlacitka */
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    onMouseButton(&event.button);
                    break;

                /* tiknuti timeru */
                case SDL_USEREVENT:
                    if( event.user.code == IZG_TIMER_EVENT )
                    {
                        onTimer((int)SDL_GetTicks());
                    }
                    break;

                /* SDL_QUIT event */
                case SDL_QUIT:
                    quit = 1;
                    break;

                default:
                    break;
            }
        }

        /* vykresleni pres SDL knihovnu */
        draw();
    }

    /* ukonceni timeru */
    SDL_RemoveTimer(timer);

    /* ukonceni SDL */
    SDL_FreeSurface(screen);

    /* shutdown all SDL subsystems */
    SDL_Quit();

    /* zrusime co jsme vytvorili a ukoncime program */
    modRelease(&polymodel);
    renderer->releaseFunc(&renderer);

    return 0;
}


/*****************************************************************************/
/*****************************************************************************/
