
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: calcfroth.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void set_Froth_palette(void);

/* static variables */
static char froth3_256c[] = "froth3.map";
static char froth6_256c[] = "froth6.map";
static char froth3_16c[] =  "froth316.map";
static char froth6_16c[] =  "froth616.map";

static int froth_altcolor;
static int froth_shades;
static int frothsix;

/* color maps which attempt to replicate the images of James Alexander. */
static void set_Froth_palette(void)
{
  char *mapname;

  if (colorstate != 0) /* 0 means dacbox matches default */
    return;

  if (colors >= 16)
    {
    if (colors >= 256)
      {
      if (frothsix)
        mapname = froth6_256c;
      else
        mapname = froth3_256c;
      }
    else /* colors >= 16 */
      {
      if (frothsix)
        mapname = froth6_16c;
      else
        mapname = froth3_16c;
      }
    if (ValidateLuts(mapname) != 0)
      return;
    colorstate = 0; /* treat map as default */
    spindac(0,0);
    }
}

int froth_setup(void)
{
  if (param[0] != 3 && param[0] != 6) /* if no match then */
    param[0] = 3;                     /* make it 3 */
  frothsix = param[0] == 6;
  froth_altcolor = param[1] != 0;
  froth_shades = (colors-1) / (frothsix ? 6 : 3);
  /* rqlim needs to be at least 6 or so */
  if (rqlim < 6.0)
    rqlim=6.0;
  set_Froth_palette();
  /* make the best of the .map situation */
  orbit_color = !frothsix && colors >= 16 ? (froth_shades<<1)+1 : colors-1;
  return 1;
}

/* Froth Fractal type */
int calcfroth(void)   /* per pixel 1/2/g, called with row & col set */
{
  int found_attractor=0;
  double x, y, nx, ny, x2, y2;
  long lx, ly, lnx, lny, lx2, ly2;

/* compiler should handle this at compile time */

  orbit_ptr = 0;
  color = 0;
  if(showdot>0)
    (*plot) (col, row, showdot&(colors-1));
  if (!integerfractal) /* fp mode */
    {
    double close=  CLOSE;
    double a=      FROTH_A;
    double b1=     FROTH_B1;
    double xmidt=  XMIDT;
    double m2=     FROTH_M2;
    double b2=     FROTH_B2;
    double m3=     FROTH_M3;
    double b3=     FROTH_B3;
    double x1min=  X1MIN;
    double x1max=  X1MAX;
    double x2max1= X2MAX1;
    double xmidr=  XMIDR;
    double x3min1= X3MIN1;
    double x2min2= X2MIN2;
    double xmidl=  XMIDL;
    double x3max2= X3MAX2;

    if(invert)
      {
      invertz2(&tmp);
      x = tmp.x;
      y = tmp.y;
      }
    else
      {
      x = dx0[col]+dx1[row];
      y = dy0[row]+dy1[col];
      }

    magnitude = (x2=sqr(x)) + (y2=sqr(y));
    while (!found_attractor && (magnitude < rqlim) && (color < maxit))
      {
      /* simple formula: z = z^2 + conj(z*(-1+ai)) */
      /* but it's the attractor that makes this so interesting */
      nx = x2 - y2 - x - a*y;
      ny = (x+x)*y - a*x + y;
      x = nx;
      y = ny;
      if (frothsix) /* repeat mapping */
        {
        nx = sqr(x) - sqr(y) - x - a*y;
        ny = (x+x)*y - a*x + y;
        x = nx;
        y = ny;
        }
      magnitude = (x2=sqr(x)) + (y2=sqr(y));
      color++;

      if (show_orbit)
        plot_orbit(x, y, -1);

        if (x > x1min && x < x1max && fabs(b1-y) < close)
        {
        if (!frothsix || x < xmidt)
          found_attractor = 1;
        else
          found_attractor = 2;
        }
      else if (fabs(m2*x+b2-y) < close)
        {
        if (x > xmidr && x < x2max1)
          found_attractor = !frothsix ? 2 : 4;
        else if (x > x3min1 && x < xmidr)
          found_attractor = !frothsix ? 3 : 6;
        }
      else if (fabs(m3*x+b3-y) < close)
        {
        if (x > x2min2 && x < xmidl)
          found_attractor = !frothsix ? 2 : 3;
        else if (x > xmidl && x < x3max2)
          found_attractor = !frothsix ? 3 : 5;
        }
      }
    }
  else /* integer mode */
    {
    long lclose=   D_TO_L(CLOSE);
    long la=       D_TO_L(FROTH_A);
    long lb1=      D_TO_L(FROTH_B1);
    long lxmidt=   D_TO_L(XMIDT);
    long lm2=      D_TO_L(FROTH_M2);
    long lb2=      D_TO_L(FROTH_B2);
    long lm3=      D_TO_L(FROTH_M3);
    long lb3=      D_TO_L(FROTH_B3);
    long lx1min=   D_TO_L(X1MIN);
    long lx1max=   D_TO_L(X1MAX);
    long lx2max1=  D_TO_L(X2MAX1);
    long lxmidr=   D_TO_L(XMIDR);
    long lx3min1=  D_TO_L(X3MIN1);
    long lx2min2=  D_TO_L(X2MIN2);
    long lxmidl=   D_TO_L(XMIDL);
    long lx3max2=  D_TO_L(X3MAX2);

    if(invert)
      {
      invertz2(&tmp);
      lx = tmp.x * fudge;
      ly = tmp.y * fudge;
      }
    else
      {
      lx = lx0[col] + lx1[row];
      ly = ly0[row] + ly1[col];
      }

    lmagnitud = (lx2=lsqr(lx)) + (ly2=lsqr(ly));
    while (!found_attractor && (lmagnitud < llimit)
           && (lmagnitud > 0) && (color < maxit))
      {
      /* simple formula: z = z^2 + conj(z*(-1+ai)) */
      /* but it's the attractor that makes this so interesting */
      lnx = lx2 - ly2 - lx - multiply(la,ly,bitshift);
      lny = (multiply(lx,ly,bitshift)<<1) - multiply(la,lx,bitshift) + ly;
      lx = lnx;
      ly = lny;
      if (frothsix)
        {
        lmagnitud = (lx2=lsqr(lx)) + (ly2=lsqr(ly));
        if ((lmagnitud > llimit) || (lmagnitud < 0))
           break;
        lnx = lx2 - ly2 - lx - multiply(la,ly,bitshift);
        lny = (multiply(lx,ly,bitshift)<<1) - multiply(la,lx,bitshift) + ly;
        lx = lnx;
        ly = lny;
        }
      lmagnitud = (lx2=lsqr(lx)) + (ly2=lsqr(ly));
      color++;

      if (show_orbit)
        iplot_orbit(lx, ly, -1);

      if (lx > lx1min && lx < lx1max && labs(lb1-ly) < lclose)
        {
        if (!frothsix || lx < lxmidt)
          found_attractor = 1;
        else
          found_attractor = 2;
        }
      else if (labs(multiply(lm2,lx,bitshift)+lb2-ly) < lclose)
        {
        if (lx > lxmidr && lx < lx2max1)
           found_attractor = !frothsix ? 2 : 4;
        else if (lx > lx3min1 && lx < lxmidr)
           found_attractor = !frothsix ? 3 : 6;
        }
      else if (labs(multiply(lm3,lx,bitshift)+lb3-ly) < lclose)
        {
        if (lx > lx2min2 && lx < lxmidl)
          found_attractor = !frothsix ? 2 : 3;
        else if (lx > lxmidl && lx < lx3max2)
          found_attractor = !frothsix ? 3 : 5;
        }
      }
    }
  if (show_orbit)
    scrub_orbit();

  realcolor = color;
  if ((kbdcount -= realcolor) <= 0)
    {
    if (check_key() != 0)
      return (-1);
    kbdcount = max_kbdcount;
    }

  /* inside - Here's where non-palette based images would be nice.  Instead, */
  /* we'll use blocks of (colors-1)/3 or (colors-1)/6 and use special froth  */
  /* color maps in attempt to replicate the images of James Alexander.       */

  if (found_attractor)
    {
    if (colors >= 256)
      {
      if (!froth_altcolor)
        {
        if (color > froth_shades)
          color = froth_shades;
        }
      else
        color = froth_shades * color / maxit;
      if (color == 0)
        color = 1;
      color += froth_shades * (found_attractor-1);
      }
    else if (colors >= 16)
      { /* only alternate coloring scheme available for 16 colors */
      long lshade;
      /* Trying to make a better 16 color distribution. */
      /* Since their are only a few possiblities, just handle each case. */
      /* This is a mostly guess work here. */
      lshade = ((long)color<<16)/maxit;
      if (!frothsix)
        {
        if (lshade < 2622)       /* 0.04 */
          color = 1;
        else if (lshade < 10486) /* 0.16 */
          color = 2;
        else if (lshade < 23593) /* 0.36 */
          color = 3;
        else if (lshade < 41943) /* 0.64 */
          color = 4;
        else
          color = 5;
        color += 5 * (found_attractor-1);
        }
      else
        {
        if (lshade < 10486)      /* 0.16 */
          color = 1;
        else
          color = 2;
        color += 2 * (found_attractor-1);
        }
      }
    else /* use a color corresponding to the attractor */
      color = found_attractor;
    oldcolor = color;
    }
  else if (color >= maxit)
    color = oldcolor; /* inside, but didn't get sucked in by attractor. */
  else /* outside */
    color = 0; /* all outside points are color 0 */

  (*plot)(col, row, color);

  return color;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: calcfroth.c,v $
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
 * Revision 0.101  1994/11/28  20:21:53  darryl
 * Fix bug where some non-standard fractal types were not
 * interrupted when escape key was pressed. Move more macros
 * and definitions to header files.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
