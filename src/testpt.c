
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: testpt.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 * Write your fractal program here. initreal and initimag are the values in
 * the complex plane; parm1, and parm2 are paramaters to be entered with the
 * "params=" option (if needed). The function should return the color associated
 * with initreal and initimag.  Xmfract will repeatedly call your function with
 * the values of initreal and initimag ranging over the rectangle defined by the
 * "corners=" option. Assuming your formula is iterative, "maxit" is the maximum
 * iteration. If "maxit" is hit, color "inside" should be returned.
 *
 * Note that this routine could be sped up using external variables/arrays
 * rather than the current parameter-passing scheme.  The goal, however was
 * to make it as easy as possible to add fractal types, and this looked like
 * the easiest way.
 *
 * The sample code below is a straightforward Mandelbrot routine.
 */

int teststart()     /* this routine is called just before the fractal starts */
{
  return( 0 );
}

void testend()       /* this routine is called just after the fractal ends */
{
}

  /* this routine is called once for every pixel */
  /* (note: possibly using the dual-pass / solid-guessing options */

int testpt(
  double initreal,
  double initimag,
  double parm1,
  double parm2,
  int maxit,
  int inside)
{
  double oldreal, oldimag, newreal, newimag, magnitude;
  int color;
  oldreal=parm1;
  oldimag=parm2;
  magnitude = 0.0;
  color = 0;
  while ((magnitude < 4.0) && (color < maxit))
    {
    newreal = oldreal * oldreal - oldimag * oldimag + initreal;
    newimag = 2 * oldreal * oldimag + initimag;
    color++;
    oldreal = newreal;
    oldimag = newimag;
    magnitude = newreal * newreal + newimag * newimag;
    }
  if (color >= maxit)
    color = inside;
  return(color);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: testpt.c,v $
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
 * Revision 0.301  1995/01/10  00:40:37  darryl
 * Add comments, delete references to obsolete code.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
