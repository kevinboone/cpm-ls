/*===========================================================================

  string.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

===========================================================================*/
#ifndef __STRING_H
#define __STRING_H

/* Size of the buffer needed to hold a string representation of a 
 * decimal (16-bit) integer (including the sign and the zero terminator) */
#define MAXDECSTR 7

/* 
 * void strlwr (char *s) 
 * Convert string to lower case
 */
void strlwr();

int strlen();
int strcmp();

/*
 * void utoa (unsigned n, char *s, int base)
 * Convert an unsigned integer to a string in the specified base.
 * The array s must be large enough to store the converted value.
 * For decimal, use MAXDECSTR
 */
void utoa ();

/*
 * void reverse (char *s, int l)
 * If l == 0, calculate the length. 
 * Reverse the string s in place. The data need not be zero-terminated
 * unless l == 0
 */
void reverse();

#endif

