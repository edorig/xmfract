
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: plot_orbit.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void plotdorbit(double dx, double dy, int color);

void iplot_orbit(ix, iy, color)
long ix, iy;
int color;
{
  plotdorbit((double)ix/fudge-xxmin,(double)iy/fudge-yymax,color);
}

void plot_orbit(real,imag,color)
double real,imag;
int color;
{
  plotdorbit(real-xxmin,imag-yymax,color);
}

static void plotdorbit(double dx, double dy, int color)
{
  int i, j, c;
  int save_sxoffs,save_syoffs;

  if (orbit_ptr >= 1500)
    return;

  i = dy * plotmx1 - dx * plotmx2; i += sxoffs;

  if (i < 0 || i >= sxdots)
    return;

  j = dx * plotmy1 - dy * plotmy2; j += syoffs;

  if (j < 0 || j >= sydots)
    return;

  save_sxoffs = sxoffs;
  save_syoffs = syoffs;
  sxoffs = syoffs = 0;

  /* save orbit value */
  if(color == -1)
    {
    *(save_orbit + orbit_ptr++) = i;
    *(save_orbit + orbit_ptr++) = j;
    *(save_orbit + orbit_ptr++) = c = getcolor(i,j);
    putcolor(i,j,c^orbit_color);
    }
  else
    putcolor(i,j,color);

  sxoffs = save_sxoffs;
  syoffs = save_syoffs;

  if(orbit_delay > 0)
    {
    int i;
    for(i = 0; i < orbit_delay * 10; i++);
    }
}

void scrub_orbit()
{
  int i,j,c;
  int save_sxoffs,save_syoffs;

  save_sxoffs = sxoffs;
  save_syoffs = syoffs;
  sxoffs = syoffs = 0;

  while(orbit_ptr > 0)
    {
    c = *(save_orbit + --orbit_ptr);
    j = *(save_orbit + --orbit_ptr);
    i = *(save_orbit + --orbit_ptr);
    putcolor(i,j,c);
    }

  sxoffs = save_sxoffs;
  syoffs = save_syoffs;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: plot_orbit.c,v $
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
