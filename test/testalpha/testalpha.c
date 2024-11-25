
/* Simple program:  Fill a colormap with gray and stripe it down the screen,
		    Then move an alpha valued sprite around the screen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"

#define FRAME_TICKS	(1000/30)		/* 30 frames/second */

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	224
#define COLOR_DEPTH		8

static const unsigned short buffer_size = 256;
static const char* icon_image="icon.bmp";

/* Create a "light" -- a yellowish surface with variable alpha */
SDL_Surface *CreateLight(SDL_Surface *screen, int radius)
{
	Uint8  trans, alphamask;
	int    range, addition;
	int    xdist, ydist;
	Uint16 x, y;
	Uint16 skip;
	Uint32 pixel;
	SDL_Surface *light;
	char text_buffer[buffer_size];
	memset(text_buffer, 0, buffer_size);

#ifdef LIGHT_16BIT
	Uint16 *buf;

	/* Create a 16 (4/4/4/4) bpp square with a full 4-bit alpha channel */
	/* Note: this isn't any faster than a 32 bit alpha surface */
	alphamask = 0x0000000F;
	light = SDL_CreateRGBSurface(SDL_SWSURFACE, 2*radius, 2*radius, 16,
			0x0000F000, 0x00000F00, 0x000000F0, alphamask);
#else
	Uint32 *buf;

	/* Create a 32 (8/8/8/8) bpp square with a full 8-bit alpha channel */
	alphamask = 0x000000FF;
	light = SDL_CreateRGBSurface(SDL_SWSURFACE, 2*radius, 2*radius, 32,
															0xFF000000, 0x00FF0000, 0x0000FF00, alphamask);
	if ( light == NULL ) {
		sprintf(text_buffer, "Couldn't Create light : %s\n",
							SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		return(NULL);
	}
#endif

	/* Fill with a light yellow-orange color */
	skip = light->pitch-(light->w*light->format->BytesPerPixel);
#ifdef LIGHT_16BIT
	buf = (Uint16 *)light->pixels;
#else
	buf = (Uint32 *)light->pixels;
#endif
        /* Get a tranparent pixel value - we'll add alpha later */
	pixel = SDL_MapRGBA(light->format, 0xFF, 0xDD, 0x88, 0);
	for ( y=0; y<light->h; ++y ) {
		for ( x=0; x<light->w; ++x ) {
			*buf++ = pixel;
		}
		buf += skip;	/* Almost always 0, but just in case... */
	}

	/* Calculate alpha values for the surface. */
#ifdef LIGHT_16BIT
	buf = (Uint16 *)light->pixels;
#else
	buf = (Uint32 *)light->pixels;
#endif
	for ( y=0; y<light->h; ++y ) {
		for ( x=0; x<light->w; ++x ) {
			/* Slow distance formula (from center of light) */
			xdist = x-(light->w/2);
			ydist = y-(light->h/2);
			range = (int)sqrt(xdist*xdist+ydist*ydist);

			/* Scale distance to range of transparency (0-255) */
			if ( range > radius ) {
				trans = alphamask;
			} else {
				/* Increasing transparency with distance */
				trans = (Uint8)((range*alphamask)/radius);

				/* Lights are very transparent */
				addition = (alphamask+1)/8;
				if ( (int)trans+addition > alphamask ) {
					trans = alphamask;
				} else {
					trans += addition;
				}
			}
			/* We set the alpha component as the right N bits */
			*buf++ |= (255-trans);
		}
		buf += skip;	/* Almost always 0, but just in case... */
	}
	/* Enable RLE acceleration of this alpha surface */
	SDL_SetAlpha(light, SDL_SRCALPHA|SDL_RLEACCEL, 0);

	/* We're done! */
	return(light);
}

static Uint32 flashes = 0;
static Uint32 flashtime = 0;

void FlashLight(SDL_Surface *screen, SDL_Surface *light, int x, int y)
{
	SDL_Rect position;
	Uint32   ticks1;
	Uint32   ticks2;

	/* Easy, center light */
	position.x = x-(light->w/2);
	position.y = y-(light->h/2);
	position.w = light->w;
	position.h = light->h;
	ticks1 = SDL_GetTicks();
	SDL_BlitSurface(light, NULL, screen, &position);
	ticks2 = SDL_GetTicks();
	SDL_UpdateRects(screen, 1, &position);
	++flashes;

	/* Update time spend doing alpha blitting */
	flashtime += (ticks2-ticks1);
}

static int sprite_visible = 0;
static SDL_Surface *sprite;
static SDL_Surface *backing;
static SDL_Rect    position;
static int         x_vel, y_vel;
static int	   alpha_vel;

int LoadSprite(SDL_Surface *screen, const char *file)
{
	SDL_Surface *converted;

	/* Load the sprite image */
	sprite = SDL_LoadBMP(file);
	if ( sprite == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			 							"Couldn't load %s: %s",
										file, SDL_GetError());
		return(-1);
	} else {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
			 							"Sprite address : %p\n",
										sprite);
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
			 							"Sprite width (%d), height (%d), Pitch (%d), Offset (%d)\n",
										sprite->w,
										sprite->h,
										sprite->pitch,
										sprite->offset);
	}

	/* Set transparent pixel as the pixel at (0,0) */
	if ( sprite->format->palette ) {
		SDL_SetColorKey(sprite, SDL_SRCCOLORKEY,
										*(Uint8 *)sprite->pixels);
	}

	/* Convert sprite to video format */
	converted = SDL_DisplayFormat(sprite);
	SDL_FreeSurface(sprite);
	if ( converted == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			 							"Couldn't convert background: %s\n",
										SDL_GetError());
		return(-1);
	}
	sprite = converted;

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"Sprite converted address : %p\n",
									sprite);
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"Sprite converted width (%d), height (%d), Pitch (%d), Offset (%d)\n",
									sprite->w,
									sprite->h,
									sprite->pitch,
									sprite->offset);
	/* Create the background */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"Create the background\n");
	backing = SDL_CreateRGBSurface(SDL_SWSURFACE, sprite->w, sprite->h, 8,
																	0, 0, 0, 0);
	if ( backing == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
											"Couldn't create background: %s\n",
											SDL_GetError());
		SDL_FreeSurface(sprite);
		return(-1);
	}

	/* Convert background to video format */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"Convert background to video format\n");
	converted = SDL_DisplayFormat(backing);
	SDL_FreeSurface(backing);
	if ( converted == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
										"Couldn't convert background: %s\n",
										SDL_GetError());
		SDL_FreeSurface(sprite);
		return(-1);
	}
	backing = converted;

	/* Set the initial position of the sprite */
	position.x = (screen->w-sprite->w)/2;
	position.y = (screen->h-sprite->h)/2;
	position.w = sprite->w;
	position.h = sprite->h;
	x_vel = 0; y_vel = 0;
	alpha_vel = 1;

	/* We're ready to roll. :) */
	return(0);
}

void AttractSprite(Uint16 x, Uint16 y)
{
	x_vel = ((int)x-position.x)/10;
	y_vel = ((int)y-position.y)/10;
}

void MoveSprite(SDL_Surface *screen, SDL_Surface *light)
{
	SDL_Rect updates[2];
	int alpha;

	/* Erase the sprite if it was visible */
	if ( sprite_visible ) {
		updates[0] = position;
		SDL_BlitSurface(backing, NULL, screen, &updates[0]);
	} else {
		updates[0].x = 0; updates[0].y = 0;
		updates[0].w = 0; updates[0].h = 0;
		sprite_visible = 1;
	}

	/* Since the sprite is off the screen, we can do other drawing
	   without being overwritten by the saved area behind the sprite.
	 */
	if ( light != NULL ) {
		int x, y;

		SDL_GetMouseState(&x, &y);
		FlashLight(screen, light, x, y);
	}

	/* Move the sprite, bounce at the wall */
	position.x += x_vel;
	if ( (position.x < 0) || (position.x >= screen->w) ) {
		x_vel = -x_vel;
		position.x += x_vel;
	}
	position.y += y_vel;
	if ( (position.y < 0) || (position.y >= screen->h) ) {
		y_vel = -y_vel;
		position.y += y_vel;
	}

	/* Update transparency (fade in and out) */
	alpha = sprite->format->alpha;
	if ( (alpha+alpha_vel) < 0 ) {
		alpha_vel = -alpha_vel;
	} else
	if ( (alpha+alpha_vel) > 255 ) {
		alpha_vel = -alpha_vel;
	}
	SDL_SetAlpha(sprite, SDL_SRCALPHA, (Uint8)(alpha+alpha_vel));

	/* Save the area behind the sprite */
	updates[1] = position;
	SDL_BlitSurface(screen, &updates[1], backing, NULL);

	/* Blit the sprite onto the screen */
	updates[1] = position;
	SDL_BlitSurface(sprite, NULL, screen, &updates[1]);

	/* Make it so! */
	SDL_UpdateRects(screen, 2, updates);
}

void WarpSprite(SDL_Surface *screen, int x, int y)
{
	SDL_Rect updates[2];

	/* Erase, move, Draw, update */
	updates[0] = position;
	SDL_BlitSurface(backing, NULL, screen, &updates[0]);
	position.x = x-sprite->w/2;	/* Center about X */
	position.y = y-sprite->h/2;	/* Center about Y */
	updates[1] = position;
	SDL_BlitSurface(screen, &updates[1], backing, NULL);
	updates[1] = position;
	SDL_BlitSurface(sprite, NULL, screen, &updates[1]);
	SDL_UpdateRects(screen, 2, updates);
}

int main(/*int argc, char *argv[]*/)
{
	const SDL_VideoInfo *info = NULL;
	SDL_Surface *screen;
	Uint8  video_bpp;
	Uint32 videoflags;
	Uint8 *buffer;
	int    i, done;
	SDL_Event event;
	SDL_Surface *light;
	int mouse_pressed;
	Uint32 ticks, lastticks;
	char text_buffer[buffer_size];
	memset(text_buffer, 0, buffer_size);

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		sprintf(text_buffer, "Couldn't initialize SDL: %s\n", SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}
	//atexit(SDL_Quit);

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE);

	/* Alpha blending doesn't work well at 8-bit color */
	info = SDL_GetVideoInfo();
	if ( info->vfmt->BitsPerPixel > 8 ) {
		video_bpp = info->vfmt->BitsPerPixel;
	} else {
		video_bpp = COLOR_DEPTH; //8
	}
	videoflags = (SDL_HWSURFACE | SDL_FULLSCREEN);

	/* Set 640x480 video mode */
	if ( (screen = SDL_SetVideoMode(SCREEN_WIDTH,
																SCREEN_HEIGHT,
																video_bpp,
																videoflags)) == NULL ) {
		sprintf(text_buffer, "Couldn't set %d x %d x %d video mode: %s\n",
						SCREEN_WIDTH,SCREEN_HEIGHT,
						video_bpp, SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}

	/* Set the surface pixels and refresh! */
	if ( SDL_LockSurface(screen) < 0 ) {
		sprintf(text_buffer,
						"Couldn't lock the display surface: %s\n",
							SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}

	buffer=(Uint8 *)screen->pixels;
	for ( i=0; i<screen->h; ++i ) {
		memset(buffer,(i*255)/screen->h, screen->pitch);
		buffer += screen->pitch;
	}
	SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	/* Create the light */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Create the light\n");
	light = CreateLight(screen, 82);
	if ( light == NULL ) {
		sprintf(text_buffer, "Couldn't Create light : %s\n",
							SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}

	/* Load the sprite */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Load the sprite\n");
	if ( LoadSprite(screen, icon_image) < 0 ) {
		SDL_FreeSurface(light);
		sprintf(text_buffer, "%s l%d :Couldn't load %s\n",
							__FUNCTION__,
							__LINE__,
							icon_image);
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}

	/* Set a clipping rectangle to clip the outside edge of the screen */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
		 						"Set a clipping rectangle to clip the outside edge of the screen\n");
	{ SDL_Rect clip;
		clip.x = 32;
		clip.y = 32;
		clip.w = screen->w-(2*32);
		clip.h = screen->h-(2*32);
		SDL_SetClipRect(screen, &clip);
	}

	/* Wait for a keystroke */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Wait for a keystroke\n");
	lastticks = SDL_GetTicks();
	done = 0;
	mouse_pressed = 0;
	while ( !done ) {
		/* Update the frame -- move the sprite */
		if ( mouse_pressed ) {
			MoveSprite(screen, light);
			mouse_pressed = 0;
		} else {
			MoveSprite(screen, NULL);
		}

		/* Slow down the loop to 30 frames/second */
		ticks = SDL_GetTicks();
		//SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Slow down the loop to 30 frames/second\n");
		if ( (ticks-lastticks) < FRAME_TICKS ) {
#ifdef CHECK_SLEEP_GRANULARITY
fprintf(stderr, "Sleeping %d ticks\n", FRAME_TICKS-(ticks-lastticks));
#endif
			SDL_Delay(FRAME_TICKS-(ticks-lastticks));
#ifdef CHECK_SLEEP_GRANULARITY
fprintf(stderr, "Slept %d ticks\n", (SDL_GetTicks()-ticks));
#endif
		}
		lastticks = ticks;

		/* Check for events */
		//SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Check for events\n");
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				/* Attract sprite while mouse is held down */
				case SDL_MOUSEMOTION:
					if (event.motion.state != 0) {
						AttractSprite(event.motion.x,
													event.motion.y);
						mouse_pressed = 1;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if ( event.button.button == 1 ) {
						AttractSprite(event.button.x,
						              event.button.y);
						mouse_pressed = 1;
					} else {
						SDL_Rect area;

						area.x = event.button.x-16;
						area.y = event.button.y-16;
						area.w = 32;
						area.h = 32;
						SDL_FillRect(screen, &area, 0);
						SDL_UpdateRects(screen,1,&area);
					}
					break;
				case SDL_KEYDOWN:
					/* Any keypress quits the app... */
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
	}
	SDL_FreeSurface(light);
	SDL_FreeSurface(sprite);
	SDL_FreeSurface(backing);

	/* Print out some timing information */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Print out some timing information\n");
	if ( flashes > 0 ) {
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%d alpha blits, ~%4.4f ms per blit\n",
			flashes, (float)flashtime/flashes);
	}
	return(0);
}
