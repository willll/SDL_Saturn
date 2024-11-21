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
#include <string.h>     /* For the definition of memset */

#include "SDL_saturn.h"
#include "SDL_error.h"
#include "SDL_log.h"
#include "SDL_joystick.h"
#include "../SDL_sysjoystick.h"
#include "../SDL_joystick_c.h"

#include <sgl.h>
#include <sega_per.h>

#define MAX_PORT	     16	/* 2 physical ports */
#define MAX_JOYSTICKS	 2	/* only 2 are supported in the multimedia API */
#define MAX_AXES	     2	/* each joystick can have up to 6 axes */
#define MAX_BUTTONS	     9	/* and 8 buttons                      */
#define	MAX_HATS	     0

#define SDL_MAX_AXIS    32767
#define SDL_MIN_AXIS    -32768
#define SDL_AXIS_STEP   128

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
  //PER_DGT_KU,
  //PER_DGT_KD,
  //PER_DGT_KR,
  //PER_DGT_KL,
  PER_DGT_TA,
  PER_DGT_TB,
  PER_DGT_TC,
  PER_DGT_ST,
  PER_DGT_TX,
  PER_DGT_TY,
  PER_DGT_TZ,
  PER_DGT_TR,
  PER_DGT_TL,
};

/* array to hold joystick ID values */
struct joystick_index
{
  int index;
  int id;
};

static struct joystick_index	SYS_Joystick_addr[MAX_JOYSTICKS];

/* The private structure used to keep track of a joystick */
struct joystick_hwdata
{
  int id;
  Uint16	prev_data;
	Uint16	prev_push;
};

static int SDL_SYS_JoystickLoopUp(void);
static int SDL_SYS_JoystickCount(void);


/* Function ...
 */
static int SDL_SYS_JoystickLoopUp(void)
{
  int nReturn = 0;

  PerDigital	*pptr  = Smpc_Peripheral;

  // reset all the inputs devices
  for ( Uint16 i = 0; i < MAX_JOYSTICKS; i++ ) {
   SYS_Joystick_addr[i].id = -1;
  }

  if (!pptr) {
    SDL_SetError("No Devices found\n");
    SDL_LogCritical(SDL_LOG_CATEGORY_INPUT, "No Devices found\n");
    nReturn = -1;
  } else {
    for( Uint16 m = 0; m < MAX_PORT; m++ ) {
        pptr = &(Smpc_Peripheral[m]);
        if (pptr && pptr->id != PER_ID_NotConnect) {
            SYS_Joystick_addr[nReturn].index = m;
            SYS_Joystick_addr[nReturn].id = pptr->id;
            SDL_LogVerbose(SDL_LOG_CATEGORY_INPUT, "Device[%d] ID(%d) : found\n", m, pptr->id);
            SDL_SetError("Device found\n");
            ++nReturn;
        } else {
            char text_buffer[256];
            memset(text_buffer, 0, 256);

            if(pptr) {
              sprintf(text_buffer, "Device[%d] ID(%d): not found\n", m, pptr->id);
            } else {
              sprintf(text_buffer, "Device[%d]: not found\n", m);
            }

            SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, text_buffer);
            SDL_SetError(text_buffer);
        }
      }
  }

  //SYS_Joystick_addr[nReturn].index = 0;
  //SYS_Joystick_addr[nReturn].id = Smpc_Peripheral[0].id;
  //nReturn = 1;

  SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "__FUNCTION__ : %d Device found\n", nReturn);

  return nReturn;
}

/* Function ...
 */
static int SDL_SYS_JoystickCount(void) {
  int nReturn = 0;
  for ( Uint16 i = 0; i < MAX_JOYSTICKS; i++ ) {
   nReturn = nReturn + (SYS_Joystick_addr[i].id != -1);
  }
  return nReturn;
}

/* Function to scan the system for joysticks.
 * This function should set SDL_numjoysticks to the number of available
 * joysticks.  Joystick 0 should be the system default joystick.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
int SDL_SYS_JoystickInit(void)
{
    int nReturn = MAX_JOYSTICKS;

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
  int nReturn = -1;

  if (SYS_Joystick_addr[joystick->index].id == -1) {
    SDL_SYS_JoystickLoopUp();
  }

  if (SYS_Joystick_addr[joystick->index].id != -1) {
  	/* allocate memory for system specific hardware data */
  	joystick->hwdata = (struct joystick_hwdata *) SDL_malloc(sizeof(*joystick->hwdata));

  	if (joystick->hwdata == NULL) {
  		SDL_OutOfMemory();
  	} else {
    	memset(joystick->hwdata, 0, sizeof(*joystick->hwdata));

    	/* fill nbuttons, naxes, and nhats fields */
    	joystick->nbuttons = MAX_BUTTONS;
    	joystick->naxes = MAX_AXES;
    	joystick->nhats = MAX_HATS;

        nReturn = 0;
    }
  }

	return(nReturn);
}


/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */

void SDL_SYS_JoystickUpdate(SDL_Joystick *joystick)
{
  if (SYS_Joystick_addr[joystick->index].id == -1) {
    SDL_SYS_JoystickLoopUp();
  }

  if (SYS_Joystick_addr[joystick->index].id != -1) {

    const int index = SYS_Joystick_addr[joystick->index].index;
    PerDigital	*pptr  = &(Smpc_Peripheral[index]);
    Sint16	padd = ~pptr->data;
    Sint16	padp = ~pptr->push;


    Sint16 prev_data =joystick->hwdata->prev_data;
    joystick->hwdata->prev_data = padd;
    Sint16 prev_push =joystick->hwdata->prev_push;
    joystick->hwdata->prev_push = padp;

    Sint16 data_changed = padd^prev_data;
    Sint16 push_changed = padp^prev_push;

    //if (data_changed) {
    //    if ((data_changed)&(PER_DGT_KU|PER_DGT_KD|PER_DGT_KL|PER_DGT_KR)) {
          Sint16 axis_0 = SDL_JoystickGetAxis(joystick, 0);
          Sint16 axis_1 = SDL_JoystickGetAxis(joystick, 1);;

          if (padd&PER_DGT_KD) {
            if (axis_1 > SDL_MAX_AXIS - SDL_AXIS_STEP)
              axis_1 = SDL_MAX_AXIS;
            else
              axis_1 += SDL_AXIS_STEP;
          } else {
              if (padd&PER_DGT_KU) {
                if (axis_1 < SDL_MIN_AXIS + SDL_AXIS_STEP)
                  axis_1 = SDL_MIN_AXIS;
                else
                  axis_1 -= SDL_AXIS_STEP;
              }
          }
          if (padd&PER_DGT_KR) {
            if (axis_0 > SDL_MAX_AXIS - SDL_AXIS_STEP)
                axis_0 = SDL_MAX_AXIS;
              else
                axis_0 += SDL_AXIS_STEP;
          } else {
              if (padd&PER_DGT_KL) {
                if (axis_0 < SDL_MIN_AXIS + SDL_AXIS_STEP)
                  axis_0 = SDL_MIN_AXIS;
                else
                  axis_0 -= SDL_AXIS_STEP;
              }
          }

          if (SDL_JoystickGetAxis(joystick, 0) != axis_0) {
            SDL_PrivateJoystickAxis(joystick, 0, axis_0);
          }
          if (SDL_JoystickGetAxis(joystick, 1) != axis_1) {
            SDL_PrivateJoystickAxis(joystick, 1, axis_1);
          }
        //}

      	for(int i = 0; i < sizeof(sdl_buttons)/sizeof(sdl_buttons[0]); i++) {
      		if (data_changed & sdl_buttons[i]) {
      			SDL_PrivateJoystickButton(joystick, i, (padd & sdl_buttons[i])?SDL_PRESSED:SDL_RELEASED);
      		}
        }
    //}
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
		SDL_free(joystick->hwdata);
	}
}

/* Function to perform any system-specific joystick related cleanup */
void SDL_SYS_JoystickQuit(void)
{
	return;
}
