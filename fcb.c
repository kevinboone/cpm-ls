/*==========================================================================

  fcb.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

==========================================================================*/

#include "compat.h"
#include "fcb.h"

/*==========================================================================

  fcb_fill_ptn

==========================================================================*/
BOOL fcb_fill_ptn (thing, fcb, has_file)
char *thing;
char *fcb;
BOOL *has_file;
  {
  int i, l, o, j;
  int drive = 0; /* 0 = default drive */

  if (thing[1] == ':')
    {
    char dlet;
    dlet = thing[0];
    drive = dlet - 'A' + 1;
    thing += 2;
    } 

  if (drive < 0 || drive > 26)
    {
    putch (drive + 'A' - 1);
    puteol();
    return FALSE;
    }

  /* At this point, "thing" may point to a pattern, a file, or a terminating
     zero (if no argument was supplied) */

  fcb[0] = drive;

  l = strlen (thing); 

  if (l == 0)
    {
    /* Start with all the filename match characters as wildcards */
    for (i = 1; i < 12; i++)
      {
      fcb[i] = '?';
      }
    *has_file = FALSE;
    }
  else
    {
    /* Start with all the filename match characters as spaces */
    for (i = 1; i < 12; i++)
      {
      fcb[i] = ' ';
      }
    o = 1;
    for (i = 0; i < l && o < 12; i++)
      {
      char c = thing[i];
      if (c == '.')
        {
	o = 9;
        }
      else if (c == '*')
	{
        if (o < 9)
          { 
          for (j = o; j < 9; j++)
            fcb[o++] = '?';
          }
        else
          { 
          for (j = o; j < 12; j++)
            fcb[o++] = '?';
          }
        }
      else
	{
        fcb[o] = c;
        o++;
	}
      }
    *has_file = TRUE;
    }

  return TRUE; /* TODO */
  }


