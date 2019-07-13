/*
 * Copyright (C) 1995 Thomas Winder Tu-Wien
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 * $Id: stricmp.c,v 1.1 1995/03/14 12:22:10 tom Exp $
 * $Log: stricmp.c,v $
 * Revision 1.1  1995/03/14  12:22:10  tom
 * Initial revision
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <sys/types.h>

#ifdef __STDC__
int
stricmp (const char *s1, const char *s2)
#else
stricmp (s1, s2)
  char *s1, *s2;
#endif
{
  int c1, c2;

  while (1)
    {
    c1 = *s1++;
    c2 = *s2++;
    if (isupper (c1))
      c1 = tolower (c1);
    if (isupper (c2))
      c2 = tolower (c2);
    if (c1 != c2)
      {
      return c1 - c2;
      }
    if (c1 == 0)
      {
      return 0;
      }
    }
}

#ifdef __STDC__
int
strnicmp (const char *s1, const char *s2, size_t n)
#else
strnicmp (s1, s2, n)
  char *s1, *s2;
  int n;
#endif
{
  int c1, c2;

  while (n--)
    {
    c1 = *s1++;
    c2 = *s2++;
    if (isupper (c1))
      c1 = tolower (c1);
    if (isupper (c2))
      c2 = tolower (c2);
    if (c1 != c2)
      {
      return c1 - c2;
      }
    if (c1 == 0)
      {
      return 0;
      }
    }
  return 0;
}
