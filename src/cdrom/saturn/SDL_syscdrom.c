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

    based on win32/SDL_syscdrom.c by

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

/* Functions for system-level CD-ROM audio control */

#include <stdlib.h>
#include <stdio.h>

#include "sega_cdc.h"
#include "sega_gfs.h"

#include "SDL_error.h"
#include "SDL_cdrom.h"
#include "SDL_syscdrom.h"

/* The system-dependent CD control functions */
static const char *SDL_SYS_CDName(int drive);
static int SDL_SYS_CDOpen(int drive);
static int SDL_SYS_CDGetTOC(SDL_CD *cdrom);
static CDstatus SDL_SYS_CDStatus(SDL_CD *cdrom, int *position);
static int SDL_SYS_CDPlay(SDL_CD *cdrom, int start, int length);
static int SDL_SYS_CDPause(SDL_CD *cdrom);
static int SDL_SYS_CDResume(SDL_CD *cdrom);
static int SDL_SYS_CDStop(SDL_CD *cdrom);
static int SDL_SYS_CDEject(SDL_CD *cdrom);
static void SDL_SYS_CDClose(SDL_CD *cdrom);

static void InitCD();
static void	InitCDBlock();

extern GfsDirName dir_name[MAX_DIR];

static	CdcPly	playdata;
static	CdcPos	posdata;

int  SDL_SYS_CDInit()
{
  InitCD();
  InitCDBlock();

	/* Fill in our driver capabilities */
	SDL_CDcaps.Name = SDL_SYS_CDName;
	SDL_CDcaps.Open = SDL_SYS_CDOpen;
	SDL_CDcaps.GetTOC = SDL_SYS_CDGetTOC;
	SDL_CDcaps.Status = SDL_SYS_CDStatus;
	SDL_CDcaps.Play = SDL_SYS_CDPlay;
	SDL_CDcaps.Pause = SDL_SYS_CDPause;
	SDL_CDcaps.Resume = SDL_SYS_CDResume;
	SDL_CDcaps.Stop = SDL_SYS_CDStop;
	SDL_CDcaps.Eject = SDL_SYS_CDEject;
	SDL_CDcaps.Close = SDL_SYS_CDClose;

	return 0;
}

static const char *SDL_SYS_CDName(int drive)
{
	return "/cd"; // TODO
}

static int SDL_SYS_CDOpen(int drive)
{
	return(drive);
}

#define	TRACK_CDDA	0
static int SDL_SYS_CDGetTOC(SDL_CD *cdrom)
{
  cdrom;

	return 0;
}

/* Get CD-ROM status */
static CDstatus SDL_SYS_CDStatus(SDL_CD *cdrom, int *position)
{
  cdrom;
  position;

	return	CD_ERROR;
}

/* Start play */
static int SDL_SYS_CDPlay(SDL_CD *cdrom, int start, int length)
{
  cdrom;
  start;
  length;
	return -1;
}

/* Pause play */
static int SDL_SYS_CDPause(SDL_CD *cdrom)
{
	cdrom;
	return -1;
}

/* Resume play */
static int SDL_SYS_CDResume(SDL_CD *cdrom)
{
	cdrom;
	return -1;
}

/* Stop play */
static int SDL_SYS_CDStop(SDL_CD *cdrom)
{
  cdrom;
	return -1;
}

/* Eject the CD-ROM */
static int SDL_SYS_CDEject(SDL_CD *cdrom)
{
  cdrom;
	return -1;
}

/* Close the CD-ROM handle */
static void SDL_SYS_CDClose(SDL_CD *cdrom)
{
  cdrom;
}

void SDL_SYS_CDQuit(void)
{

}

//--------------------------------------------------------------------------------------------------------------------------------------
static void InitCD()
{
  Uint32 lib_work[GFS_WORK_SIZE(MAX_OPEN) / sizeof(Uint32)];
  GfsDirTbl dirtbl;

  //CdUnlock();

  CDC_CdInit(0x00,0x00,0x05,0x0f);
  GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME(&dirtbl) = dir_name;
  GFS_DIRTBL_NDIR(&dirtbl) = MAX_DIR;
  GFS_Init(MAX_OPEN, lib_work, &dirtbl);
}

//--------------------------------------------------------------------------------------------------------------------------------------
static void	InitCDBlock()
{
//    InitSoundDriver();

    CDC_PLY_STYPE(&playdata) = CDC_PTYPE_TNO;	/* set by track number.*/
    CDC_PLY_STNO( &playdata) = 2;		/* start track number. */
    CDC_PLY_SIDX( &playdata) = 1;		/* start index number. */
    CDC_PLY_ETYPE(&playdata) = CDC_PTYPE_TNO;	/* set by track number.*/
    CDC_PLY_ETNO( &playdata) = 10;		/* start track number. */
    CDC_PLY_EIDX( &playdata) = 99;		/* start index number. */
    CDC_PLY_PMODE(&playdata) = CDC_PTYPE_NOCHG;//CDC_PM_DFL + 30;	/* Play Mode. */ // lecture en boucle
//    CDC_PLY_PMODE(&playdata) = CDC_PTYPE_NOCHG;//CDC_PM_DFL+30;//CDC_PM_DFL ;	/* Play Mode. */ // lecture unique
}
