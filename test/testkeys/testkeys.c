
/* Print out all the keysyms we have, just to verify them */

#include <stdio.h>
#include <ctype.h>

#include "SDL.h"

#include <sgl.h>

static	volatile Uint16	PadData1  = 0x0000;

static const unsigned short buffer_size = 256;

int main()
{
	SDLKey key;
	unsigned char line = 1;
	char text_buffer[buffer_size];
	memset(text_buffer, 0, buffer_size);

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		sprintf(text_buffer, "Couldn't initialize SDL: %s",SDL_GetError());
		slPrint(text_buffer, slLocate (1,line++));
		SYS_Exit(1);
	}
	for ( key=SDLK_FIRST; key<SDLK_LAST; ++key ) {
		sprintf(text_buffer, "Key #%d, \"%s\"\n", key, SDL_GetKeyName(key));
		slPrint(text_buffer, slLocate (1,line++));
	}

	for(;;) {
		slSynch();
	}
	
	SDL_Quit();
	return(0);
}
