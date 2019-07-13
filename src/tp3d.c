
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: tp3d.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static unsigned long ReadTPlusBankedPixel(int col, int row);

/* static variables */
static int NewTPFractal;

static int ShadowColors;

static void (*PutTC)(int col, int row, unsigned long color) =
   WriteTPlusBankedPixel;

unsigned long (*GetTC)(int col, int row) =
   ReadTPlusBankedPixel;

/*ARGSUSED*/
static unsigned long ReadTPlusBankedPixel(int col, int row)
{
  return 0L;
}

char *TCError[] =
{
  "Could not match this video resolution to a TARGA+ mode",
  "True Color disk video isn't implement yet, maybe later . . .",
};

void TranspPerPixel(int MathType, union FracArg *xy, union FracArg *zt)
{
  if(NewTPFractal)
    {
    dx = (xxmax - xxmin) / xdots;
    dy = (yymin - yymax) / ydots;  /* reverse direction of y-axis */
    dz = (zzmax - zzmin) / xdots;
    dt = (ttmax - ttmin) / NumFrames;

    NewTPFractal = 0;
    }

  switch(MathType)
    {
    /* For calculation purposes, 'x' and 'z' are swapped */
    case D_MATH:
    default:
      xy->d.x = xxmin + (dx * col);
      xy->d.y = yymax + (dy * row);
      zt->d.x = zzmin + (dz * tpdepth);
      zt->d.y = ttmin + (dt * tptime);
    break;
    }
}

void ShadowPutColor(unsigned xdot, unsigned ydot, unsigned color)
{
  ShadowVideo(0);
  if(ShadowColors)
    putcolor(xdot >> AntiAliasing, ydot >> AntiAliasing, color);
  else
    {
    unsigned r, g, b;
    unsigned long lcolor;

    r = (dacbox[color][0]);
    g = (dacbox[color][1]);
    b = (dacbox[color][2]);
    lcolor = ((long)r << 16) + (g << 8) + b;
    PutTC(xdot >> AntiAliasing, ydot >> AntiAliasing, lcolor);
    }
  ShadowVideo(1);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: tp3d.c,v $
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
