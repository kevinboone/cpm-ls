/*===========================================================================

  compat.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

===========================================================================*/
#ifndef __COMPAT_H
#define __COMPAT_H

#define MAX_ARGC  10

#ifdef CPM
#define CONST
#else
#define CONST const
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef BOOL
#define BOOL int
#endif

#endif /* compat.h */

