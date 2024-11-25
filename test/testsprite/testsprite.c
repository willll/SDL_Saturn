
/* Simple program:  Move N sprites around on the screen as fast as possible */

/*
#define        VDP2_VRAM_B0    0x25e40000
#define        VDP2_VRAM_B1    0x25e60000

#undef cgaddress
#undef pal
#undef TEXDEF

#define    cgaddress    0x8000 //SpriteBufSize
#define    cgaddress8    cgaddress/8
#define pal1 COL_256
#define TEXDEF(h,v,presize)        {h,v,(cgaddress+(((presize)*4)>>(pal1)))/8,(((h)&0x1f8)<<5 | (v))}

do {
DMA_ScuMemCopy((uint8*)(SpriteVRAM + cgaddress), (uint8*)rgb, 12*16*4);
    SCU_DMAWait();

    TEXTURE *txptr = (TEXTURE *)&tex_spr[1];
    *txptr = TEXDEF(w, (16>>6), 0);
//SWAP(_txt1Layer, _txt2Layer);
    SPRITE user_sprite;
    user_sprite.CTRL= 0;
    user_sprite.PMOD= CL256Bnk| ECdis | 0x0800;// | ECenb | SPdis;  // pas besoin pour les sprites
    user_sprite.SRCA= (cgaddress) / 8;
    user_sprite.COLR= 0;

    user_sprite.SIZE=(w/8)<<8|h;
    user_sprite.XA=x;
    user_sprite.YA=y;
    user_sprite.GRDA=0;
 #define        toFIXED2(a)        ((FIXED)(65536.0 * (a)))
    slSetSprite(&user_sprite, toFIXED2(10));    // à remettre // ennemis et objets
    slSynch();`
}
while(1);
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "SDL.h"

#define NUM_SPRITES	1 //100
#define MAX_SPEED 	1

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	224
#define COLOR_DEPTH		8

SDL_Surface *sprite;
int numsprites;
SDL_Rect *sprite_rects;
SDL_Rect *positions;
SDL_Rect *velocities;
int sprites_visible;

int LoadSprite(SDL_Surface *screen, char *file)
{
	SDL_Surface *temp = NULL;

	/* Load the sprite image */
	sprite = SDL_LoadBMP(file);
	if ( sprite == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s", file, SDL_GetError());
		return(-1);
	}

	/* Set transparent pixel as the pixel at (0,0) */
	if ( sprite->format->palette ) {
		SDL_SetColorKey(sprite, (SDL_SRCCOLORKEY|SDL_RLEACCEL),
						*(Uint8 *)sprite->pixels);
	}

	/* Convert sprite to video format */
	temp = SDL_DisplayFormat(sprite);
	SDL_FreeSurface(sprite);
	if ( temp == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			 							"Couldn't convert background: %s\n",
										SDL_GetError());
		return(-1);
	}
	sprite = temp;

	/* We're ready to roll. :) */
	return(0);
}

void MoveSprites(SDL_Surface *screen, Uint32 background)
{
	int i, nupdates;
	SDL_Rect area, *position, *velocity;

	nupdates = 0;
	/* Erase all the sprites if necessary */
	if ( sprites_visible ) {
		SDL_FillRect(screen, NULL, background);
	}

	/* Move the sprite, bounce at the wall, and draw */
	for ( i=0; i<numsprites; ++i ) {
		position = &positions[i];
		velocity = &velocities[i];
		position->x += velocity->x;
		if ( (position->x < 0) || (position->x >= screen->w) ) {
			velocity->x = -velocity->x;
			position->x += velocity->x;
		}
		position->y += velocity->y;
		if ( (position->y < 0) || (position->y >= screen->h) ) {
			velocity->y = -velocity->y;
			position->y += velocity->y;
		}

		/* Blit the sprite onto the screen */
		area = *position;
		SDL_BlitSurface(sprite, NULL, screen, &area);
		sprite_rects[nupdates++] = area;
	}

	/* Update the screen! */
	if ( (screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF ) {
		SDL_Flip(screen);
	} else {
		SDL_UpdateRects(screen, nupdates, sprite_rects);
	}
	sprites_visible = 1;
}

/* This is a way of telling whether or not to use hardware surfaces */
Uint32 FastestFlags(Uint32 flags)
{
	const SDL_VideoInfo *info = NULL;

	/* Hardware acceleration is only used in fullscreen mode */
	flags |= SDL_FULLSCREEN;

	/* Check for various video capabilities */
	info = SDL_GetVideoInfo();
	if ( info->blit_hw_CC && info->blit_fill ) {
		/* We use accelerated colorkeying and color filling */
		flags |= SDL_HWSURFACE;
	}
	/* If we have enough video memory, and will use accelerated
	   blits directly to it, then use page flipping.
	 */
	if ( (flags & SDL_HWSURFACE) == SDL_HWSURFACE ) {
		/* Direct hardware blitting without double-buffering
		   causes really bad flickering.
		 */
		SDL_Surface *screen = SDL_GetVideoSurface();
		if ( info->video_mem > (screen->h*screen->pitch) ) {
			flags |= SDL_DOUBLEBUF;
		} else {
			flags &= ~SDL_HWSURFACE;
		}
	}

	/* Return the flags */
	return(flags);
}

int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	Uint8 *mem;
	int width, height;
	Uint8  video_bpp;
	Uint32 videoflags;
	Uint32 background;
	int    i, done;
	SDL_Event event;
	Uint32 then, now, frames;

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	//atexit(SDL_Quit);

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE);

	numsprites = NUM_SPRITES;
	videoflags =  (SDL_HWSURFACE | SDL_FULLSCREEN);
	width = SCREEN_WIDTH;
	height = SCREEN_HEIGHT;
	video_bpp = COLOR_DEPTH;

	/* Set video mode */
	screen = SDL_SetVideoMode(width, height, video_bpp, videoflags);
	if ( ! screen ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
										"Couldn't set %dx%d video mode: %s\n",
										width, height, SDL_GetError());
		exit(2);
	}

	/* Load the sprite */
	if ( LoadSprite(screen, "icon.bmp") < 0 ) {
		exit(1);
	}

	/* Allocate memory for the sprite info */
	mem = (Uint8 *)malloc(4*sizeof(SDL_Rect)*numsprites);
	if ( mem == NULL ) {
		SDL_FreeSurface(sprite);
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Out of memory!\n");
		exit(2);
	} else {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"Sprite Memory allocated\n");
	}

	sprite_rects = (SDL_Rect *)mem;
	positions = sprite_rects;
	sprite_rects += numsprites;
	velocities = sprite_rects;
	sprite_rects += numsprites;
	srand(time(NULL));

	for ( i=0; i<numsprites; ++i ) {
		positions[i].x = rand()%screen->w;
		positions[i].y = rand()%screen->h;
		positions[i].w = sprite->w;
		positions[i].h = sprite->h;
		velocities[i].x = 0;
		velocities[i].y = 0;
		while ( ! velocities[i].x && ! velocities[i].y ) {
			velocities[i].x = (rand()%(MAX_SPEED*2+1))-MAX_SPEED;
			velocities[i].y = (rand()%(MAX_SPEED*2+1))-MAX_SPEED;
		}
	}
	background = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

	/* Print out information about our surfaces */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
								"Screen is at %d bits per pixel\n",
								screen->format->BitsPerPixel);

	if ( (screen->flags & SDL_HWSURFACE) == SDL_HWSURFACE ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Screen is in video memory\n");
	} else {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Screen is in system memory\n");
	}

	if ( (screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Screen has double-buffering enabled\n");
	}

	if ( (sprite->flags & SDL_HWSURFACE) == SDL_HWSURFACE ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Sprite is in video memory\n");
	} else {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Sprite is in system memory\n");
	}

	/* Run a sample blit to trigger blit acceleration */
	{ SDL_Rect dst;
		dst.x = 0;
		dst.y = 0;
		dst.w = sprite->w;
		dst.h = sprite->h;
		SDL_BlitSurface(sprite, NULL, screen, &dst);
		SDL_FillRect(screen, &dst, background);
	}

	if ( (sprite->flags & SDL_HWACCEL) == SDL_HWACCEL ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Sprite blit uses hardware acceleration\n");
	}

	if ( (sprite->flags & SDL_RLEACCEL) == SDL_RLEACCEL ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Sprite blit uses RLE acceleration\n");
	}

	/* Loop, blitting sprites and waiting for a keystroke */
	frames = 0;
	then = SDL_GetTicks();
	done = 0;
	sprites_visible = 0;
	// while ( !done ) {
	// 	/* Check for events */
	// 	++frames;
	// 	while ( SDL_PollEvent(&event) ) {
	// 		switch (event.type) {
	// 			case SDL_KEYDOWN:
	// 				/* Any keypress quits the app... */
	// 			case SDL_QUIT:
	// 				done = 1;
	// 				break;
	// 			default:
	// 				break;
	// 		}
	// 	}
	// 	MoveSprites(screen, background);
	// }
	SDL_FreeSurface(sprite);
	free(mem);

	/* Print out some timing information */
	now = SDL_GetTicks();
	if ( now > then ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%2.2f frames per second\n",
					((double)frames*1000)/(now-then));
	}
	return(0);
}
