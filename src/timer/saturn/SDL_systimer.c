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

    based on win32/SDL_systimer.c

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

#include <limits.h>

//#include "SDL_thread.h"
#include "SDL_timer.h"
#include "SDL_error.h"
#include "../SDL_timer_c.h"

#include <sega_int.h>
#include <sega_tim.h>

static unsigned start;

/* Data to handle a single periodic alarm */
static int timer_alive = 0;
//static SDL_Thread *timer = NULL;
/*
	jif =  ms * HZ /1000
	ms  = jif * 1000/HZ
*/

static Uint32 previouscount=0;
static Uint16 previousmillis=0;

//void Time0Int(void);

//--------------------------------------------------------------------------------------------------------------------------------------
Uint32 SDL_GetTicks(void)
{
  Uint32 tmp = TIM_FRT_CNT_TO_MCR(TIM_FRT_GET_16())+previousmillis;
  Uint32 tmp2 = tmp/ 1000;

  previouscount += tmp2;
  TIM_FRT_SET_16(0);
  previousmillis= (tmp-(tmp2*1000));
  //set_imask(imask);
  //
  return previouscount;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void SDL_Delay(Uint32 ms)
{
    char buffer[256];
    ms*=1000; // milliseconds to microseconds
    Uint32 count = TIM_FRT_MCR_TO_CNT(ms);

    Uint16 trucount = count / USHRT_MAX;
    Uint16 remaining = count - trucount;

    Uint16 tmpcount = 0;
    Uint16 diff = 0;

    while(trucount) {
      TIM_FRT_SET_16(0);
      diff = USHRT_MAX;
      tmpcount = TIM_FRT_GET_16();

      while(USHRT_MAX - tmpcount <= diff) {
        diff = USHRT_MAX - tmpcount;
        tmpcount = TIM_FRT_GET_16();
      }
      --trucount;
    }

    // whatever ...
    TIM_FRT_SET_16(0);
    while((remaining) > TIM_FRT_GET_16());
}

void SDL_StartTicks()
{
  TIM_T0_ENABLE();
}

static int RunTimer(void *unused)
{
	/*while ( timer_alive ) {
		if ( SDL_timer_running ) {
			SDL_ThreadedTimerCheck();
		}
		SDL_Delay(10);
	}*/
	return(0);
}

/* This is only called if the event thread is not running */
int SDL_SYS_TimerInit()
{
	timer_alive = 1;

//  INT_SetScuFunc(INT_SCU_TIM0, Time0Int);

	return 0;
}

void SDL_SYS_TimerQuit()
{
	timer_alive = 0;

}

int SDL_SYS_StartTimer()
{
  //TIM_FRT_INIT(32); // 8, 32 or 128
  TIM_FRT_SET_16(0);

	return 0;
}

void SDL_SYS_StopTimer()
{
	return;
}
