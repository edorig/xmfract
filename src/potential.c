
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: potential.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

int potential(double mag, int iterations)
{
  float f_mag,f_tmp,pot;
  double d_tmp;
  int i_pot;
  long l_pot;

  if(iterations < maxit)
    {
    pot = i_pot = iterations+2;

    if(i_pot <= 0 || mag <= 1.0)
      pot = 0.0;
    else /* pot = log(mag) / pow(2.0, (double)pot); */
      {
      if(i_pot < 120 && !floatflag) /* empirically determined limit of fShift */
        {
        f_mag = mag;
        fLog14(f_mag,f_tmp); /* this SHOULD be non-negative */
        fShift(f_tmp,(char)-i_pot,pot);
        }
      else
        {
        d_tmp = log(mag)/(double)pow(2.0,(double)pot);

        if(d_tmp > FLT_MIN) /* prevent float type underflow */
          pot = d_tmp;
        else
          pot = 0.0;
        }
      }

    /* following transformation strictly for aesthetic reasons */

    /* meaning of parameters:
     *
     * potparam[0] -- zero potential level - highest color -
     * potparam[1] -- slope multiplier -- higher is steeper
     * potparam[2] -- rqlim value if changeable (bailout for modulus)
     */

    if(pot > 0.0)
      {
      if(floatflag)
        pot = (float)sqrt((double)pot);
      else
        {
        fSqrt14(pot,f_tmp);
        pot = f_tmp;
        }
      pot = potparam[0] - pot*potparam[1] - 1.0;
      }
    else
      pot = potparam[0] - 1.0;

    if(pot < 1.0)
      pot = 1.0; /* avoid color 0 */
    }
  else if(inside >= 0)
    pot = inside;
  else /* inside < 0 implies inside=maxit, so use 1st pot param instead */
    pot = potparam[0];

  i_pot = (l_pot = pot * 256) >> 8;

  if(i_pot >= colors)
    {
    i_pot = colors - 1;
    l_pot = 255;
    }

  if(pot16bit)
    {
    writedisk(col+sxoffs,row+syoffs,i_pot);
    writedisk(col+sxoffs,row+sydots+syoffs,(int)l_pot);
    }

  return(i_pot);
}

/******************************************************************/
/* Continuous potential calculation for Mandelbrot and Julia      */
/* Reference: Science of Fractal Images p. 190.                   */
/* Special thanks to Mark Peterson for his "MtMand" program that  */
/* beautifully approximates plate 25 (same reference) and spurred */
/* on the inclusion of similar capabilities in xmfract.           */
/*                                                                */
/* The purpose of this function is to calculate a color value     */
/* for a fractal that varies continuously with the screen pixels  */
/* locations for better rendering in 3D.                          */
/*                                                                */
/* Here "magnitude" is the modulus of the orbit value at          */
/* "iterations". The potparms[] are user-entered paramters        */
/* controlling the level and slope of the continuous potential    */
/* surface. Returns color.  - Tim Wegner 6/25/89                  */
/*                                                                */
/*               -- Change history --                             */
/*                                                                */
/* 09/12/89   - added floatflag support and fixed float underflow */
/*                                                                */
/******************************************************************/
/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: potential.c,v $
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
