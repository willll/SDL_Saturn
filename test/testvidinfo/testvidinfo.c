
/* Simple program -- figure out what kind of video display we have */

#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

int main(int argc, char *argv[])
{
	const SDL_VideoInfo *info = NULL;
	int i;
	SDL_Rect **modes = NULL;

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE);

	info = SDL_GetVideoInfo();
	if (info) {
		if (info->vfmt) {
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"Current display: %d bits-per-pixel\n", info->vfmt->BitsPerPixel);

			if ( info->vfmt->palette == NULL ) {
				SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
											"	Red Mask = 0x%.8x\n", info->vfmt->Rmask);
				SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
											"	Green Mask = 0x%.8x\n", info->vfmt->Gmask);
				SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
											"	Blue Mask = 0x%.8x\n", info->vfmt->Bmask);
			}
		} else {
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				 							"Current display: not set properly\n");
		}
	} else {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
										"SDL_GetVideoInfo returned NULL ... not good\n");
	}

	/* Print available fullscreen video modes */
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN);

	if ( modes == (SDL_Rect **)0 ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"No available fullscreen video modes\n");
	} else
		if ( modes == (SDL_Rect **)-1 ) {
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"No special fullscreen video modes\n");
		} else {
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"Fullscreen video modes:\n");
			for ( i=0; modes[i]; ++i ) {
				SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
											"\t%dx%d\n", modes[i]->w, modes[i]->h);
			}
	}

	if ( info->wm_available ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"A window manager is available\n");
	} else {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"A window manager is NOT available\n");
	}

	if ( info->hw_available ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"Hardware surfaces are available (%dK video memory)\n",
											info->video_mem);
	}

	if ( info->blit_hw ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"Copy blits between hardware surfaces are accelerated\n");
	}

	if ( info->blit_hw_CC ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"Colorkey blits between hardware surfaces are accelerated\n");
	}

	if ( info->blit_hw_A ) {
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
									"Alpha blits between hardware surfaces are accelerated\n");
	}

	if ( info->blit_sw ) {
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"Copy blits from software surfaces to hardware surfaces are accelerated\n");
	}

	if ( info->blit_sw_CC ) {
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"Colorkey blits from software surfaces to hardware surfaces are accelerated\n");
	}

	if ( info->blit_sw_A ) {
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
										"Alpha blits from software surfaces to hardware surfaces are accelerated\n");
	}

	if ( info->blit_fill ) {
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
											"Color fills on hardware surfaces are accelerated\n");
	}

	for(;;);

	SDL_Quit();
	return(0);
}
