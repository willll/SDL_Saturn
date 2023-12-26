
/* Simple program to test the SDL joystick routines */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define COLOR_DEPTH		8

static const unsigned short buffer_size = 256;

void WatchJoystick(SDL_Joystick *joystick, SDL_Surface *screen)
{
	const char *name;
	int i, done;
	SDL_Event event;
	int x, y, draw;
	SDL_Rect axis_area[2];

	/* Print info about the joystick we are watching */
	name = SDL_JoystickName(SDL_JoystickIndex(joystick));
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Watching joystick %d: (%s)\n", SDL_JoystickIndex(joystick),
	       name ? name : "Unknown Joystick");
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Joystick has %d axes, %d hats, %d balls, and %d buttons\n",
	       SDL_JoystickNumAxes(joystick), SDL_JoystickNumHats(joystick),
	       SDL_JoystickNumBalls(joystick),SDL_JoystickNumButtons(joystick));

	/* Initialize drawing rectangles */
	memset(axis_area, 0, (sizeof axis_area));
	draw = 0;

	/* Loop, getting joystick events! */
	done = 0;
	while ( ! done ) {
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
			    case SDL_JOYAXISMOTION:
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Joystick %d axis %d value: %d\n",
						       event.jaxis.which,
						       event.jaxis.axis,
						       event.jaxis.value);
						break;
			    case SDL_JOYHATMOTION:
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Joystick %d hat %d value:\n",
						       event.jhat.which,
						       event.jhat.hat);
						if ( event.jhat.value == SDL_HAT_CENTERED )
							SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION," centered\n");
						if ( event.jhat.value & SDL_HAT_UP )
							SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION," up\n");
						if ( event.jhat.value & SDL_HAT_RIGHT )
							SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION," right\n");
						if ( event.jhat.value & SDL_HAT_DOWN )
							SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION," down\n");
						if ( event.jhat.value & SDL_HAT_LEFT )
							SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION," left\n");
						break;
			    case SDL_JOYBALLMOTION:
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Joystick %d ball %d delta: (%d,%d)\n",
						       event.jball.which,
						       event.jball.ball,
						       event.jball.xrel,
						       event.jball.yrel);
						break;
			    case SDL_JOYBUTTONDOWN:
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Joystick %d button %d down\n",
						       event.jbutton.which,
						       event.jbutton.button);
						break;
			    case SDL_JOYBUTTONUP:
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Joystick %d button %d up\n",
						       event.jbutton.which,
						       event.jbutton.button);
						break;
			    case SDL_KEYDOWN:
						if ( event.key.keysym.sym != SDLK_ESCAPE ) {
							break;
						}
				/* Fall through to signal quit */
			    case SDL_QUIT:
						done = 1;
						break;
			    default:
				break;
			}
		}
		/* Update visual joystick state */
		for ( i=0; i<SDL_JoystickNumButtons(joystick); ++i ) {
			SDL_Rect area;

			area.x = i*34;
			area.y = SCREEN_HEIGHT-34;
			area.w = 32;
			area.h = 32;
			if (SDL_JoystickGetButton(joystick, i) == SDL_PRESSED) {
				SDL_FillRect(screen, &area, 0xFFFF);
			} else {
				SDL_FillRect(screen, &area, 0x0000);
			}
			SDL_UpdateRects(screen, 1, &area);
		}

		/* Erase previous axes */
		SDL_FillRect(screen, &axis_area[draw], 0x0000);

		/* Draw the X/Y axis */
		draw = !draw;
		x = (((int)SDL_JoystickGetAxis(joystick, 0))+32768);
		x *= SCREEN_WIDTH;
		x /= 65535;
		if ( x < 0 ) {
			x = 0;
		} else
		if ( x > (SCREEN_WIDTH-16) ) {
			x = SCREEN_WIDTH-16;
		}
		y = (((int)SDL_JoystickGetAxis(joystick, 1))+32768);
		y *= SCREEN_HEIGHT;
		y /= 65535;
		if ( y < 0 ) {
			y = 0;
		} else
		if ( y > (SCREEN_HEIGHT-16) ) {
			y = SCREEN_HEIGHT-16;
		}
		axis_area[draw].x = (Sint16)x;
		axis_area[draw].y = (Sint16)y;
		axis_area[draw].w = 16;
		axis_area[draw].h = 16;
		SDL_FillRect(screen, &axis_area[draw], 0xFFFF);

		SDL_UpdateRects(screen, 2, axis_area);

		// Wait for vsync
		slSynch();
	}
}

int main(int argc, char *argv[])
{
	const char *name = NULL;
	int i;
	SDL_Joystick *joystick = NULL;
	SDL_Surface *screen =NULL;

	int joystick_id = 0;

	char text_buffer[buffer_size];
	memset(text_buffer, 0, buffer_size);

	/* Initialize SDL (Note: video is required to start event loop) */
	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0 ) {
		sprintf(text_buffer, "Couldn't initialize SDL: %s\n", SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_VERBOSE);

	/* Set a video mode to display joystick axis position */
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DEPTH, SDL_HWSURFACE);
	if ( screen == NULL ) {
		sprintf(text_buffer, "Couldn't set video mode: %s\n",SDL_GetError());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
		SDL_SetError(text_buffer);
		for(;;);
	}

	/* Print information about the joysticks */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "There are %d joysticks attached\n", SDL_NumJoysticks());

	for ( i=0; i<SDL_NumJoysticks(); ++i ) {
		name = SDL_JoystickName(i);
		sprintf(text_buffer, "Joystick %d: %s\n",i, name ? name : "Unknown Joystick\n");
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
	}

	joystick = SDL_JoystickOpen(joystick_id);
	if ( joystick == NULL ) {
			sprintf(text_buffer, "Couldn't open joystick %d: %s\n", joystick_id, SDL_GetError());
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
			SDL_SetError(text_buffer);
 			for(;;);
	} else {
		WatchJoystick(joystick, screen);
		SDL_JoystickClose(joystick);
	}

	SDL_QuitSubSystem(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);

	return(0);
}
