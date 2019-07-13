
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: calcmand.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* fast per pixel 1/2/b/g, called with row & col set */
int calcmand()
{
  /* map color, but not if maxit & adjusted for inside,etc */
  if (LogTable && (realcolor < maxit || (inside < 0 && color == maxit)))
    color = LogTable[min(color, maxit)];
  if (color >= colors) /* don't use color 0 unless from inside/outside */
    if (colors < 16)
      color &= andcolor;
    else
      color = ((color - 1) % andcolor) + 1;  /* skip color zero */
  if(debugflag != 470)
    if(color <= 0 && stdcalcmode == 'b' )   /* fix BTM bug */
      color = 1;
  (*plot) (col, row, color);
  return (color);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: calcmand.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.101  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
