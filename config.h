/*===========================================================================

  config.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

===========================================================================*/
#ifndef __CONFIG_H
#define __CONFIG_H

/* Maximumum number of files that will be displayed. 256 files corresponds
 * to about 7kB of memory. */
#define LS_MAX_F	256

/* Number of screen columns. Used to calculate the layout in "wide" mode */
#define NUM_COLS	80

/* Number of screen rows. Used to work out how many lines per page */
#define NUM_ROWS        24	

/* The terminal character to interpret as a "kill" interrupt; usually ctrl+c */ 
#define INTR            3	

#endif

