
#include <stdio.h>

#include "SDL_types.h"
#include "SDL.h"

static	volatile Uint16	PadData1  = 0x0000;

int main()
{
	int error = 0;
	int verbose = 1;
	unsigned char line = 1;

	static const unsigned short buffer_size = 256;
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

	if ( sizeof(Uint8) != 1 ) {
		if ( verbose ) {
			sprintf(text_buffer, "sizeof(Uint8) != 1, instead = %d\n",
								sizeof(Uint8));
			slPrint(text_buffer, slLocate (1,line++));
		}
		++error;
	}
	if ( sizeof(Uint16) != 2 ) {
		if ( verbose ) {
			sprintf(text_buffer,"sizeof(Uint16) != 2, instead = %d\n",
								sizeof(Uint16));
			slPrint(text_buffer, slLocate (1,line++));
		}
		++error;
	}
	if ( sizeof(Uint32) != 4 ) {
		if ( verbose ) {
			sprintf(text_buffer,"sizeof(Uint32) != 4, instead = %d\n",
								sizeof(Uint32));
			slPrint(text_buffer, slLocate (1,line++));
		}
		++error;
	}
#ifdef SDL_HAS_64BIT_TYPE
	if ( sizeof(Uint64) != 8 ) {
		if ( verbose ) {
			sprintf(text_buffer,"sizeof(Uint64) != 8, instead = %d\n",
								sizeof(Uint64));
			slPrint(text_buffer, slLocate (1,line++));
		}
		++error;
	}
#else
	if ( verbose ) {
		sprintf(text_buffer,"WARNING: No 64-bit datatype on this platform");
		slPrint(text_buffer, slLocate (1,line++));
	}
#endif
	if ( verbose && ! error ) {
		sprintf(text_buffer, "All data types are the expected size.");
		slPrint(text_buffer, slLocate (1,line++));
	}

	for(;;) {
		slSynch();
	}

	SDL_Quit();

	return( error ? 1 : 0 );
}
