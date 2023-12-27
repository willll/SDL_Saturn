
#include "SDL_saturn.h"

#include	<sega_scl.h>
#include	<sega_int.h>
#include	<sega_mem.h>
#include	<sgl.h>

#include "file/saturn/gfs_wrap.h"

#ifndef DISABLE_FILE
  GfsDirName dir_name[MAX_DIR];
#endif

static void	_intr_v_blank_in( void ){
	SCL_VblankStart();
}

static void	_intr_v_blank_out( void ){
	SCL_VblankEnd();

	//PER_GetPort( __port );
}

void _init_saturn(void) {
  MEM_Init( ( Uint32 )__heap, sizeof( __heap ));
  #ifndef DISABLE_FILE
    init_GFS();
  #endif
}
