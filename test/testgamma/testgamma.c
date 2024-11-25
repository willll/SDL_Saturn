
/* Bring up a window and manipulate the gamma on it */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "SDL.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	224
#define COLOR_DEPTH		8


/* Turn a normal gamma value into an appropriate gamma ramp */
void CalculateGamma(double gamma, Uint16 *ramp)
{
	int i, value;

	gamma = 1.0 / gamma;
	for ( i=0; i<256; ++i ) {
		value = (int)(pow((double)i/256.0, gamma)*65535.0 + 0.5);
		if ( value > 65535 ) {
			value = 65535;
		}
		ramp[i] = (Uint16)value;
	}
}

/* This can be used as a general routine for all of the test programs */
void get_video_args(int *w, int *h, int *bpp, Uint32 *flags)
{
	*w = SCREEN_WIDTH;
	*h = SCREEN_HEIGHT;
	*bpp = COLOR_DEPTH;
	*flags = (SDL_HWSURFACE | SDL_FULLSCREEN);
}

int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	SDL_Surface *image;
	float gamma;
	int i;
	int w, h, bpp;
	Uint32 flags;
	Uint16 ramp[256];
	Uint16 red_ramp[256];
	Uint32 then, timeout;

	/* Check command line arguments */
	get_video_args(&w, &h, &bpp, &flags);

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,  "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	//atexit(SDL_Quit);

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE);


	/* Initialize the display, always use hardware palette */
	screen = SDL_SetVideoMode(w, h, bpp, flags | SDL_HWPALETTE);
	if ( screen == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set %dx%d video mode: %s\n",
																									w, h, SDL_GetError());
		exit(1);
	}

	/* Set the window manager title bar */
	SDL_WM_SetCaption("SDL gamma test", "testgamma");

	/* Set the desired gamma, if any */
	gamma = 1.0f;
	// if ( *argv ) {
	// 	gamma = (float)atof(*argv);
	// }

	if ( SDL_SetGamma(gamma, gamma, gamma) < 0 ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Unable to set gamma: %s\n", SDL_GetError());
		exit(1);
	}

#if 0 /* This isn't supported.  Integrating the gamma ramps isn't exact */
	/* See what gamma was actually set */
	float real[3];
	if ( SDL_GetGamma(&real[0], &real[1], &real[2]) < 0 ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Couldn't get gamma: %s\n", SDL_GetError());
	} else {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Set gamma values: R=%2.2f, G=%2.2f, B=%2.2f\n",
			real[0], real[1], real[2]);
	}
#endif

	/* Do all the drawing work */
	image = SDL_LoadBMP("sample.bmp");
	if ( image ) {
		SDL_Rect dst;

		dst.x = (screen->w - image->w)/2;
		dst.y = (screen->h - image->h)/2;
		dst.w = image->w;
		dst.h = image->h;
		SDL_BlitSurface(image, NULL, screen, &dst);
		SDL_UpdateRects(screen, 1, &dst);
	}

	/* Wait a bit, handling events */
	then = SDL_GetTicks();
	timeout = (5*1000);
	while ( (SDL_GetTicks()-then) < timeout ) {
		SDL_Event event;

		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
			    case SDL_QUIT:	/* Quit now */
				timeout = 0;
				break;
			    case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				    case SDLK_SPACE:	/* Go longer.. */
					timeout += (5*1000);
					break;
				    case SDLK_UP:
					gamma += 0.2f;
					SDL_SetGamma(gamma, gamma, gamma);
					break;
				    case SDLK_DOWN:
					gamma -= 0.2f;
					SDL_SetGamma(gamma, gamma, gamma);
					break;
				    case SDLK_ESCAPE:
					timeout = 0;
					break;
				    default:
					break;
				}
				break;
			}
		}
	}

	/* Perform a gamma flash to red using color ramps */
	while ( gamma < 10.0 ) {
		/* Increase the red gamma and decrease everything else... */
		gamma += 0.1f;
		CalculateGamma(gamma, red_ramp);
		CalculateGamma(1.0/gamma, ramp);
		SDL_SetGammaRamp(red_ramp, ramp, ramp);
	}
	/* Finish completely red */
	memset(red_ramp, 255, sizeof(red_ramp));
	memset(ramp, 0, sizeof(ramp));
	SDL_SetGammaRamp(red_ramp, ramp, ramp);

	/* Now fade out to black */
	for ( i=(red_ramp[0] >> 8); i >= 0; --i ) {
		memset(red_ramp, i, sizeof(red_ramp));
		SDL_SetGammaRamp(red_ramp, NULL, NULL);
	}
	SDL_Delay(1*1000);

	return(0);
}
