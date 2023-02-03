
/* Test program to check the resolution of the SDL timer on the current
   platform
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "SDL.h"

#define DEFAULT_RESOLUTION	1
#define buffer_size	256

static int ticks = 0;

static	volatile Uint16	PadData1  = 0x0000;

static char text_buffer[buffer_size];
static unsigned char line = 1;

unsigned int * lowworkram = (unsigned int*)(0x200100);

static Uint32 ticktock(Uint32 interval)
{
	++ticks;
	return(interval);
}

static Uint32 callback(Uint32 interval, void *param)
{
  sprintf(text_buffer, ("Timer %d : param = %d\n", interval, (int)param));
	slPrint(text_buffer, slLocate (1,line++));
  return interval;
}

int main()
{
	int desired;
	SDL_TimerID t1, t2, t3;

	memset(text_buffer, 0, buffer_size);

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
		sprintf(text_buffer, "Couldn't initialize SDL: %s",SDL_GetError());
		slPrint(text_buffer, slLocate (1,line++));
		SYS_Exit(1);
	}

	SDL_SetVideoMode(320, 240, 8, SDL_HWSURFACE);
	SDL_ShowCursor(0);

	atexit(SDL_Quit);

	/* Start the timer */
	desired = 0;

	if ( desired == 0 ) {
		desired = DEFAULT_RESOLUTION;
	}
	SDL_SetTimer(desired, ticktock);

	/* Wait 10 seconds */
	sprintf(text_buffer, "Waiting 10 seconds");
	slPrint(text_buffer, slLocate (1,line++));

	SDL_Delay(10*1000);

	/* Stop the timer */
	SDL_SetTimer(0, NULL);

	/* Print the results */
	if ( ticks ) {
		sprintf(text_buffer, "Timer resolution: desired = %d ms, actual = %f ms",
					desired, (double)(10*1000)/ticks );
		slPrint(text_buffer, slLocate (1,line++));
	}

	/* Test multiple timers */
	sprintf(text_buffer, "Testing multiple timers...");
	slPrint(text_buffer, slLocate (1,line++));

	t1 = SDL_AddTimer(100, callback, (void*)1);
	if(!t1) {
		sprintf(text_buffer, "Could not create timer 1");
		slPrint(text_buffer, slLocate (1,line++));
	}

	t2 = SDL_AddTimer(50, callback, (void*)2);
	if(!t2) {
		sprintf(text_buffer, "Could not create timer 2");
		slPrint(text_buffer, slLocate (1,line++));
	}
	t3 = SDL_AddTimer(233, callback, (void*)3);
	if(!t3) {
		sprintf(text_buffer, "Could not create timer 3");
		slPrint(text_buffer, slLocate (1,line++));
	}

	/* Wait 10 seconds */
	sprintf(text_buffer, "Waiting 10 seconds");
	slPrint(text_buffer, slLocate (1,line++));
	SDL_Delay(10*1000);

	sprintf(text_buffer, "Removing timer 1 and waiting 5 more seconds");
	slPrint(text_buffer, slLocate (1,line++));
	SDL_RemoveTimer(t1);

	SDL_Delay(5*1000);

	SDL_RemoveTimer(t2);
	SDL_RemoveTimer(t3);

	for(;;) {
		slSynch();
	}

	SDL_Quit();

	return(0);
}
