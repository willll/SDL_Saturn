#ifndef _GFS_BYTE_WRAPPER_
#define _GFS_BYTE_WRAPPER_

#include <sega_gfs.h>
#include <stdio.h>

#define   OPEN_MAX  (Sint32)6
#define   DIR_MAX   (Sint32)8


#define SECTOR_SIZE 2048 // Bytes for each sector

#define FNAME_SIZE 15

struct SDL_RWops;

typedef struct GFS_FILE {
	GfsHn  fid;       // Pointer to GFS file structure
	long f_size;      // file size in *bytes*
	long  f_seek_pos; // seek pointer position in *bytes*
	Uint32 file_hash; // file hash
} GFS_FILE;

#define FILE GFS_FILE

/* Seek to 'offset' relative to whence, one of stdio's whence values:
	SEEK_SET, SEEK_CUR, SEEK_END
	 Returns the final offset in the data source.
 */
int sat_seek(struct SDL_RWops *context, int offset, int whence);

/* Read up to 'num' objects each of size 'objsize' from the data
	 source to the area pointed at by 'ptr'.
	 Returns the number of objects read, or -1 if the read failed.
 */
int sat_read(struct SDL_RWops *context, void *ptr, int size, int maxnum);

/* Write exactly 'num' objects each of size 'objsize' from the area
	 pointed at by 'ptr' to data source.
	 Returns 'num', or -1 if the write failed.
 */
int sat_write(struct SDL_RWops *context, const void *ptr, int size, int num);

/* Close and free an allocated SDL_FSops structure */
int sat_close(struct SDL_RWops *context);

void init_GFS();

GFS_FILE *sat_fopen(const char *path);
int sat_fclose(GFS_FILE *fp);

int sat_fseek(GFS_FILE *stream, long offset, int whence);
long sat_ftell(GFS_FILE *stream);

size_t sat_fread(void *ptr, size_t size, size_t nmemb, GFS_FILE *stream);
char *sat_fgets(char *s, int size, GFS_FILE *stream);

size_t sat_fwrite(const void *ptr, size_t size, size_t nmemb, GFS_FILE *stream);

int sat_feof(GFS_FILE *stream);

int sat_ferror(GFS_FILE *stream);

long sat_fsize(GFS_FILE *stream);

#endif
