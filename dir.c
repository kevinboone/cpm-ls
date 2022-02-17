/*==========================================================================

  dir.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

==========================================================================*/

#include "config.h"
#include "compat.h"
#include "string.h"
#include "dir.h"
#include "conio.h"

/* BDOS constants */
#define FCB 0x5c
#define BDOS_DFIRST 17
#define BDOS_DNEXT 18
#define BDOS_FSIZE 35 
#define BDOS_OPEN 15 
#define BDOS_CLOSE 16 
#define DMABUF 0x80 
/* The MSBs of filename characters are actually attribute flags */
#define BD_CHR_MASK 0x7F
#define BD_ATT_MASK 0x80

/* File attributes */
#define ATT_RO   0x01
#define ATT_SYS  0x02
#define ATT_ARCH 0x04

/* struct file contains information collected while enumerating the
   drive. Some of it is duplicated, in the sense that some entries
   are derived from others. However, it seems reasonable to sacrifice
   a little memory to save a lot of time, by avoiding the need to 
   recalculate. */
struct File 
  {
  char name[13]; /* In display format 8.3, no padding, zero terminated */
  char cpmname[12]; /* Original 8/3 name, upper case, no padding */
  unsigned recs; /* Number of records */
  int nlen; /* Length of (formatted) name, to save calculating it again */
  int attr; /* File attribute flags */
  };

/* Block of data to store the enumerated file information. Making it 
 * static avoids the need to include malloc(), etc. */
struct File files[LS_MAX_F]; 

/* Number of lines written since the last page, when using paged display */
int lines = 0;

/*==========================================================================

  dir_eol

  Write a new line. Take care of paging, if paging is enabled.

==========================================================================*/
void dir_eol (page)
BOOL page;
  {
  puteol(); 
  if (!page) return;
  lines++;
  if (lines == NUM_ROWS - 1)
    {
    int c;
    puts2 ("MORE (ent/spc/ctl+c)");
    c = getchne();
    putch (13);
    puts2 ("                           ");
    putch (13);
    if (c == INTR)
      exit(); 
    else if (c == 10 || c == 13)
      lines--;
    else
      lines = 0;
    }
  }


/*==========================================================================

  dir_san

  Sanitize an FCB filename, making it more like something one might
  actually type. So "MAIN    ASM" becomes "main.asm". Note that the
  output string must have capacity of 13 characters, because it might
  contain a period, and it will have a zero terminator. Note that the
  period is suppressed in the display if there is no extension.

  Returns the length of the formatted filename, which the caller
  can store, so we don't have to calculate it again later.

==========================================================================*/
int dir_san (in, out)
char *in;
char *out;
  {
  BOOL has_ext = TRUE; /* Assume filename has an extension... */
  register int i, o = 0;
  if (in[8] == ' ') has_ext = FALSE; /* ...unless it starts with space */
  for (i = 0; i < 8; i++)
    {
    /* Strip the file attributes in the MSB of each character */
    char c = in[i] & BD_CHR_MASK;
    if (c >= 'A' && c <= 'Z') c |= 32;
    if (c == ' ')
      {
      out[i] = 0; 
      break;
      }
    else
      {
      out[i] = c;
      out[i + 1] = 0;
      }
    o++;
    }
  if (has_ext)
    {
    out[o] = '.';
    o++;
    for (i = 8; i < 11; i++)
      {
      char c = in[i] & BD_CHR_MASK;
      if (c >= 'A' && c <= 'Z') c |= 32;
      if (c == ' ')
	{
	out[o] = 0; 
	break;
	}
      else
        {
	out[o] = c;
	out[o + 1] = 0;
        }
      o++;
      }
    }
  out[o + 1] = 0;
  return o;
  }

/*==========================================================================

  dir_fl_off

  Helper function for determining the location in the file data block
  of the record for a particular file.

==========================================================================*/
struct File *dir_fl_off (n)
int n;
  {
  return files + n * sizeof (struct File);
  }


/*==========================================================================

  dir_sort

  I'm using an ugly, lazy exchange sort here. Moreover, the file list
  is stored as sequential data blocks, not as pointers. This means that
  each exchange is a block exchange of about 40 bytes. This needs to
  be redone, to improve efficiency, but this will mean changing the way the 
  file data is stored.

==========================================================================*/
void dir_sort (num_files, flags)
int num_files;
int flags;
  {
  register int i, j;
  struct File temp;

  for (j = 0; j < num_files; j++)
    {
    for (i = j; i < num_files; i++)
      {
      BOOL swap = FALSE;
      struct File *f1 = dir_fl_off (i); 
      struct File *f2 = dir_fl_off (j); 
      if (flags & LS_ST_XT)
        {
        if (strcmp (f1->cpmname + 8, f2->cpmname + 8) < 0)
          swap = TRUE;
        }
      else if (flags & LS_ST_NM)
        {
        if (strcmp (f1->name, f2->name) < 0)
          swap = TRUE; 
        }
      else
        {
        /* Sort by size */
        if (f1->recs > f2->recs)
          swap = TRUE;
        }
      if (flags & LS_ST_DEC) swap = !swap;
      if (swap)
        {
        memcpy (&temp, f1, sizeof (struct File));
        memcpy (f1, f2, sizeof (struct File));
        memcpy (f2, &temp, sizeof (struct File));
        }
      }
    }
  }

/*==========================================================================

  dir_do

  This is where all the action happens. Given a specific drive:pattern
  specification, display the files, in a matter determined by the flags.

==========================================================================*/
int dir_do (thing, show_pattern, flags)
char *thing;
BOOL show_pattern;
int flags;
  {
  char tempfcb[36];
  int max_nlen = 0; /* Maximum length of filename found */
  int max_across; /* Maximum files across in wide mode */
  int across = 0; /* Number across so far in wide mode */
  int n, i;
  int num_files = 0; /* Number of filles found on drive */
  BOOL has_file; /* True if the command line specified a file */
  char *fcb = FCB; /* Use default FCB */
  BOOL page = flags & LS_PAGE;

  /* Show the directory pattern at top of listing if asked for */
  if (show_pattern)
    {
    puts2 (thing);
    puts2 (":");
    dir_eol (page);
    }

  /* Fill in the FCB drive and filename fields based on the supplied
     drive and file spec. */
  if (!fcb_fill_ptn (thing, fcb, &has_file)) 
    {
    puts2 ("Bad file or pattern: ");
    puts2 (thing);
    dir_eol (page);
    return 1;
    }

  /* Loop around the directory, filling in the files structure as we go */
  n = bdos (BDOS_DFIRST, FCB);
  while (n != 255 && (num_files < LS_MAX_F))
    {
    /*struct File *file = files + num_files * sizeof (struct File);*/
    int namelen;
    char temp_name [12];
    char *fcbbuf = DMABUF + 32 * n;
    struct File *file = dir_fl_off (num_files);
    char *cpmname = file->cpmname;
    /* Store both the CPM name and the display name for each file, as
       we will need both later. We sacrifice a bit of storage for
       increased speed. */
    file->attr = 0;
    namelen = dir_san (fcbbuf + 1, file->name);
    memcpy (file->cpmname, fcbbuf + 1, 11);
    cpmname[12] = 0;
    if (cpmname[9] & BD_ATT_MASK)
      file->attr |= ATT_RO;
    if (cpmname[10] & BD_ATT_MASK)
      file->attr |= ATT_SYS;
    if (cpmname[11] & BD_ATT_MASK)
      file->attr |= ATT_ARCH;

    /* Work out the maximum filename length, used for formatting later */
    if (namelen > max_nlen) max_nlen = namelen;

    file->nlen = namelen;
    num_files++;
    n = bdos (BDOS_DNEXT, FCB);
    }

  if (num_files == 0)
    {
    /* It's not an error if there are no files, _except_ if the
       user actually specified a particular filename as an argument. */
    if (has_file) 
      {
      puts2 ("Not found: ");
      puts2 (thing);
      puteol ();
      }
    return;
    }

  /* Calculate the file sizes, if later operations require it. */
  if (flags & (LS_LONG | LS_ST_SZ)) 
    {
    for (i = 0; i < num_files; i++)
      {
      int j;
      /*struct File *file = files + i * sizeof (struct File);*/
      struct File *file = dir_fl_off (i);
      for (j = 0; j < sizeof (tempfcb); j++) tempfcb[j] = 0;
      tempfcb[0] = fcb[0];
      memcpy (tempfcb + 1, file->cpmname, 11);
      if (bdos (BDOS_OPEN, tempfcb) == 0)
	{
	bdos (BDOS_FSIZE, tempfcb);
	file->recs = tempfcb[33] + 256 * tempfcb[34];
	}
      else
	{
	/* Should we display an error here? */
	file->recs = 0; 
	}
      bdos (BDOS_CLOSE, tempfcb);
      }
    }

  if (flags & (LS_ST_FLAGS))
    dir_sort (num_files, flags);

  /* Display the files */

  if (flags & LS_LONG)
    {
    for (i = 0; i < num_files; i++)
      {
      char s[MAXDECSTR];
      int j;
      /*struct File *file = files + i * sizeof (struct File);*/
      struct File *file = dir_fl_off (i);
      int pad = max_nlen - file->nlen + 1;
      /* Display the file attributes */
      putch ('[');
      file->attr & ATT_RO ? putch ('R') : putch ('-');
      file->attr & ATT_SYS ? putch ('S') : putch ('-');
      file->attr & ATT_ARCH ? putch ('A') : putch ('-');
      putch (']');
      putch (' ');
      puts2 (file->name);
      for (j = 0; j < pad; j++)
	putch (' ');

      if (flags & LS_BLK)
        {
	utoa (file->recs, s, 10); 
	puts2 (s);
        }
      else
        {
	/* Display file size in kB, for any file > 1kB. This is not
	   just neater -- it obviates the need to use any arithmetic
	   in more than 16 bits */
	if (file->recs >= 8)
	  {
	  /* Each 128-byte record is one eighth of a kB) */
	  utoa ((int)file->recs / 8, s, 10);  
	  puts2 (s);
	  putch ('k');
	  }
	else
	  {
	  utoa (file->recs * 128, s, 10); 
	  puts2 (s);
	  }
        }
      dir_eol (page); 
      }
    }
  else
    {
    /* TODO -- externalize terminal width */
    max_across = NUM_COLS / (max_nlen + 1);

    for (i = 0; i < num_files; i++)
      {
      int j;
      struct File *file = dir_fl_off (i);
      int pad = max_nlen - file->nlen + 1;
      puts2 (file->name);
      for (j = 0; j < pad; j++)
	puts2 (" ");
      across++;
      if (across == max_across)
	{
	across = 0;
	dir_eol (page);
	}
      }
    if (across != 0)
      dir_eol (page);
    }

  return 0;
  }

