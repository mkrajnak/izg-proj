/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: bmp.c 416 2015-03-10 08:07:46Z spanel $
 *
 * Opravy a modifikace:
 * - 2015/3 (MS): Ukladani BMP pres SDL knihovnu
 * - 2016/2 (MM): Nacitani BMP pres SDL knihovnu
 */

#include "bmp.h"



/****************************************************************************
 * Ulozeni obsahu obrazovky do souboru ve formatu BMP
 */ 

int saveBitmap(const char * filename, S_RGBA * buffer, int width, int height)
{
    SDL_Surface     * image;
    Uint8           * dst;
    S_RGBA          * src;
    int             x, y, bpp, result;

    IZG_ASSERT(filename && buffer && width >= 0 && height >= 0);

    /* vytvoreni SDL surface */
    image = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE);

    /* test typu pixelu */
    bpp = image->format->BytesPerPixel;
    IZG_ASSERT(bpp == 4);
    
    /* kopie bufferu */
    for( y = 0; y < height; ++y )
    {
        dst = (Uint8 *)image->pixels + y * image->pitch;
        src = buffer + y * width;
        for( x = 0; x < width; ++x, dst += bpp, ++src )
        {
            *((Uint32 *)dst) = SDL_MapRGB(image->format, src->red, src->green, src->blue);
        }
    }

    /* ulozeni do BMP obrazku */
    result = SDL_SaveBMP(image, filename);
    SDL_FreeSurface(image);
    return result;
}

/****************************************************************************
* Nacteni textury z BMP
*/

S_RGBA * loadBitmap( const char * filename, int * width, int * height )
{
    SDL_Surface     * image;
    S_RGBA          * dst;
    Uint32          tmp;
    int             i, x, y;

    dst = NULL;

    IZG_ASSERT( filename && width && height );

    /* nacteni obrazku z BMP (vytvoreni SDL surface) */
    image = SDL_LoadBMP( filename );
    IZG_CHECK( image, SDL_GetError() );

    /* alokace pameti pro texturu */
    dst = ( S_RGBA * ) malloc( image->w * image->h * sizeof( S_RGBA ) );
    IZG_CHECK( dst, "Cannot allocate enough memory" );

    /* nastaveni vystupnich parametru (sirka a vyska textury) */
    ( *width ) = image->w;
    ( *height ) = image->h;

    /* naplneni dat textury hodnotami z nacteneho BMP z SDL surface */
    for ( i = 0, x = 0; x < image->w; ++x )
    {
        for ( y = 0; y < image->h; ++y, ++i )
        {
            tmp = getpixel( image, x, y );
            SDL_GetRGB( tmp, image->format, &dst[ i ].red, &dst[ i ].green, &dst[ i ].blue );
            dst[ i ].alpha = 1;
        }
    }

    /* uvolnìní zdrojù */
    SDL_FreeSurface( image );
    return dst;
}

/****************************************************************************
* Ziskani uint32 hodnoty pixelu v SDL surface na souradnicich x, y
*/

Uint32 getpixel( SDL_Surface *surface, int x, int y )
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = ( Uint8 * ) surface->pixels + y * surface->pitch + x * bpp;
    switch ( bpp ) {
        case 1:
            return *p;
            break;
        case 2:
            return *( Uint16 * ) p;
            break;
        case 3:
            if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
                return p[ 0 ] << 16 | p[ 1 ] << 8 | p[ 2 ];
            else
                return p[ 0 ] | p[ 1 ] << 8 | p[ 2 ] << 16;
            break;
        case 4:
            return *( Uint32 * ) p;
            break;
        default:
            return 0;
    }
}

/*****************************************************************************/
/*****************************************************************************/
