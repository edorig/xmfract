
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: julibrot.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int zline(long x, long y);
static int zlinefp(double x, double y);

/* static variables */
static _CMPLX jbcfp;
static double brratiofp;
static double djxfp;
static double djyfp;
static double dmxfp;
static double dmyfp;
static double inch_per_xdotfp;
static double inch_per_ydotfp;
static double initzfp;
static double jxfp;
static double jyfp;
static double mxfp;
static double myfp;
static double x_per_inchfp;
static double y_per_inchfp;
static double xoffsetfp;
static double yoffsetfp;
static double xpixelfp;
static double ypixelfp;
static int plotted;
static int savedac;
static int zpixel;
static long brratio;
static long dmx;
static long dmy;
static long inch_per_xdot;
static long inch_per_ydot;
static long jbdepth;
static long lninitz;
static long x_per_inch;
static long y_per_inch;
struct Perspective *Per;
struct Perspective LeftEye;
struct Perspective RightEye;
struct Perspectivefp *Perfp;
struct Perspectivefp LeftEyefp;
struct Perspectivefp RightEyefp;

int JulibrotSetup(void)
{
  struct fractalspecificstuff *oldcurfractalspecific;
  int oldfractype;
  int r;
  int oldhelpmode;
  char *mapname;

  r=0;
  oldhelpmode = helpmode;
  helpmode = HT_JULIBROT;
  xoffsetfp = (xxmax + xxmin) / 2;     /* Calculate average */
  yoffsetfp = (yymax + yymin) / 2;     /* Calculate average */
  dmxfp = (mxmaxfp - mxminfp) / zdots;
  dmyfp = (mymaxfp - myminfp) / zdots;
  floatparm = &jbcfp;

  x_per_inchfp = (xxmin - xxmax) / widthfp;
  y_per_inchfp = (yymax - yymin) / heightfp;
  inch_per_xdotfp = widthfp / xdots;
  inch_per_ydotfp = heightfp / ydots;

  initzfp = originfp - (depthfp / 2);
  if(juli3Dmode == 0)
    RightEyefp.x = 0.0;
  else
    RightEyefp.x = eyesfp / 2;
  LeftEyefp.x = -RightEyefp.x;
  LeftEyefp.y = RightEyefp.y = 0;
  LeftEyefp.zx = RightEyefp.zx = distfp;
  LeftEyefp.zy = RightEyefp.zy = distfp;
  bbase = 128;
  oldcurfractalspecific = curfractalspecific;
  oldfractype = fractype;
  fractype = neworbittype;
  curfractalspecific = &fractalspecific[fractype];
  lastorbittype = neworbittype;
  curfractalspecific = oldcurfractalspecific;
  fractype = oldfractype;
  helpmode = oldhelpmode;
  if (juli3Dmode == 3)
    {
    savedac = 0;
    mapname = Glasses1Map;
    }
  else
    mapname = GreyFile;
  if(savedac != 1)
    {
    if (ValidateLuts(mapname) != 0)
      return (0);
    spindac(0, 0);               /* load it, but don't spin */
    if(savedac == 2)
      savedac = 1;
    }
  return (r >= 0);
}

int jb_per_pixel(void)
{
  jx = multiply(Per->x - xpixel, lninitz, 16);
  jx = divide(jx, dist, 16) - xpixel;
  jx = multiply(jx << (bitshift - 16), x_per_inch, bitshift);
  jx += xoffset;
  djx = divide(jbdepth, dist, 16);
  djx = multiply(djx, Per->x - xpixel, 16) << (bitshift - 16);
  djx = multiply(djx, x_per_inch, bitshift) / zdots;

  jy = multiply(Per->y - ypixel, lninitz, 16);
  jy = divide(jy, dist, 16) - ypixel;
  jy = multiply(jy << (bitshift - 16), y_per_inch, bitshift);
  jy += yoffset;
  djy = divide(jbdepth, dist, 16);
  djy = multiply(djy, Per->y - ypixel, 16) << (bitshift - 16);
  djy = multiply(djy, y_per_inch, bitshift) / zdots;

  return (1);
}

int jbfp_per_pixel(void)
{
  jxfp = ((Perfp->x - xpixelfp) * initzfp / distfp - xpixelfp) * x_per_inchfp;
  jxfp += xoffsetfp;
  djxfp = (depthfp / distfp) * (Perfp->x - xpixelfp) * x_per_inchfp / zdots;

  jyfp = ((Perfp->y - ypixelfp) * initzfp / distfp - ypixelfp) * y_per_inchfp;
  jyfp += yoffsetfp;
  djyfp = depthfp / distfp * (Perfp->y - ypixelfp) * y_per_inchfp / zdots;

  return (1);
}

static int zline(long x, long y)
{
  int n;

  xpixel = x;
  ypixel = y;
  mx = mxmin;
  my = mymin;
  switch(juli3Dmode)
    {
    case 0:
    case 1:
      Per = &LeftEye;
      break;
    case 2:
      Per = &RightEye;
      break;
    case 3:
      if ((row + col) & 1)
         Per = &LeftEye;
      else
         Per = &RightEye;
    break;
    }
  jb_per_pixel();
  for (zpixel = 0; zpixel < zdots; zpixel++)
    {
    lold.x = jx;
    lold.y = jy;
    if (check_key() != 0)
      return (-1);
    ltempsqrx = multiply(lold.x, lold.x, bitshift);
    ltempsqry = multiply(lold.y, lold.y, bitshift);
    for (n = 0; n < maxit; n++)
      if (fractalspecific[neworbittype].orbitcalc())
        break;
    if (n == maxit)
      {
      if (juli3Dmode==3)
        {
        color = (int) (128l * zpixel / zdots);
        if ((row + col) & 1)
          {
          (*plot) (col, row, 127 - color);
          }
        else
          {
          color = (int) (multiply((long) color << 16, brratio, 16) >> 16);
          if (color < 1)
            color = 1;
          if (color > 127)
            color = 127;
          (*plot) (col, row, 127 + bbase - color);
          }
        }
      else
        {
        color = (int) (254l * zpixel / zdots);
        (*plot) (col, row, color + 1);
        }
      plotted = 1;
      break;
      }
    mx += dmx;
    my += dmy;
    jx += djx;
    jy += djy;
    }
  return (0);
}

static int zlinefp(double x, double y)
{
  int n;
  xpixelfp = x;
  ypixelfp = y;
  mxfp = mxminfp;
  myfp = myminfp;
  switch(juli3Dmode)
    {
    case 0:
    case 1:
      Perfp = &LeftEyefp;
      break;
    case 2:
      Perfp = &RightEyefp;
      break;
    case 3:
      if ((row + col) & 1)
        Perfp = &LeftEyefp;
      else
        Perfp = &RightEyefp;
    break;
    }
  jbfp_per_pixel();
  for (zpixel = 0; zpixel < zdots; zpixel++)
    {
    old.x = jxfp;
    old.y = jyfp;
    jbcfp.x = mxfp;
    jbcfp.y = myfp;
    qc = param[0];
    qci = param[1];
    qcj = param[2];
    qck = param[3];
    if (check_key() != 0)
      return (-1);
    tempsqrx = sqr(old.x);
    tempsqry = sqr(old.y);

    for (n = 0; n < maxit; n++)
      if (fractalspecific[neworbittype].orbitcalc())
        break;
    if (n == maxit)
      {
      if (juli3Dmode == 3)
        {
        color = (int) (128l * zpixel / zdots);
        if ((row + col) & 1)
          (*plot) (col, row, 127 - color);
        else
          {
          color = color * brratiofp;
          if (color < 1)
            color = 1;
          if (color > 127)
            color = 127;
          (*plot) (col, row, 127 + bbase - color);
          }
        }
      else
        {
        color = (int) (254l * zpixel / zdots);
        (*plot) (col, row, color + 1);
        }
      plotted = 1;
      break;
      }
    mxfp += dmxfp;
    myfp += dmyfp;
    jxfp += djxfp;
    jyfp += djyfp;
    }
  return (0);
}

int Std4dFractal(void)
{
  long x, y;
  int xdot, ydot;
  c_exp = param[2];
  if(neworbittype == LJULIAZPOWER)
    {
    if(c_exp < 1)
      c_exp = 1;
    if(param[3] == 0.0 && debugflag != 6000 && (double)c_exp == param[2])
      fractalspecific[neworbittype].orbitcalc = longZpowerFractal;
    else
      fractalspecific[neworbittype].orbitcalc = longCmplxZpowerFractal;
     }

  for (y = 0, ydot = (ydots >> 1) - 1; ydot >= 0; ydot--, y -= inch_per_ydot)
    {
    plotted = 0;
    x = -(width >> 1);
    for (xdot = 0; xdot < xdots; xdot++, x += inch_per_xdot)
      {
      col = xdot;
      row = ydot;
      if (zline(x, y) < 0)
        return (-1);
      col = xdots - col - 1;
      row = ydots - row - 1;
      if (zline(-x, -y) < 0)
        return (-1);
      }
    if (plotted == 0)
      {
      if (y == 0)
        plotted = -1;  /* no points first pass; don't give up */
      else
        break;
      }
    }
  return (0);
}

int Std4dfpFractal(void)
{
  double x, y;
  int xdot, ydot;
  c_exp = param[2];

  if(neworbittype == FPJULIAZPOWER)
    {
    if(param[3] == 0.0 && debugflag != 6000 && (double)c_exp == param[2])
      fractalspecific[neworbittype].orbitcalc = floatZpowerFractal;
    else
      fractalspecific[neworbittype].orbitcalc = floatCmplxZpowerFractal;
    get_julia_attractor (param[0], param[1]);  /* another attractor? */
    }

  for (y = 0, ydot = (ydots >> 1) - 1; ydot >= 0; ydot--, y -= inch_per_ydotfp)
    {
    plotted = 0;
    x = -widthfp / 2;
    for (xdot = 0; xdot < xdots; xdot++, x += inch_per_xdotfp)
      {
      col = xdot;
      row = ydot;
      if (zlinefp(x, y) < 0)
        return (-1);
      col = xdots - col - 1;
      row = ydots - row - 1;
      if (zlinefp(-x, -y) < 0)
        return (-1);
      }

    if (plotted == 0)
      {
      if (y == 0)
        plotted = -1;  /* no points first pass; don't give up */
      else
        break;
      }
    }
  return (0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: julibrot.c,v $
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
