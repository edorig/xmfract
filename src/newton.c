
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: newton.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static _CMPLX staticroots[16];
static double d1overd;
static double roverd;

int NewtonSetup()        /* Newton/NewtBasin Routines */
{
  int i;
  if (debugflag != 1010)
    {
    if(fractype == MPNEWTON)
      fractype = NEWTON;
    else if(fractype == MPNEWTBASIN)
      fractype = NEWTBASIN;
    curfractalspecific = &fractalspecific[fractype];
    }

  /* set up table of roots of 1 along unit circle */

  degree = (int)parm.x;

  if(degree < 2)
    degree = 3;   /* defaults to 3, but 2 is possible */

  root = 1;

  /* precalculated values */

  roverd = (double)root / (double)degree;
  d1overd = (double)(degree - 1) / (double)degree;
  maxcolor = 0;
  threshold = .3*PI/degree; /* less than half distance between roots */

  floatmin = FLT_MIN;
  floatmax = FLT_MAX;

  basin = 0;
  if(roots != staticroots)
    {
    XtFree((char *) roots);
    roots = staticroots;
    }

  if (fractype==NEWTBASIN)
    {
    if(parm.y)
      basin = 2; /*stripes */
    else
      basin = 1;
    if(degree > 16)
      {
      if((roots=(_CMPLX *)XtMalloc(degree*sizeof(_CMPLX)))==NULL)
        {
        roots = staticroots;
        degree = 16;
        }
      }
    else
      roots = staticroots;

    /* list of roots to discover where we converged for newtbasin */

    for(i=0;i<degree;i++)
      {
      roots[i].x = cos(i*twopi/(double)degree);
      roots[i].y = sin(i*twopi/(double)degree);
      }
    }

  param[0] = (double)degree; /* JCO 7/1/92 */

  if (degree%4 == 0)
    symmetry = XYAXIS;
  else
    symmetry = XAXIS;

  calctype=StandardFractal;

  return(1);
}

int NewtonFractal2()
{
  static char start = 1;

  if(start)
    start = 0;

  cpower(&old, degree-1, &tmp);
  complex_mult(tmp, old, &new);

  if (DIST1(new) < threshold)
    {
    if(fractype==NEWTBASIN || fractype==MPNEWTBASIN)
      {
      int tmpcolor;
      int i;
      tmpcolor = -1;

      /*
       * this code determines which degree-th root of root the
       * Newton formula converges to. The roots of a 1 are
       * distributed on a circle of radius 1 about the origin.
       */

      for(i = 0; i < degree; i++)
        {
        /*
         * color in alternating shades with iteration according to
         * which root of 1 it converged to
         */
        if(distance(roots[i],old) < threshold)
          {
          if (basin==2)
            tmpcolor = 1+(i&7)+((color&1)<<3);
          else
            tmpcolor = 1+i;
          break;
          }
        }

      if(tmpcolor == -1)
        color = maxcolor;
      else
        color = tmpcolor;
      }
    return(1);
    }
  new.x = d1overd * new.x + roverd;
  new.y *= d1overd;

  /* Watch for divide underflow */
  if ((t2 = tmp.x * tmp.x + tmp.y * tmp.y) < FLT_MIN)
     return(1);
  else
    {
    t2 = 1.0 / t2;
    old.x = t2 * (new.x * tmp.x + new.y * tmp.y);
    old.y = t2 * (new.y * tmp.x - new.x * tmp.y);
    }

  return(0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: newton.c,v $
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
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
