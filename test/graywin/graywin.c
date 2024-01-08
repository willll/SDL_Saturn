
/* Simple program:  Fill a colormap with gray and stripe it down the screen */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "SDL.h"

#ifdef TEST_VGA16 /* Define this if you want to test VGA 16-color video modes */
#define NUM_COLORS	16
#else
#define NUM_COLORS	256
#endif

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	224
#define COLOR_DEPTH		8

/* Draw a randomly sized and colored box centered about (X,Y) */
void DrawBox(SDL_Surface *screen, int X, int Y)
{
	static unsigned int seeded = 0;
	SDL_Rect area;
	Uint32 color;

	/* Seed the random number generator */
	if ( seeded == 0 ) {
		srand(time(NULL));
		seeded = 1;
	}

	/* Get the bounds of the rectangle */
	area.w = (rand()%SCREEN_WIDTH);
	area.h = (rand()%SCREEN_HEIGHT);
	area.x = X-(area.w/2);
	area.y = Y-(area.h/2);
	color = (rand()%NUM_COLORS);

	/* Do it! */
	SDL_FillRect(screen, &area, color);
	SDL_UpdateRects(screen, 1, &area);
}

SDL_Surface *CreateScreen(Uint16 w, Uint16 h, Uint8 bpp, Uint32 flags)
{
	SDL_Surface *screen;
	int i;
	SDL_Color palette[NUM_COLORS];
	Uint8 *buffer;

	/* Set the video mode */
	screen = SDL_SetVideoMode(w, h, bpp, flags);
	if ( screen == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
							"Couldn't set display mode: %s\n",
							SDL_GetError());
		return(NULL);
	}
	//!!fprintf(stderr, "Screen is in %s mode\n",
	//!!	(screen->flags & SDL_FULLSCREEN) ? "fullscreen" : "windowed");

	/* Set a gray colormap, reverse order from white to black */
	for ( i=0; i<NUM_COLORS; ++i ) {
		palette[i].r = (NUM_COLORS-1)-i * (256 / NUM_COLORS);
		palette[i].g = (NUM_COLORS-1)-i * (256 / NUM_COLORS);
		palette[i].b = (NUM_COLORS-1)-i * (256 / NUM_COLORS);
	}
	SDL_SetColors(screen, palette, 0, NUM_COLORS);

	/* Set the surface pixels and refresh! */
	if ( SDL_LockSurface(screen) < 0 ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
										"Couldn't lock display surface: %s\n",
										SDL_GetError());
		return(NULL);
	}
	buffer = (Uint8 *)screen->pixels;
	for ( i=0; i<screen->h; ++i ) {
		memset(buffer,(i*(NUM_COLORS-1))/screen->h, screen->w);
		buffer += screen->pitch;
	}
	SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	return(screen);
}

int main(int argc, char *argv[])
{
	SDL_Surface *screen = NULL;
	Uint32 videoflags;
	int    done = 0;
	SDL_Event event;
	SDL_Joystick *joystick = NULL;
	int joystick_id = 0;
	int width, height, bpp;

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0 ) {
		char text_buffer[256];
		memset(text_buffer, 0, 256);
		sprintf(text_buffer, "Couldn't initialize SDL: %s\n", SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		exit(1);
	}

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE);

	/* See if we try to get a hardware colormap */
	width = SCREEN_WIDTH;
	height = SCREEN_HEIGHT;
	bpp = COLOR_DEPTH;
	videoflags = (SDL_HWSURFACE | SDL_FULLSCREEN);

	/* Set a video mode */
	screen = CreateScreen(width, height, bpp, videoflags);
	if ( screen == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't CreateScreen\n");
		exit(2);
	}

	joystick = SDL_JoystickOpen(joystick_id);

	/* Wait for a keystroke */
	while ( !done && SDL_WaitEvent(&event) ) {
		switch (event.type) {
			case SDL_JOYAXISMOTION:
				SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,"Joystick %d axis %d value: %d\n",
							 event.jaxis.which,
							 event.jaxis.axis,
							 event.jaxis.value);
				 DrawBox(screen, SDL_JoystickGetAxis(joystick, 0), SDL_JoystickGetAxis(joystick, 1));
				break;
			case SDL_MOUSEBUTTONDOWN:
				DrawBox(screen, event.button.x, event.button.y);
				break;
			case SDL_JOYBUTTONDOWN:
 				 SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Joystick %d button %d down\n",
 								event.jbutton.which,
 								event.jbutton.button);
				 DrawBox(screen, SDL_JoystickGetAxis(joystick, 0), SDL_JoystickGetAxis(joystick, 1));
 				 break;
 			 case SDL_JOYBUTTONUP:
 				 SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Joystick %d button %d up\n",
 								event.jbutton.which,
 								event.jbutton.button);
				 break;
			case SDL_KEYDOWN:
				/* Ignore ALT-TAB for windows */
				if ( (event.key.keysym.sym == SDLK_LALT) ||
				     (event.key.keysym.sym == SDLK_TAB) ) {
					break;
				}
				/* Center the mouse on <SPACE> */
				if ( event.key.keysym.sym == SDLK_SPACE ) {
					SDL_WarpMouse(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
					break;
				}
				/* Toggle fullscreen mode on <RETURN> */
				if ( event.key.keysym.sym == SDLK_RETURN ) {
					videoflags ^= SDL_FULLSCREEN;
					screen = CreateScreen( screen->w, screen->h,
																screen->format->BitsPerPixel,
																videoflags);
					if ( screen == NULL ) {
						SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't toggle fullscreen mode\n");
						done = 1;
					}
					break;
				}
				/* Any other key quits the application... */
			case SDL_QUIT:
				done = 1;
				break;
			default:
				break;
		}
	}
	SDL_Quit();
	return(0);
}
