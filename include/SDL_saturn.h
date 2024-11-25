/*
...
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

/* Include file for SDL application focus event handling */

#ifndef _SDL_saturn_h
#define _SDL_saturn_h

#include "SDL_types.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#include <sgl.h>
#include <sl_def.h>

#undef pal

#include <sega_def.h>
#include <sega_sys.h>
#include <sega_gfs.h>
#include <sega_tim.h>
#include <sega_mem.h>
#include <sega_mth.h>

#define	NBG1_MAP_ADR	(VDP2_VRAM_B1 + 0x18000)
#define BACK_COL_ADR    (VDP2_VRAM_A1 + 0x1fffe)
#define NBG1_CEL_ADR    (VDP2_VRAM_B0 + 0x00000)
#define NBG1_COL_ADR    (VDP2_COLRAM  + 0x00200)
#define	TEX_COL_ADR		(VDP2_COLRAM  + 0x00200)
#define	NBG0_COL_ADR    (VDP2_COLRAM  + 0x00000)
#define NBG0_CEL_ADR    (VDP2_VRAM_A0 + 0x00000)

#define	  SZ_HEAP	        0x10000
static    unsigned long	  __heap[SZ_HEAP];

#define malloc(X) MEM_Malloc(X)
#define free(X) MEM_Free(X)
#define realloc(X, Y) MEM_Realloc((X), (Y))
#define calloc(X, Y) MEM_Calloc((X), (Y))

#define srand(X)    MTH_InitialRand((X))
#define rand        MTH_GetRand

#define exit(X)    MTH_InitialRand((X))

#define SDL_malloc(X) MEM_Malloc(X)
#define SDL_free(X) MEM_Free(X)
#define SDL_realloc(X, Y) MEM_Realloc((X), (Y))
#define SDL_calloc(X, Y) MEM_Calloc((X), (Y))

#define SDL_vsnprintf vsnprintf
#define SDL_size_add_overflow(X,Y,Z)  0

void _init_saturn(void);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_saturn_h */
