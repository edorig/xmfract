
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: plot3d.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 *  This file includes miscellaneous plot functions and logic
 *  for 3D, used by lorenz.c and line3d.c
 *  By Tim Wegner and Marc Reinig.
 */

/* static functions */
static void plot3dsuperimpose16(int x, int y, int color);
static void plot3dsuperimpose256(int x, int y, int color);
static void plotIFS3dsuperimpose256(int x, int y, int color);
static void plot3dalternate(int x, int y, int color);

/* static variables */
static int blue_local_left;
static int blue_local_right;
static int red_local_left;
static int red_local_right;
static unsigned char T_RED;

static void plot3dsuperimpose16(int x,int y,int color)
{
  int tmp;

  tmp = getcolor(x,y);

  if(whichimage == 1) /* RED */
    {
    color = PAL_RED;
    if(tmp > 0 && tmp != color)
      color = PAL_MAGENTA;
    if(red_local_left < x && x < red_local_right)
      {
      putcolor(x,y,color);
      if (Targa_Out)
        targa_color(x, y, color);
      }
    }
  else if(whichimage == 2) /* BLUE */
    if(blue_local_left < x && x < blue_local_right)
      {
      color = PAL_BLUE;
      if(tmp > 0 && tmp != color)
        color = PAL_MAGENTA;
      putcolor(x,y,color);
      if (Targa_Out)
        targa_color(x, y, color);
      }
}

static void plot3dsuperimpose256(int x,int y,int color)
{
  int tmp;
  unsigned char t_c;

  t_c = 255-color;

  if (color != 0)     /* Keeps index 0 still 0 */
    {
    color = colors - color; /*  Reverses color order */
    if (max_colors == 236)
      color = 1 + color / 21; /*  Maps colors 1-255 to 13 even ranges */
    else
      color = 1 + color / 18; /*  Maps colors 1-255 to 15 even ranges */
    }

  tmp = getcolor(x,y);
  /* map to 16 colors */
  if(whichimage == 1) /* RED */
    {
    if(red_local_left < x && x < red_local_right)
      {
      /* Overwrite prev Red don't mess w/blue */
      putcolor(x,y,color|(tmp&240));
      if (Targa_Out)
        if (!ILLUMINE)
          targa_color(x, y, color|(tmp&240));
        else
          targa_writedisk (x+sxoffs, y+syoffs, t_c, 0, 0);
      }
    }
  else if(whichimage == 2) /* BLUE */
    if(blue_local_left < x && x < blue_local_right)
      {
      /* Overwrite previous blue, don't mess with existing red */
      color = color <<4;
      putcolor(x,y,color|(tmp&15));
      if (Targa_Out)
        if (!ILLUMINE)
          targa_color(x, y, color|(tmp&15));
        else
          {
          targa_readdisk (x+sxoffs, y+syoffs, &T_RED,
            (unsigned char *)&tmp, (unsigned char *)&tmp);
          targa_writedisk (x+sxoffs, y+syoffs, T_RED, 0, t_c);
          }
      }
}

static void plotIFS3dsuperimpose256(int x,int y,int color)
{
  int tmp;
  unsigned char t_c;

  t_c = 255-color;

  if (color != 0)     /* Keeps index 0 still 0 */
    {
    /* my mind is fried - lower indices = darker colors is EASIER! */
    color = colors - color; /*  Reverses color order */
    if (max_colors == 236)
    color = 1 + color / 21; /*  Maps colors 1-255 to 13 even ranges */
    else
    color = 1 + color / 18; /*  Looks weird but maps colors 1-255 to 15
          relatively even ranges */
    }

  tmp = getcolor(x,y);
  /* map to 16 colors */
  if(whichimage == 1) /* RED */
    {
    if(red_local_left < x && x < red_local_right)
      {
      putcolor(x,y,color|tmp);
      if (Targa_Out)
        if (!ILLUMINE)
          targa_color(x, y, color|tmp);
        else
          targa_writedisk (x+sxoffs, y+syoffs, t_c, 0, 0);
      }
    }
  else if(whichimage == 2) /* BLUE */
    if(blue_local_left < x && x < blue_local_right)
      {
      color = color <<4;
      putcolor(x,y,color|tmp);
      if (Targa_Out)
        if (!ILLUMINE)
          targa_color(x, y, color|tmp);
        else
          {
          targa_readdisk (x+sxoffs, y+syoffs, &T_RED,
            (unsigned char *)&tmp, (unsigned char *)&tmp);
          targa_writedisk (x+sxoffs, y+syoffs, T_RED, 0, t_c);
          }
      }
}

/*ARGSUSED*/
static void plot3dalternate(int x,int y,int color)
{
  unsigned char t_c;

  t_c = 255-color;
  /* lorez high color red/blue 3D plot function */
  /* if which image = 1, compresses color to lower 128 colors */

  /* my mind is STILL fried - lower indices = darker colors is EASIER! */
  color = colors - color;
  if((whichimage == 1) && !((x+y)&1)) /* - lower half palette */
    {
    if(red_local_left < x && x < red_local_right)
      {
      putcolor(x,y,color>>1);
      if (Targa_Out)
        if (!ILLUMINE)
          targa_color(x, y, color>>1);
        else
          targa_writedisk (x+sxoffs, y+syoffs, t_c, 0, 0);
      }
    }
  else if((whichimage == 2) && ((x+y)&1) ) /* - upper half palette */
    {
    if(blue_local_left < x && x < blue_local_right)
      {
      putcolor(x,y,(color>>1)+(colors>>1));
      if (Targa_Out)
        if (!ILLUMINE)
          targa_color(x, y, (color>>1)+(colors>>1));
        else
          targa_writedisk (x+sxoffs, y+syoffs, T_RED, 0, t_c);
      }
    }
}

void plot_setup()
{
  int i;
  double d_red_bright = 0.0;
  double d_blue_bright = 0.0;

  /* set funny glasses plot function */
  switch(glassestype)
    {
    case 1:
      standardplot = plot3dalternate;
    break;

    case 2:
      if(colors == 256)
        if (fractype != IFS3D)
          standardplot = plot3dsuperimpose256;
        else
          standardplot = plotIFS3dsuperimpose256;
      else
        standardplot = plot3dsuperimpose16;
    break;

    default:
      standardplot = putcolor;
    break;
    }

  xshift1 = xshift = (XSHIFT * (double)xdots)/100;
  yshift1 = yshift = (YSHIFT * (double)ydots)/100;

  if(glassestype)
    {
    red_local_left  =   (red_crop_left    * (double)xdots)/100.0;
    red_local_right =   ((100 - red_crop_right) * (double)xdots)/100.0;
    blue_local_left =   (blue_crop_left   * (double)xdots)/100.0;
    blue_local_right =  ((100 - blue_crop_right) * (double)xdots)/100.0;
    d_red_bright  =   (double)red_bright/100.0;
    d_blue_bright   =   (double)blue_bright/100.0;

    switch(whichimage)
      {
      case 1:
        xshift  += (eyeseparation* (double)xdots)/200;
        xxadjust = ((xtrans+xadjust)* (double)xdots)/100;
        xshift1 -= (eyeseparation* (double)xdots)/200;
        xxadjust1 = ((xtrans-xadjust)* (double)xdots)/100;
      break;

      case 2:
        xshift  -= (eyeseparation* (double)xdots)/200;
        xxadjust = ((xtrans-xadjust)* (double)xdots)/100;
      break;
      }
    }
  else
    xxadjust = (xtrans* (double)xdots)/100;

  yyadjust = -(ytrans* (double)ydots)/100;

  if (mapset)
    {
    ValidateLuts(MAP_name); /* read the palette file */
    if(glassestype==1 || glassestype==2)
      {
      if(glassestype == 2 && colors < 256)
        {
        dacbox[PAL_RED  ][0] = 255;
        dacbox[PAL_RED  ][1] =  0;
        dacbox[PAL_RED  ][2] =  0;

        dacbox[PAL_BLUE ][0] =  0;
        dacbox[PAL_BLUE ][1] =  0;
        dacbox[PAL_BLUE ][2] = 255;

        dacbox[PAL_MAGENTA][0] = 255;
        dacbox[PAL_MAGENTA][1] =  0;
        dacbox[PAL_MAGENTA][2] = 255;
        }
      for (i=0;i<256;i++)
        {
        dacbox[i][0] = dacbox[i][0] * d_red_bright;
        dacbox[i][2] = dacbox[i][2] * d_blue_bright;
        }
      }
    spindac(0,0); /* load it, but don't spin */
  }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: plot3d.c,v $
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
