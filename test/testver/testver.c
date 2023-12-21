
/* Test program to compare the compile-time version of SDL with the linked
   version of SDL
*/

#include <stdio.h>

#include "SDL.h"
#include "SDL_byteorder.h"

#include <sega_sys.h>

#include <sgl.h>

#include <string.h>
#include <stdlib.h>


static	volatile Uint16	PadData1  = 0x0000;

static const unsigned short buffer_size = 256;

int main()
{
	SDL_version compiled;
	unsigned char line = 1;

	char text_buffer[buffer_size];
	memset(text_buffer, 0, buffer_size);

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
		sprintf(text_buffer, "Couldn't initialize SDL: %s",SDL_GetError());
		slPrint(text_buffer, slLocate (1,line++));
		SYS_Exit(1);
	}

	SDL_SetVideoMode(320, 240, 8, SDL_HWSURFACE);
	SDL_ShowCursor(0);
	slPrint("SDL initialized", slLocate (1,line++));

#if SDL_VERSION_ATLEAST(1, 2, 0)
	slPrint("Compiled with SDL 1.2 or newer", slLocate (1,line++));
#else
	slPrint("Compiled with SDL older than 1.2",  slLocate (1,line++));
#endif
	SDL_VERSION(&compiled);

	memset(text_buffer, 0, buffer_size);
	sprintf(text_buffer, "Compiled version: %d.%d.%d",
			compiled.major, compiled.minor, compiled.patch);
	slPrint(text_buffer, slLocate (1,line++));

	memset(text_buffer, 0, buffer_size);
	sprintf(text_buffer, "Linked version: %d.%d.%d",
			SDL_Linked_Version()->major,
			SDL_Linked_Version()->minor,
			SDL_Linked_Version()->patch);
	slPrint(text_buffer, slLocate (1,line++));

	memset(text_buffer, 0, buffer_size);
	sprintf(text_buffer, "This is a %s endian machine.",
		(SDL_BYTEORDER == SDL_LIL_ENDIAN) ? "little" : "big");
	slPrint(text_buffer, slLocate (1,line++));

	for(;;) {
		slSynch();
	}

	SDL_Quit();

	return(0);
}
