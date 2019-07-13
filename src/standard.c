
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: standard.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static double min_orbit;
static int min_index;

/* per pixel 1/2/b/g, called with row & col set */
int StandardFractal()
{
  double tantable[16];
  int hooper = 0;
  double close;
  long lclose;
  int cyclelen = -1;
  int savedcolor = 0;
  int caught_a_cycle;
  int savedand, savedincr;
  _LCMPLX lsaved;
  int i, attracted;
  _LCMPLX lat;
  _CMPLX  at;
  _CMPLX deriv;
  int dem_color = 0;
  _CMPLX dem_new;
  close = .01;
  lclose = close*fudge;

  if(inside == STARTRAIL)
    {
    int i;
    for(i = 0; i < 16; i++)
      tantable[i] = 0.0;
    }
  else if(inside == EPSCROSS)
    {
    close = .01;
    lclose = close*fudge;
    }

  if (periodicitycheck == 0 || inside == ZMAG || inside == STARTRAIL)
    oldcolor = 32767;     /* don't check periodicity at all */
  else if (inside == PERIOD)    /* for display-periodicity */
    oldcolor = maxit*4/5;    /* don't check until nearly done */
  else if (reset_periodicity)
    oldcolor = 250;        /* don't check periodicity 1st 250 iterations */

  /* really fractal specific, but we'll leave it here */
  if (!integerfractal)
    {
    if (useinitorbit == 1)
      saved = initorbit;
    else
      {
      saved.x = 0;
      saved.y = 0;
      }

    init.y = dy0[row] + dy1[col];

    if (distest)
      {
      rqlim = rqlim_save;          /* start with regular bailout */
      if (distest != 1 || colors == 2) /* not doing regular outside colors */
        {
        if (rqlim < DEM_BAILOUT)      /* so go straight for dem bailout */
          rqlim = DEM_BAILOUT;
        }

      deriv.x = 1;
      deriv.y = 0;
      magnitude = 0;
      dem_color = -1;
      }
    }
  else
    {
    if (useinitorbit == 1)
      lsaved = linitorbit;
    else
      {
      lsaved.x = 0;
      lsaved.y = 0;
      }
    linit.y = ly0[row] + ly1[col];
    }

  orbit_ptr = 0;
  color = 0;

  if(fractype==JULIAFP || fractype==JULIA)
    color = -1;

  caught_a_cycle = 0;

  if (inside == PERIOD)
    savedand = 16;        /* begin checking every 16th cycle */
  else
    savedand = 1;        /* begin checking every other cycle */

  savedincr = 1;        /* start checking the very first time */

  if (inside <= BOF60 && inside >= BOF61)
    {
    magnitude = lmagnitud = 0;
    min_orbit = 100000.0;
    }

  overflow = 0;        /* reset integer math overflow flag */

  curfractalspecific->per_pixel(); /* initialize the calculations */

  attracted = FALSE;

  while (++color < maxit)
    {
    if(showdot>0)
      (*plot) (col, row, showdot&(colors-1));

    /* calculation of one orbit goes here */
    /* input in "old" -- output in "new" */

    if (distest)
      {
      double ftemp;

      /* Distance estimator for points near Mandelbrot set */
      /* Original code by Phil Wilson, hacked around by PB */
      /* Algorithms from Peitgen & Saupe, Science of Fractal Images, p.198 */

      ftemp     = 2 * (old.x * deriv.x - old.y * deriv.y) + 1;

      deriv.y = 2 * (old.y * deriv.x + old.x * deriv.y);
      deriv.x = ftemp;

      if (sqr(deriv.x)+sqr(deriv.y) > dem_toobig)
        break;

      /*
       * if above exit taken, the later test vs dem_delta will place this
       *  point on the boundary, because mag(old)<bailout just now
       */

      if (curfractalspecific->orbitcalc())
        {
        if (dem_color < 0)          /* note "regular" color for later */
          {
          dem_color = color;
          dem_new = new;
          }

        if (rqlim >= DEM_BAILOUT  /* exit if past real bailout */
            || magnitude >= (rqlim = DEM_BAILOUT) /* reset to real bailout */
            || magnitude == 0)     /* exit if type doesn't "floatbailout" */
          {
          break;
          }
        old = new;              /* carry on till past real bailout */
        }
      }
    else /* the usual case */
      {
      if (curfractalspecific->orbitcalc() && inside != STARTRAIL)
        break;
      }

    if (show_orbit)
      {
      if (!integerfractal)
        plot_orbit(new.x, new.y, -1);
      else
        iplot_orbit(lnew.x, lnew.y, -1);
      }

    if(inside == STARTRAIL)
      {
      if(0 < color && color < 16)
        {
        if (integerfractal)
          {
          new.x = lnew.x;
          new.x /= fudge;
          new.y = lnew.y;
          new.y /= fudge;
          }
        tantable[color-1] = new.y/(new.x+.000001);
        }
      }
    else if(inside == EPSCROSS)
      {
      hooper = 0;
      if(integerfractal)
        {
        if(labs(lnew.x) < lclose)
          {
          hooper = 1; /* close to y axis */
          goto plot_inside;
          }
        else if(labs(lnew.y) < lclose)
          {
          hooper = 2; /* close to x axis */
          goto plot_inside;
          }
        }
      else
        {
        if(fabs(new.x) < close)
          {
          hooper = 1; /* close to y axis */
          goto plot_inside;
          }
        else if(fabs(new.y) < close)
          {
          hooper = 2; /* close to x axis */
          goto plot_inside;
          }
        }
      }
    else if (inside <= BOF60 && inside >= BOF61)
      {
      if (integerfractal)
        {
        if (lmagnitud == 0)
          lmagnitud = lsqr(lnew.x) + lsqr(lnew.y);
        magnitude = lmagnitud;
        magnitude = magnitude / fudge;
        }
      else
        if (magnitude == 0.0)
          magnitude = sqr(new.x) + sqr(new.y);
      if (magnitude < min_orbit)
        {
        min_orbit = magnitude;
        min_index = color + 1;
        }
      }

    if (attractors > 0)    /* finite attractor in the list   */
      {             /* NOTE: Integer code is UNTESTED */
      if (integerfractal)
        {
        for (i = 0; i < attractors; i++)
          {
          lat.x = lnew.x - lattr[i].x;
          lat.x = lsqr(lat.x);
          if (lat.x < l_at_rad)
            {
            lat.y = lnew.y - lattr[i].y;
            lat.y = lsqr(lat.y);
            if (lat.y < l_at_rad)
              {
              if ((lat.x + lat.y) < l_at_rad)
                {
                attracted = TRUE;
                if (finattract<0) color = (color%attrperiod[i])+1;
                  break;
                }
              }
            }
          }
        }
      else
        {
        for (i = 0; i < attractors; i++)
          {
          at.x = new.x - attr[i].x;
          at.x = sqr(at.x);
          if (at.x < f_at_rad)
            {
            at.y = new.y - attr[i].y;
            at.y = sqr(at.y);
            if ( at.y < f_at_rad)
              {
              if ((at.x + at.y) < f_at_rad)
                {
                attracted = TRUE;
                if (finattract<0) color = (color%attrperiod[i])+1;
                  break;
                }
              }
            }
          }
        }
      if (attracted)
        break;        /* AHA! Eaten by an attractor */
      }

    if (color > oldcolor)    /* check periodicity */
      {
      if ((color & savedand) == 0)         /* time to save a new value */
        {
        savedcolor = color;
        if (!integerfractal)
          saved = new;  /* floating pt fractals */
        else
          lsaved = lnew;/* integer fractals */
        if (--savedincr == 0)    /* time to lengthen the periodicity? */
          {
          savedand = (savedand << 1) + 1;         /* longer periodicity */
          savedincr = 4;/* restart counter */
          }
        }
      else             /* check against an old save */
        {
        if (!integerfractal)     /* floating-pt periodicity chk */
          {
          if (fabs(saved.x - new.x) < closenuff)
            if (fabs(saved.y - new.y) < closenuff)
              {
              caught_a_cycle = 1;
              cyclelen = color-savedcolor;
              color = maxit - 1;
              }
          }
        else         /* integer periodicity check */
          {
          if (labs(lsaved.x - lnew.x) < lclosenuff)
            if (labs(lsaved.y - lnew.y) < lclosenuff)
              {
              caught_a_cycle = 1;
              cyclelen = color-savedcolor;
              color = maxit - 1;
              }
          }
        }
      }
    }

  if (show_orbit)
    scrub_orbit();

  realcolor = color;        /* save this before we start adjusting it */

  if (color >= maxit)
    oldcolor = 0;        /* check periodicity immediately next time */
  else
    {
    oldcolor = color + 10;    /* check when past this + 10 next time */
    if (color == 0)
      color = 1;        /* needed to make same as calcmand */
    }

  if (potflag)
    {
    if (integerfractal)    /* adjust integer fractals */
      {
      new.x = ((double)lnew.x) / fudge;
      new.y = ((double)lnew.y) / fudge;
      }
    magnitude = sqr(new.x) + sqr(new.y);
    color = potential(magnitude, color);
    if (LogTable)
      color = LogTable[min(color, maxit)];
    goto plot_pixel;        /* skip any other adjustments */
    }

  if (color >= maxit)        /* an "inside" point */
    goto plot_inside;     /* distest, decomp, biomorph don't apply */

  if (outside < -1)  /* these options by Richard Hughes modified by TW */
    {
    if (integerfractal)
      {
      new.x = ((double)lnew.x) / fudge;
      new.y = ((double)lnew.y) / fudge;
      }
    /* Add 7 to overcome negative values on the MANDEL    */
    if (outside == REAL)         /* "real" */
      color += new.x + 7;
    else if (outside == IMAG)         /* "imag" */
      color += new.y + 7;
    else if (outside == MULT  && new.y)  /* "mult" */
      color *= (new.x/new.y);
    else if (outside == SUM)         /* "sum" */
      color += new.x + new.y;

    /* eliminate negative colors & wrap arounds */
      if (color < 0 || color > maxit)
    color = 0;
    }

  if (distest)
    {
    double dist,temp;
    dist = sqr(new.x) + sqr(new.y);
    temp = log(dist);
    dist = dist * sqr(temp) / ( sqr(deriv.x) + sqr(deriv.y) );

    if (dist < dem_delta)    /* point is on the edge */
      {
      if (distest > 0)
        goto plot_inside;    /* show it as an inside point */
      color = 0 - distest;    /* show boundary as specified color */
      goto plot_pixel;    /* no further adjustments apply */
      }

    if (colors == 2)
      {
      color = !inside;    /* the only useful distest 2 color use */
      goto plot_pixel;    /* no further adjustments apply */
      }

    if (distest > 1)        /* pick color based on distance */
      {
      color = sqrt(dist / dem_width + 1);
      color &= INT_MAX;     /* oops - color can be negative */
      goto plot_pixel;    /* no further adjustments apply */
      }

    color = dem_color;    /* use pixel's "regular" color */
    new = dem_new;
    }

  if (decomp[0] > 0)
    decomposition();
  else if (biomorph != -1)
    {
    if (integerfractal)
      {
      if (labs(lnew.x) < llimit2 || labs(lnew.y) < llimit2)
        color = biomorph;
      }
    else
      if (fabs(new.x) < rqlim2 || fabs(new.y) < rqlim2)
        color = biomorph;
    }

  if (outside >= 0 && attracted == FALSE) /* merge escape-time stripes */
    color = outside;
  else if (LogTable)
    color = LogTable[min(color, maxit)];
  goto plot_pixel;

plot_inside: /* we're "inside" */

  if (periodicitycheck < 0 && caught_a_cycle)
    color = 7;           /* show periodicity */
  else if (inside >= 0)
    color = inside;           /* set to specified color, ignore logpal */
  else
    {
    if(inside == STARTRAIL)
      {
      int i;
      double diff;
      color = 0;
      for(i=1;i<16;i++)
        {
        diff = tantable[0] - tantable[i];
        if(fabs(diff) < .05)
          {
          color = i;
            break;
          }
        }
      }
    else if(inside== PERIOD) {
      if (cyclelen>0) {
        color = cyclelen;
      } else {
      color = maxit;
      }
    }
  else if(inside == EPSCROSS)
    {
    if(hooper==1)
      color = GREENCOLOR;
    else if(hooper==2)
      color = BROWNCOLOR;
    else
      color = maxit;
    if (show_orbit)
      scrub_orbit();
    }
  else if (inside == BOF60)
    color = sqrt(min_orbit) * 75;
  else if (inside == BOF61)
    color = min_index;
  else if (inside == ZMAG)
    {
    if (integerfractal)
      {
      /*
      new.x = ((double)lnew.x) / fudge;
      new.y = ((double)lnew.y) / fudge;
      */
      color = (((double)lsqr(lnew.x))/fudge +
              ((double)lsqr(lnew.y))/fudge) * (maxit>>1) + 1;
      }
    else
      color = (sqr(new.x) + sqr(new.y)) * (maxit>>1) + 1;
      }
    else /* inside == -1 */
      color = maxit;
    if (LogTable)
      color = LogTable[min(color, maxit)];
    }

plot_pixel:

  if (color >= colors) /* don't use color 0 unless from inside/outside */
    if (colors < 16)
      color &= andcolor;
    else
      color = ((color - 1) % andcolor) + 1;    /* skip color zero */

  if(debugflag != 470)
    if(color <= 0 && stdcalcmode == 'b' )   /* fix BTM bug */
       color = 1;

  (*plot) (col, row, color);

  if ((kbdcount -= realcolor) <= 0)
    {
    if (check_key() != 0)
      return (-1);
    kbdcount = max_kbdcount;
    }

  return (color);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: standard.c,v $
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
