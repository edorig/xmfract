
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: calcmandfp.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static int periodicity_color;

int calcmandfpasmstart()
{
  if (inside < 0)
    inside_color = maxit;
  else
    inside_color = inside;

  if (periodicitycheck < 0)
    periodicity_color = 7;
  else
    periodicity_color = inside_color;

  oldcolor = 0;
  return 0;
}

int calcmandfp()
{
  if(invert)
    invertz2(&init);
  else
    {
    init.y = dy0[row]+dy1[col];
    init.x = dx0[col]+dx1[row];
    }

  if (calcmandfpasm() >= 0)
    {
    if (potflag)
      color = potential(magnitude, realcolor);

    /* map color, but not if maxit & adjusted for inside,etc */
    if (LogTable && (realcolor < maxit || (inside < 0 && color == maxit)))
      color = LogTable[min(color, maxit)];

    if (color >= colors) /* don't use color 0 unless from inside/outside */
      {
      if (colors < 16)
        color &= andcolor;
      else
        color = ((color - 1) % andcolor) + 1;  /* skip color zero */
      }

    if(debugflag != 470)
      if(color == 0 && stdcalcmode == 'b' )   /* fix BTM bug */
        color = 1;

    (*plot) (col, row, color);
    }
  return (color);
}

int calcmandfpasm()
{
  int cx;
  int savedand;
  double x,y,x2, y2, xy, Cx, Cy, savedx, savedy;
  int savedincr;

  if (periodicitycheck==0 || periodicitycheck == -59)
    oldcolor = 0;
  else if (reset_periodicity==0)
    oldcolor = maxit-250;

  /* initparms */
  savedx = 0;
  savedy = 0;
  savedand = 1;
  savedincr = 1;
  orbit_ptr = 0;
  kbdcount--; /* Only check the keyboard sometimes */

  if (kbdcount<0)
    {
    KeySym key;
    kbdcount = 400;
    key = waitkeypressed(0);
    if (key)
      {
      if (key==XMKEY_O)
        {
        getakey();
        show_orbit = 1-show_orbit;
        }
      else if (key == XK_d || key==SPACE || key==SPACE2 || jiiming)
        getakey();
      else
        {
        color = -1;
        return -1;
        }
      }
    }

  cx = maxit;

  if (fractype != JULIAFP && fractype != JULIA)
    {
    /* Mandelbrot_87 */
    cx--;
    Cx = init.x;
    Cy = init.y;
    x = parm.x+Cx;
    y = parm.y+Cy;
    }
  else
    {
    /* dojulia_87 */
    Cx = parm.x;
    Cy = parm.y;
    x = init.x;
    y = init.y;
    }

  x2 = x*x;
  y2 = y*y;
  xy = x*y;

  /* top_of_cs_loop_87 */
  do {
    x = x2-y2+Cx;
    y = 2*xy+Cy;

    if (outside<=-2)
      {
      new.x = x;
      new.y = y;
      }

    /* no_save_new_xy_87 */
    if (inside==-100)
      {
      /* epsilon_cross */
      if (ABS(x)<0.01)
        {
        realcolor = maxit-cx;

        if (realcolor==0)
          realcolor++;

        kbdcount -= realcolor;

        color = GREENCOLOR;
        oldcolor = 0;

        goto pop_stack;
        }
      else if (ABS(y)<0.01)
        {
        realcolor = maxit-cx;

        if (realcolor==0)
          realcolor++;

        kbdcount -= realcolor;

        color = BROWNCOLOR;
        oldcolor = 0;

        goto pop_stack;
        }
      } /* end_epsilon_cross_87 */

    if (cx<oldcolor)
      {
      if (savedand==0)
        {
        savedx = x;
        savedy = y;
        savedincr--;

        if (savedincr==0)
          {
          savedand = (savedand<<1) + 1;
          savedincr = 4;
          }
        else
          {
          if (ABS(x-savedx)<closenuff && ABS(y-savedy)<closenuff)
            {
            oldcolor = 65535;
            realcolor = maxit;
            kbdcount = kbdcount-(maxit-cx);
            color = periodicity_color;
            goto pop_stack;
            }
          }
        }
      }

    /* no_periodicity_check_87 */
    if (show_orbit != 0)
      plot_orbit(x,y,-1);

    /* no_show_orbit_87 */
    x2 = x*x;
    y2 = y*y;
    xy = x*y;

    if (potflag != 0)
      magnitude = x2+y2;

    if (x2+y2 > rqlim)
      goto over_bailout_87;

    cx--;
    } while (cx>0);

  /* reached maxit */
  oldcolor = 65535;
  kbdcount -= maxit;
  realcolor = maxit;

  if (inside==-59)
    color = (x2+y2)*maxit/2+1;
  else
    color = inside_color;

pop_stack:

  if (orbit_ptr)
    scrub_orbit();

  return color;

over_bailout_87:

  if (cx-10>=0)
    oldcolor = cx-10;
  else
    oldcolor = 0;

  color = realcolor = maxit-cx;

  if (realcolor==0)
    realcolor = 1;

  kbdcount -= realcolor;

  if (outside > -2 && outside != -1)
    color = outside;
  else
    {
    /* special_outside */
    if (outside==-2)
      color += new.x + 7;
    else if (outside==-3)
      color += new.y + 7;
    else if (outside==-4)
      {
      if (new.y!=0)
        color *= new.x/new.y;
      }
    else if (outside==-5)
      color +=  new.x + new.y;

    /* check_color */
    if (color<0 || color>maxit)
      color = 0;
    }
  goto pop_stack;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: calcmandfp.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.5  1995/06/02  00:42:22  darryl
 * Fix orbit delay and show orbits modes.
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
