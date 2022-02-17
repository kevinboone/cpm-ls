# cpm-ls

An implementation of the Unix `ls` utility for CP/M. 

Version 0.1, February 2022

## What is this?

`cpm-ls` is a file lister for CP/M (Z80 or 8080) broadly similar to the Linux
`ls` utility. There are many such implementations for CP/M; I've tried here
to make a reasonably fully-featured one that fits into less than 10kB. In
particular, though, I've tried to implement something that can, if necessary,
be maintained on a CP/M system, using original CP/M tools, not just
cross-compiled on a modern workstation. 

This implementation is, of course, much simpler than the Linux one, because
CP/M stores only a rudimentary set of information about a file, and does not
support directories.

## Usage

    ls [options] [drive:pattern] [drive:pattern]...

Options can be specified using the '/' or '-' characters.

*-b* In 'long' format, display file size in CP/M blocks rather than
bytes/kilobytes

*-l* Display in long format, one file to a line, including file attributes
and size. Attributes are displayed like this:

    [RSA]

where R=read-only, S=system, A=archive.

*-p* Display a page at a time. After each page, hit 'enter' to advance
by one line, 'space' to advance by a page, or ctrl-c to exit.

*-r* Sort in the opposite order to the default (i.e., reverse alphabetic
order).

*-s* Sort by size.

*-u* Unsorted -- do not sort at all.

*-x* Sort by file extension.

The drive/pattern specification can contain wildcards (see below), 
such as "b:\*.com". By default, files are sorted in alphabetic order of
name.  If no drive letter is given, the current logged-in drive is
assumed.

## Wildcards

* and ? can be used as wildcards. Patterns are not as flexible as
on Linux. 

`*.com` matches anything that has the extension `com`.

`c*.com` matches anything that begins with `c`, and has the extension `com`.

`*c.com` is treated the same as `*.com`

`*` matches anything _without_ an extension (as in MSDOS). 
Unlike in Unix/Linux, the extension in CP/M is a first-class part of 
a file, not just something following a period. 

`*.*` matches everything, with or without extension. Specifying this as a 
pattern is equivalent to not specifying anything.  

## Building

This utility will build on CP/M using the Manx Aztec C compiler, version
1.06d. It will also build on Linux, using a CP/M emulator and the same
compiler. See the Makefile for where to get the necessary bits.

Building on CP/M amounts to compiling all the C files, assembling all
the ASM files, and then linking all the resulting object files. On Linux,
use the Makefile, which automates these steps.

## Implementation

`cpm-ls` is written in a mixture of C and assembly language.  The assembly
parts are simple -- they just do console I/O. Doing these things in assembly
prevents the linker incorporating all the buffered I/O stuf from the standard
library, reducing the code size by about 6k (that doesn't sound much, but
it's about 30%).

## Limitations

By default, `cpm-ls` lists a maximum of 256 files, allowing it to run on
system with 16kB RAM without much memory checking.

Sorting files, particular by name, can be slow on low-powered systems (and
"low-powered" really means something in the CP/M world).

File sizes are display in kB when larger than 1 kB. This is not just for
neatness, but to avoid any need for arithmetic of more than 16-bit range
(which would make the program significantly larger).

## Legal and copyright

Copyright (c)2022 Kevin Boone, and released under the terms of the GNU Public
Licence v3.0. 

