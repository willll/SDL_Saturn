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

    BERO
    bero@geocities.co.jp

    based on win32/SDL_mmjoystick.c

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

/* Win32 MultiMedia Joystick driver, contributed by Andrei de A. Formiga */

#include <stdlib.h>
#include <stdio.h>		/* For the definition of NULL */

#include "SDL_error.h"
#include "SDL_joystick.h"
#include "../SDL_sysjoystick.h"
#include "../SDL_joystick_c.h"

#include	<per_x.h>

#define MAX_JOYSTICKS	2	/* only 2 are supported in the multimedia API */
#define MAX_AXES	2	/* each joystick can have up to 6 axes */
#define MAX_BUTTONS	9	/* and 8 buttons                      */
#define	MAX_HATS	0

#define	MAX_DATA_SIZE	6

static char	*device_name[] = {
	"DIGITAL ","DIGITAL ","DIGITAL ","DIGITAL ",
	"DIGITAL ","DIGITAL ","DIGITAL ","DIGITAL ",
	"DIGITAL ","DIGITAL ","DIGITAL ","DIGITAL ",
	"DIGITAL ","DIGITAL ","DIGITAL ","DIGITAL ",

	"ANALOGUE","ANALOGUE","ANALOGUE","ANALOGUE",
	"ANALOGUE","ANALOGUE","ANALOGUE","ANALOGUE",
	"ANALOGUE","ANALOGUE","ANALOGUE","ANALOGUE",
	"ANALOGUE","ANALOGUE","ANALOGUE","ANALOGUE",

	"POINTING","POINTING","POINTING","POINTING",
	"POINTING","POINTING","POINTING","POINTING",
	"POINTING","POINTING","POINTING","POINTING",
	"POINTING","POINTING","POINTING","POINTING",

	"KEYBOARD","KEYBOARD","KEYBOARD","KEYBOARD",
	"KEYBOARD","KEYBOARD","KEYBOARD","KEYBOARD",
	"KEYBOARD","KEYBOARD","KEYBOARD","KEYBOARD",
	"KEYBOARD","KEYBOARD","KEYBOARD","KEYBOARD",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",

	"MD??????","MD3B    ","MD6B    ","MDMOUSE ",
	"MD??????","MD??????","MD??????","MD??????",
	"MD??????","MD??????","MD??????","MD??????",
	"MD??????","MD??????","MD??????","MD??????",

	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","VR-GUN  ","UNKNOWN ",
	"UNKNOWN ","UNKNOWN ","UNKNOWN ","NONE    ",
};

const static	int sdl_buttons[] = {
	TRG_B,
  TRG_C,
	TRG_A,
	TRG_START,
  TRG_L,
	TRG_Z,
	TRG_Y,
	TRG_X,
  TRG_R
};

static SysPort	*__port = NULL;

/* array to hold joystick ID values */
struct joystick_index
{
  int port;
  int index;
  int id;
};

static struct joystick_index	SYS_Joystick_addr[MAX_JOYSTICKS];

/* The private structure used to keep track of a joystick */
struct joystick_hwdata
{
  int id;
  int size;
 	trigger_t prev_trigger;
};

/* Function to scan the system for joysticks.
 * This function should set SDL_numjoysticks to the number of available
 * joysticks.  Joystick 0 should be the system default joystick.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
int SDL_SYS_JoystickInit(void)
{
  int nReturn = 0;
  __port = PER_OpenPort();
  PER_GetPort( __port );

  if (!__port) {
    nReturn = -1;
  } else {
    for( Uint16 m = 0; m < _MAX_PORT; m++ ){
    			for( Uint16 n = 0; n < MAX_JOYSTICKS; n++ ){
    				const SysDevice	*device = PER_GetDeviceA( &(__port[m]), n );
            if (device) {
                SYS_Joystick_addr[nReturn].port = m;
                SYS_Joystick_addr[nReturn].index = n;
  				      SYS_Joystick_addr[nReturn].id = PER_GetID( device );
                ++nReturn;
            } else {
              char text_buffer[128];
              sprintf(text_buffer, "FAIL : m(%d) n(%d) id(%d)\n", m,n,__port[m].id);
              SDL_SetError(text_buffer);
            }
    			}
  		}
  }

  for ( Uint16 i = nReturn; i < MAX_JOYSTICKS; i++ ) {
    SYS_Joystick_addr[i].id = -1;
  }

	return nReturn;
}

/* Function to get the device-dependent name of a joystick */
const char *SDL_SYS_JoystickName(int index)
{

  if (SYS_Joystick_addr[index].id != -1) {
    return device_name[SYS_Joystick_addr[index].id];
  }

  return NULL;
}

/* Function to open a joystick for use.
   The joystick to open is specified by the index field of the joystick.
   This should fill the nbuttons and naxes fields of the joystick structure.
   It returns 0, or -1 if there is an error.
 */
int SDL_SYS_JoystickOpen(SDL_Joystick *joystick)
{
	/* allocate memory for system specific hardware data */
	joystick->hwdata = (struct joystick_hwdata *) malloc(sizeof(*joystick->hwdata));
	if (joystick->hwdata == NULL)
	{
		SDL_OutOfMemory();
		return(-1);
	}
	memset(joystick->hwdata, 0, sizeof(*joystick->hwdata));

	/* fill nbuttons, naxes, and nhats fields */
	joystick->nbuttons = MAX_BUTTONS;
	joystick->naxes = MAX_AXES;
	joystick->nhats = MAX_HATS;

	return(0);
}


/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */

void SDL_SYS_JoystickUpdate(SDL_Joystick *joystick)
{
  if (SYS_Joystick_addr[joystick->index].id != -1) {
    const int * port = &SYS_Joystick_addr[joystick->index].port;
    const int index = SYS_Joystick_addr[joystick->index].index;
    const SysDevice	*device = PER_GetDeviceA( port, index );

    trigger_t tmp = PER_GetTrigger(device);

    trigger_t prev_trigger =joystick->hwdata->prev_trigger;
    joystick->hwdata->prev_trigger = tmp;
    trigger_t changed = tmp^prev_trigger;

    if (changed) {
        if ((changed)&(TRG_UP|TRG_DOWN|TRG_LEFT|TRG_RIGHT)) {
          int axis_0 = 0;
          int axis_1 = 0;

          if (tmp&TRG_UP)
            axis_0 = 128;
          else if (tmp&TRG_DOWN)
            axis_0 = -128;

          if (tmp&TRG_LEFT)
            axis_1 = 128;
          else if (tmp&TRG_RIGHT)
            axis_1 = -128;

          SDL_PrivateJoystickAxis(joystick, 0, axis_0);
          SDL_PrivateJoystickAxis(joystick, 1, axis_1);
        }

      	for(int i = 0;i < sizeof(sdl_buttons)/sizeof(sdl_buttons[0]); i++) {
      		if (changed & sdl_buttons[i]) {
      			SDL_PrivateJoystickButton(joystick, i, (tmp & sdl_buttons[i])?SDL_PRESSED:SDL_RELEASED);
      		}
        }
    }
  }
}

/* Function to close a joystick after use */
void SDL_SYS_JoystickClose(SDL_Joystick *joystick)
{
  if (SYS_Joystick_addr[joystick->index].id != -1) {
    SYS_Joystick_addr[joystick->index].id = -1;
  }

	if (joystick->hwdata != NULL) {
		/* free system specific hardware data */
		free(joystick->hwdata);
	}
}

/* Function to perform any system-specific joystick related cleanup */
void SDL_SYS_JoystickQuit(void)
{
	return;
}
