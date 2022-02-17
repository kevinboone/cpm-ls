/*==========================================================================

  fcb.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

  Functions for handling BDOS file control blocks

==========================================================================*/

#ifndef __FCB_H
#define __FCB_H

#include "compat.h"

/*
BOOL fcb_fill_ptn (char *thing, char *fcb, BOOL *has_file)

Fill the drive and filename part of an FCB (the first 12 bytes) from
a filename referenced by "thing". This is of the form "X:NAME.EXT".
If no drive letter is given, it is set to 0 in the FCB, indicating
the current drive. Wildcards "*" are expanded. "?" wildcards are
inserted verbatime, where they will function as wildcards in
a directory listing.

*has_file is set to true if there is, in fact, a filename in "thing".
In most cases it will be an error not to supply a filename, but it
is appropriate when doing a directory expansion. Thus, if there is
no filename, the whole filename part of the FCB is filled in
with "?" characters, to match anything.

Returns TRUE if the operation succeeds. At present, the only reason
for failure is an invalid drive letter. 
*/
BOOL fcb_fill_ptn ();

#endif
