
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: attractor.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

void get_julia_attractor (double real, double imag)
{
  _LCMPLX lresult;
  _CMPLX result;
  int savper,savmaxit;
  int i;

  if (attractors == 0 && finattract == 0) /* not magnet & not requested */
    return;

  if (attractors >= N_ATTR) /* space for more attractors ?  */
    return;                 /* Bad luck - no room left !    */

  savper = periodicitycheck;
  savmaxit = maxit;
  periodicitycheck = 0;
  old.x = real; /* prepare for f.p orbit calc */
  old.y = imag;
  tempsqrx = sqr(old.x);
  tempsqry = sqr(old.y);

  lold.x = real; /* prepare for int orbit calc */
  lold.y = imag;
  ltempsqrx = tempsqrx;
  ltempsqry = tempsqry;

  lold.x = lold.x << bitshift;
  lold.y = lold.y << bitshift;
  ltempsqrx = ltempsqrx << bitshift;
  ltempsqry = ltempsqry << bitshift;

  if (maxit < 500) /* we're going to try at least this hard */
     maxit = 500;
  color = 0;
  while (++color < maxit)
    if(curfractalspecific->orbitcalc())
      break;

  if (color >= maxit) /* if orbit stays in the lake */
    {
    if (integerfractal) /* remember where it went to */
      lresult = lnew;
    else
      result =  new;
    for (i=0;i<10;i++)
      {
      if(!curfractalspecific->orbitcalc()) /* if it stays in the lake */
        {                                  /* and doesn't move far, probably */
        if (integerfractal)   /* found a finite attractor */
          {
          if(labs(lresult.x-lnew.x) < lclosenuff &&
               labs(lresult.y-lnew.y) < lclosenuff)
            {
            lattr[attractors] = lnew;
            attrperiod[attractors] = i+1;
            attractors++;   /* another attractor - coloured lakes ! */
            break;
            }
          }
        else
           {
           if(fabs(result.x-new.x) < closenuff &&
           fabs(result.y-new.y) < closenuff)
             {
             attr[attractors] = new;
             attrperiod[attractors] = i+1;
             attractors++;   /* another attractor - coloured lakes ! */
             break;
             }
           }
         }
       else
        {
        break;
        }
      }
    }
  if(attractors==0)
    periodicitycheck = savper;

  maxit = savmaxit;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: attractor.c,v $
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
