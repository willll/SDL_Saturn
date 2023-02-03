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

#include "SDL_getenv.h"

#include <stdlib.h>
#include <string.h>

static const char *SDL_env[] = { "SDL_VIDEODRIVER=saturn",
                                 "SDL_AUDIODRIVER=saturn",
                                 "SDL_JOYSTICK_DEVICE=saturn",
                                 "SDL_NOMOUSE=1" };

/* Put a variable of the form "name=value" into the environment */
int SDL_putenv(const char *variable)
{
	variable;
	return -1;
}

/* Retrieve a variable named "name" from the environment */
char *SDL_getenv(const char *name)
{
	char *value = NULL;

	if ( SDL_env ) {
		int len = strlen(name);
		for ( int i = 0; SDL_env[i] && !value; ++i ) {
			if ( (strncmp(SDL_env[i], name, len) == 0) &&
			     (SDL_env[i][len] == '=') ) {
				value = &SDL_env[i][len+1];
			}
		}
	}

	return value;
}
