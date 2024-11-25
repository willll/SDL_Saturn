/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

/* Initialization code for SDL */

#include <stdlib.h>		/* For getenv() */

#include "SDL.h"
#include "SDL_endian.h"
#include "SDL_fatal.h"
#include "SDL_log.h"
#include "SDL_log_c.h"
#ifndef DISABLE_VIDEO
#include "SDL_leaks.h"
#endif

#ifdef ENABLE_SATURN
#include "SDL_saturn.h"
#endif

/* Initialization/Cleanup routines */
#ifndef DISABLE_JOYSTICK
extern int  SDL_JoystickInit(void);
extern void SDL_JoystickQuit(void);
#endif
#ifndef DISABLE_CDROM
extern int  SDL_CDROMInit(void);
extern void SDL_CDROMQuit(void);
#endif
#ifndef DISABLE_TIMERS
extern void SDL_StartTicks(void);
extern int  SDL_TimerInit(void);
extern void SDL_TimerQuit(void);
#endif


/* The current SDL version */
static SDL_version version =
	{ SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL };

/* The initialized subsystems */
static Uint32 SDL_initialized = 0;
static Uint32 ticks_started = 0;

#ifdef CHECK_LEAKS
int surfaces_allocated = 0;
#endif

int SDL_InitSubSystem(Uint32 flags)
{
#ifndef DISABLE_VIDEO
	/* Initialize the video/event subsystem */
	if ( (flags & SDL_INIT_VIDEO) && !(SDL_initialized & SDL_INIT_VIDEO) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_INIT_VIDEO\n");
		if ( SDL_VideoInit(getenv("SDL_VIDEODRIVER"),
		                   (flags&SDL_INIT_EVENTTHREAD)) < 0 ) {
			return(-1);
		}
		SDL_initialized |= SDL_INIT_VIDEO;
	}
#else
	if ( flags & SDL_INIT_VIDEO ) {
		SDL_SetError("SDL not built with video support");
		return(-1);
	}
#endif

#ifndef DISABLE_AUDIO
	/* Initialize the audio subsystem */
	if ( (flags & SDL_INIT_AUDIO) && !(SDL_initialized & SDL_INIT_AUDIO) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_INIT_AUDIO\n");
		if ( SDL_AudioInit(getenv("SDL_AUDIODRIVER")) < 0 ) {
			return(-1);
		}
		SDL_initialized |= SDL_INIT_AUDIO;
	}
#else
	if ( flags & SDL_INIT_AUDIO ) {
		SDL_SetError("SDL not built with audio support");
		//return(-1);
	}
#endif

#ifndef DISABLE_TIMERS
	/* Initialize the timer subsystem */
	if ( ! ticks_started ) {
		SDL_StartTicks();
		ticks_started = 1;
	}

	if ( (flags & SDL_INIT_TIMER) && !(SDL_initialized & SDL_INIT_TIMER) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_INIT_TIMER\n");
		if ( SDL_TimerInit() < 0 ) {
			return(-1);
		}
		SDL_initialized |= SDL_INIT_TIMER;
	}

#else
	if ( flags & SDL_INIT_TIMER ) {
		SDL_SetError("SDL not built with timer support");
		return(-1);
	}
#endif

#ifndef DISABLE_JOYSTICK
	/* Initialize the joystick subsystem */
	if ( (flags & SDL_INIT_JOYSTICK) &&
	     !(SDL_initialized & SDL_INIT_JOYSTICK) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_INIT_JOYSTICK\n");
		if ( SDL_JoystickInit() < 0 ) {
			return(-1);
		}
		SDL_initialized |= SDL_INIT_JOYSTICK;
	}
#else
	if ( flags & SDL_INIT_JOYSTICK ) {
		SDL_SetError("SDL not built with joystick support");
		//return(-1);
	}
#endif

#ifndef DISABLE_CDROM
	/* Initialize the CD-ROM subsystem */
	if ( (flags & SDL_INIT_CDROM) && !(SDL_initialized & SDL_INIT_CDROM) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_INIT_CDROM\n");
		if ( SDL_CDROMInit() < 0 ) {
			return(-1);
		}
		SDL_initialized |= SDL_INIT_CDROM;
	}
#else
	if ( flags & SDL_INIT_CDROM ) {
		SDL_SetError("SDL not built with cdrom support");
		//return(-1);
	}
#endif

	return(0);
}

int SDL_Init(Uint32 flags)
{

#if !defined(DISABLE_THREADS) && defined(ENABLE_PTH)
	if (!pth_init()) {
		return -1;
	}
#endif

#ifdef ENABLE_SATURN
  SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "ENABLE_SATURN\n");
  _init_saturn();
#endif

	/* Clear the error message */
	SDL_ClearError();

  /* Initialize logs */
  SDL_LogInit();

  SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_InitSubSystem\n");
	/* Initialize the desired subsystems */
	if ( SDL_InitSubSystem(flags) < 0 ) {
		return(-1);
	}

	/* Everything is initialized */
	if ( !(flags & SDL_INIT_NOPARACHUTE) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_InstallParachute\n");
		SDL_InstallParachute();
	}

	return(0);
}

void SDL_QuitSubSystem(Uint32 flags)
{
	/* Shut down requested initialized subsystems */
#ifndef DISABLE_CDROM
	if ( (flags & SDL_initialized & SDL_INIT_CDROM) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_CDROMQuit\n");
		SDL_CDROMQuit();
		SDL_initialized &= ~SDL_INIT_CDROM;
	}
#endif
#ifndef DISABLE_JOYSTICK
	if ( (flags & SDL_initialized & SDL_INIT_JOYSTICK) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_JoystickQuit\n");
		SDL_JoystickQuit();
		SDL_initialized &= ~SDL_INIT_JOYSTICK;
	}
#endif
#ifndef DISABLE_TIMERS
	if ( (flags & SDL_initialized & SDL_INIT_TIMER) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_TimerQuit\n");
		SDL_TimerQuit();
		SDL_initialized &= ~SDL_INIT_TIMER;
	}
#endif
#ifndef DISABLE_AUDIO
	if ( (flags & SDL_initialized & SDL_INIT_AUDIO) ) {
    SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_AudioQuit\n");
		SDL_AudioQuit();
		SDL_initialized &= ~SDL_INIT_AUDIO;
	}
#endif
#ifndef DISABLE_VIDEO
  SDL_LogVerbose(SDL_LOG_CATEGORY_SYSTEM, "SDL_VideoQuit\n");
	if ( (flags & SDL_initialized & SDL_INIT_VIDEO) ) {
		SDL_VideoQuit();
		SDL_initialized &= ~SDL_INIT_VIDEO;
	}
#endif
}

Uint32 SDL_WasInit(Uint32 flags)
{
	if ( ! flags ) {
		flags = SDL_INIT_EVERYTHING;
	}
	return (SDL_initialized&flags);
}

void SDL_Quit(void)
{
	/* Quit all subsystems */
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);

#ifdef CHECK_LEAKS
	/* Print the number of surfaces not freed */
	if ( surfaces_allocated != 0 ) {
		fprintf(stderr, "SDL Warning: %d SDL surfaces extant\n",
							surfaces_allocated);
	}
#endif

	/* Uninstall any parachute signal handlers */
	SDL_UninstallParachute();

#if !defined(DISABLE_THREADS) && defined(ENABLE_PTH)
	pth_kill();
#endif
}

/* Return the library version number */
const SDL_version * SDL_Linked_Version(void)
{
	return(&version);
}
