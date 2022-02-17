/*===========================================================================

  dir.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

===========================================================================*/
#ifndef __DIR_H
#define __DIR_H

/* Flags that control the display of the file information */
#define LS_LONG	   0x0001
#define LS_ST_SZ   0x0002
#define LS_ST_NM   0x0004
#define LS_ST_XT   0x0008
#define LS_ST_DEC  0x0010
#define LS_PAGE    0x0020
#define LS_BLK     0x0040
#define LS_ST_FLAGS (LS_ST_SZ | LS_ST_NM | LS_ST_XT)

/* int do_dir (char *thing, BOOL show_pattern, int flags)
   Lists a directory. "thing" may be empty, or contain a specific
   file, or a pattern with wildcards. If show_pattern is true,
   print the filename pattern at the start of the listing. 
*/
int dir_do();

#endif
