
/* Print out all the keysyms we have, just to verify them */

#include <stdio.h>
#include <ctype.h>

#include "SDL.h"

#include <sgl.h>

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define COLOR_DEPTH		8

static	volatile Uint16	PadData1  = 0x0000;

static const unsigned short buffer_size = 256;

int main()
{
	SDLKey key;
	unsigned char line = 1;
	SDL_Surface *screen =NULL;
	char text_buffer[buffer_size];
	memset(text_buffer, 0, buffer_size);

	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0 ) {
		sprintf(text_buffer, "Couldn't initialize SDL: %s",SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DEPTH, SDL_HWSURFACE);
	if ( screen == NULL ) {
		sprintf(text_buffer, "Couldn't set video mode: %s\n",SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}

	for ( key=SDLK_FIRST; key<SDLK_LAST; ++key ) {
		sprintf(text_buffer, "Key #%d, \"%s\"\n", key, SDL_GetKeyName(key));
		slPrint(text_buffer, slLocate (1,line++));
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_Delay(500);
		slSynch();
	}

	for(;;) {
		slSynch();
	}

	SDL_Quit();
	return(0);
}
