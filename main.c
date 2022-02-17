/*==========================================================================

  main.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

==========================================================================*/

#include "compat.h"
#include "dir.h"
#include "getopt.h"
#include "conio.h"

/*==========================================================================

  show_help 

==========================================================================*/
void show_help()
  {
  puts2 ("Usage: "); puts2 (NAME); puts2 (" {-blprsux} {drive:pattern}"); 
  puteol();
  puts2 ("  /b sz in blks");
  puteol();
  puts2 ("  /l long");
  puteol();
  puts2 ("  /p page");
  puteol();
  puts2 ("  /r reverse");
  puteol();
  puts2 ("  /s sort by sz");
  puteol();
  puts2 ("  /u unsorted");
  puteol();
  puts2 ("  /x sort by ext");
  puteol();
  }

/*==========================================================================

  main

==========================================================================*/
int main (argc, argv)
int argc;
char **argv;
  {
  int i;
  int opt;
  int lsflags = LS_ST_NM; /* default sort by name, wide */

  while ((opt = getopt (argc, argv, "BLHPRSUX")) != -1)
    {
    switch (opt)
      {
      case '?':
      case ':':
      case 'H':
        show_help ();
        return 0; 
      case 'B':
        lsflags |= LS_BLK;
        break;
      case 'L':
        lsflags |= LS_LONG;
        break;
      case 'P':
        lsflags |= LS_PAGE;
        break;
      case 'R':
        lsflags |= LS_ST_DEC;
        break;
      case 'U':
        lsflags &= ~LS_ST_FLAGS;
        break;
      case 'S':
        lsflags &= ~LS_ST_FLAGS;
        lsflags |= LS_ST_SZ;
        break;
      case 'X':
        lsflags &= ~LS_ST_FLAGS;
        lsflags |= LS_ST_XT;
        break;
      default:
        return 1;
      }
    }

  if (argc == optind)
    {
    dir_do ("", FALSE, lsflags);
    }
  else
    { 
    for (i = optind; i < argc; i++)
      {
      if (i != optind) puteol(); 
      dir_do (argv[i], (argc - optind) > 1, lsflags);
      }
    }

  return 0; /* Return value unused in CP/M */
  }


