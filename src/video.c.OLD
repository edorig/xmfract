
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: video.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void init_rubber_band(void);

/* static variables */
static int xdepth;

int
getcolor(xdot,ydot)
int xdot,ydot;
{
  int x1,y1;
  x1 = xdot+sxoffs;
  y1 = ydot+syoffs;

  if (x1<0 || y1<0 || x1>=sxdots || y1>=sydots)
    return 0;

  return dotread(x1,y1);
}

void
putcolor(xdot,ydot,color)
int xdot,ydot,color;
{
  dotwrite(xdot+sxoffs,ydot+syoffs,color&andcolor);
}

void writevideo(int x, int y, int color)
{
  static int ctr = 0;

  if (xlastcolor != color)
    {
    XSetForeground(display, gc, pixtab[color]);
    xlastcolor = color;
    }

  XPutPixel(image,x,y,pixtab[color]);

  if(refresh_rate == 100)
    XDrawPoint(display,XtWindow(canvas),gc,x,y);
  else
    {
    if(++ctr > refresh_timeout)
      {
      XPutImage(display, XtWindow(canvas), gc,
        image, 0, 0, 0, 0, sxdots, sydots);
      ctr = 0;
      }
    }
}

void clear_window()
{
  int i;

  if (pixtab[0] != 0)
    {
    if (colors == 2)
      {
      for (i=0;i<image->bytes_per_line;i++)
        image->data[i] = 0xff;
      }
    else
      {
      for (i=0;i<image->bytes_per_line;i++)
        image->data[i] = pixtab[0];
      }

    for (i=1;i<image->height;i++)
      memcpy (image->data + i * image->bytes_per_line,
              image->data, image->bytes_per_line);
    }
  else
    memset(image->data, 0, image->bytes_per_line*image->height);

  zoomoff = 1;
  init_rubber_band();
  xlastcolor = -1;
  ok_to_cycle = 0;

  XSetForeground(display, gc, BlackPixelOfScreen(screen));
  XFillRectangle(display, XtWindow(canvas), gc, 0, 0, winwidth, winheight);

  XSync(display, False);
}

static void init_rubber_band()
{
  rubber_band_data.start_x = 0;
  rubber_band_data.start_y = 0;
  rubber_band_data.last_x = 0;
  rubber_band_data.last_y = 0;
  rubber_band_data.tl_x = 0;
  rubber_band_data.tl_y = 0;
  rubber_band_data.br_x = 0;
  rubber_band_data.br_y = 0;
  rubber_band_data.width = 0;
  rubber_band_data.height = 0;
  rubber_band_data.moving = 0;
  rubber_band_data.panning = 0;
  rubber_band_data.stretching = 0;
  rubber_band_data.boxisactive = 0;
  update_zoom_menu();
}

void
put_line(row, startcol, stopcol, pixels)
int row, startcol, stopcol;
unsigned char *pixels;
{
  if (startcol+sxoffs>=sxdots||row+syoffs>sydots)
    return;

  writevideoline(row+syoffs,startcol+sxoffs,stopcol+sxoffs,pixels);
}

void
writevideoline(y,x,lastx,pixels)
int x,y,lastx;
unsigned char *pixels;
{
  int width;
  int i;
  unsigned char *pixline;

  if (x == lastx)
    {
    writevideo(x,y,pixels[0]);
    return;
    }

  width = lastx-x+1;

  if (usepixtab)
    {
    for (i = 0; i < width; i++)
      pixbuf[i] = pixtab[pixels[i]];

    pixline = pixbuf;
    }
  else
    pixline = pixels;

  for (i = 0; i < width; i++)
    XPutPixel(image, x + i, y, pixline[i]);

  if(refresh_rate == 100)
    XPutImage(display, XtWindow(canvas), gc, image, x, y, x, y, width, 1);
}

int readvideo(int x, int y)
{
  return ipixtab[XGetPixel(image, x, y)];
}

void
readvideoline(y, x, lastx, pixels)
int x,y,lastx;
unsigned char *pixels;
{
  int i,width;

  width = lastx - x + 1;

  for (i = 0; i < width; i++)
    pixels[i] = readvideo(x+i,y);
}

void
spindac(dir, inc)
int dir, inc;
{
  int i,top;
  unsigned char tmp[3];
  unsigned char *dacbot;
  int len;

  if (colors<16)
    return;

  if (dir != 0 && rotate_lo < colors && rotate_lo < rotate_hi)
    {
    top = rotate_hi > colors - 1 ? colors - 1 : rotate_hi;
    dacbot = dacbox[0] + 3 * rotate_lo;
    len = (top-rotate_lo) * 3 * sizeof(unsigned char);
    if (dir > 0)
      {
      for (i = 0; i < inc; i++)
        {
        memcpy(tmp, dacbot, 3 * sizeof(unsigned char));
        memcpy(dacbot, dacbot + 3 * sizeof(unsigned char), len);
        memcpy(dacbot + len, tmp, 3 * sizeof(unsigned char));
        }
      }
    else
      {
      for (i = 0; i < inc; i++)
        {
        memcpy(tmp, dacbot + len, 3 * sizeof(unsigned char));
        memcpy(dacbot + 3 * sizeof(unsigned char), dacbot, len);
        memcpy(dacbot, tmp, 3 * sizeof(unsigned char));
        }
      }
    }

  writevideopalette();
}

int writevideopalette()
{
  int i;

  if (!gotrealdac)
    return -1;

  for(i = 0; i < 256; i++)
    {
    cols[i].pixel = pixtab[i];
    cols[i].flags = DoRed | DoGreen | DoBlue;
    cols[i].red = dacbox[i][0] * 256;
    cols[i].green = dacbox[i][1] * 256;
    cols[i].blue = dacbox[i][2] * 256;
    }

  XStoreColors(display, colormap, cols, colors);

  XFlush(display);

  return 0;
}

void initdacbox()
{
  int i;
  if (colors == 2)
    {
    dacbox[0][0] = dacbox[0][1] = dacbox[0][2] = 0;
    dacbox[1][0] = dacbox[1][1] = dacbox[1][2] = 255;
    }
  else
    {
    for (i = 0; i < 256; i++)
      {
      dacbox[i][0] = (((i >> 5) * 8 + 7) * 4);
      dacbox[i][1] = (((((i + 16) & 28) >> 2)* 8 + 7) * 4);
      dacbox[i][2] = (((((i + 2) & 3)) * 16 + 15) * 4);
      }
    dacbox[0][0] = dacbox[0][1] = dacbox[0][2] = 0;
    dacbox[1][0] = dacbox[1][1] = dacbox[1][2] = 255;
    dacbox[2][0] = 188; dacbox[2][1] = dacbox[2][2] = 255;
    }

  writevideopalette();
}

int cmapstuff()
{
  int ncells,i,powr;

  for (i = 0; i < colors; i++)
    {
    pixtab[i] = i;
    ipixtab[i] = 999;
    }

  if (!gotrealdac)
    {
    colors = 2;
    }
  else if (sharecolor)
    {
    colors = 256;
    gotrealdac = 0;
    }
  else if (privatecolor)
    {
    colors = 256;
    colormap = XCreateColormap(display,window,visual,AllocAll);
    XSetWindowColormap(display,window,colormap);
    usepixtab = 1;
    }
  else
    {
    colormap = DefaultColormap(display, screen_number);
    xdepth = DefaultDepth(display, screen_number);
    for (powr=xdepth;powr>=1;powr--)
      {
      ncells = 1 << powr;
      if (ncells > colors)
        continue;
      if (XAllocColorCells
          (display,colormap,False,NULL,0,pixtab, (unsigned int)ncells))
        {
        colors = ncells;
        usepixtab = 1;
        break;
        }
      }
    }

  if(!usepixtab && !sharecolor)
    {
    static int tries = 0;
    fprintf(stderr, "Couldn't allocate any colors\n");
    if(tries == 0)
      {
      fprintf(stderr, "Attempting to share colors\n");
      gotrealdac = 1;
      sharecolor = 1;
      cmapstuff();
      }
    else
      {
      sharecolor = 0;
      gotrealdac = 0;
      cmapstuff();
      }
    }

  for (i = 0;i < colors; i++)
    ipixtab[pixtab[i]] = i;

  if (!gotrealdac && colors == 2 && BlackPixelOfScreen(screen) != 0)
    {
    pixtab[0] = ipixtab[0] = 1;
    pixtab[1] = ipixtab[1] = 0;
    usepixtab = 1;
    }

  if(colors < 16)
    {
    if(BlackPixelOfScreen(screen) == 0)
      {
      dacbox[0][0] = dacbox[0][1] = dacbox[0][2] = 0;
      dacbox[1][0] = dacbox[1][1] = dacbox[1][2] = 255;
      pixtab[0] = ipixtab[0] = 0;
      pixtab[1] = ipixtab[1] = 1;
      usepixtab = 1;
      }
    else
      {
      dacbox[0][0] = dacbox[0][1] = dacbox[0][2] = 255;
      dacbox[1][0] = dacbox[1][1] = dacbox[1][2] = 0;
      pixtab[0] = ipixtab[0] = 1;
      pixtab[1] = ipixtab[1] = 0;
      usepixtab = 1;
      }
    }

  return colors;
}

void draw_line (int X1, int Y1, int X2, int Y2, int color)
{
  /* uses Bresenham algorithm to draw a line */
  int dX, dY;     /* vector components */
  int row, col,
  final,          /* final row or column number */
  G,              /* used to test for new row or column */
  inc1,           /* G increment when row or column doesn't change */
  inc2;           /* G increment when row or column changes */
  char pos_slope;

  dX = X2 - X1;   /* find vector components */
  dY = Y2 - Y1;

  pos_slope = (dX > 0);       /* is slope positive? */
  if (dY < 0)
    pos_slope = !pos_slope;
  if (abs (dX) > abs (dY))    /* shallow line case */
    {
    if (dX > 0) /* determine start point and last column */
      {
      col = X1;
      row = Y1;
      final = X2;
      }
    else
      {
      col = X2;
      row = Y2;
      final = X1;
      }
    inc1 = 2 * abs (dY); /* determine increments and initial G */
    G = inc1 - abs (dX);
    inc2 = 2 * (abs (dY) - abs (dX));
    if (pos_slope)
      while (col <= final) /* step through columns checking for new row */
        {
        (*plot) (col, row, color);
        col++;
        if (G >= 0) /* it's time to change rows */
          {
          row++; /* positive slope so increment through the rows */
          G += inc2;
          }
        else /* stay at the same row */
          G += inc1;
        }
    else
      while (col <= final) /* step through columns checking for new row */
        {
        (*plot) (col, row, color);
        col++;
        if (G > 0) /* it's time to change rows */
          {
          row--; /* negative slope so decrement through the rows */
          G += inc2;
          }
        else /* stay at the same row */
          G += inc1;
        }
    }   /* if |dX| > |dY| */
  else /* steep line case */
    {
    if (dY > 0) /* determine start point and last row */
      {
      col = X1;
      row = Y1;
      final = Y2;
      }
    else
      {
      col = X2;
      row = Y2;
      final = Y1;
      }
    inc1 = 2 * abs (dX); /* determine increments and initial G */
    G = inc1 - abs (dY);
    inc2 = 2 * (abs (dX) - abs (dY));
    if (pos_slope)
      while (row <= final) /* step through rows checking for new column */
        {
        (*plot) (col, row, color);
        row++;
        if (G >= 0)                 /* it's time to change columns */
          {
          col++;  /* positive slope so increment through the columns */
          G += inc2;
          }
        else                    /* stay at the same column */
          G += inc1;
        }
    else
      while (row <= final) /* step thru rows checking for new column */
        {
        (*plot) (col, row, color);
        row++;
        if (G > 0)                  /* it's time to change columns */
          {
          col--;  /* negative slope so decrement through the columns */
          G += inc2;
          }
        else                    /* stay at the same column */
          G += inc1;
        }
    }
}

void
find_special_colors()
{
  int maxb = 0;
  int minb = 99999;
  int maxgun, mingun;
  int brt;
  int i;

  color_dark = 0;
  color_bright = 15;

  if(colors == 2)
    {
    color_bright = 1;
    return;
    }

  if(!gotrealdac)
    return;

  for (i = 0; i < colors; i++)
    {
    brt = (int) dacbox[i][0] + (int) dacbox[i][1] + (int) dacbox[i][2];

    if (brt > maxb)
      {
      maxb = brt;
      color_bright = i;
      }

    if (brt < minb)
      {
      minb = brt;
      color_dark = i;
      }
    }
}

/*ARGSUSED*/
int ShadowVideo(on)
int on;
{
  return 0;
}

int out_line(pixels, linelen)
unsigned char *pixels;
unsigned linelen;
{
  if (rowcount+syoffs>=sydots)
    return 0;

  writevideoline(rowcount+syoffs,sxoffs,(int)(linelen)+sxoffs-1,pixels);

  rowcount++;
  return 0;
}

void
get_line(row, startcol, stopcol, pixels)
int row,startcol,stopcol;
unsigned char *pixels;
{
  if (startcol+sxoffs>=sxdots||row+syoffs>=sydots)
    return;

  lineread(row+syoffs,startcol+sxoffs,stopcol+sxoffs,pixels);
}

/*
 * This routine copies the current screen to by flipping x-axis, y-axis,
 * or both. Refuses to work if calculation in progress or if fractal
 * non-resumable. Clears zoombox if any. Resets corners so resulting fractal
 * is still valid.
 */

void flip_image(int flip_direction)
{
  int i, j, ixhalf, iyhalf, tempdot;
  char tmpmsg[256];

  /* fractal must be rotate-able and be finished */

  if(((curfractalspecific->flags&NOROTATE) > 0) ||
    (calc_status != IMG_PRMS_CHGD && calc_status != IMG_COMPLETED))
    {
    buzzer(0);
    if((curfractalspecific->flags&NOROTATE) > 0)
      sprintf((char *) &tmpmsg, "Cannot flip this image.");
    else
      sprintf((char *) &tmpmsg, "Cannot flip a non-completed image.");
    inform(main_window, (char *) &tmpmsg, NULL);
    return;
    }

  clear_zoombox(); /* clear, don't copy, the zoombox */

  ixhalf = xdots / 2;
  iyhalf = ydots / 2;

  switch(flip_direction)
    {
    case 0: /* reverse X-axis */
      for (i = 0; i < ixhalf; i++)
        {
        if(keypressed())
          break;
        for (j = 0; j < ydots; j++)
          {
          tempdot=getcolor(i,j);
          putcolor(i, j, getcolor(xdots-1-i,j));
          putcolor(xdots-1-i, j, tempdot);
          }
        }
      sxmin = xxmax + xxmin - xx3rd;
      symax = yymax + yymin - yy3rd;
      sxmax = xx3rd;
      symin = yy3rd;
      sx3rd = xxmax;
      sy3rd = yymin;
      reset_zoom_corners();
    break;

    case 1: /* reverse Y-aXis */
      for (j = 0; j < iyhalf; j++)
        {
        if(keypressed())
          break;
        for (i = 0; i < xdots; i++)
          {
          tempdot=getcolor(i,j);
          putcolor(i, j, getcolor(i,ydots-1-j));
          putcolor(i,ydots-1-j, tempdot);
          }
        }
      sxmin = xx3rd;
      symax = yy3rd;
      sxmax = xxmax + xxmin - xx3rd;
      symin = yymax + yymin - yy3rd;
      sx3rd = xxmin;
      sy3rd = yymax;
    break;

    case 2: /* reverse X and Y aXis */
      for (i = 0; i < ixhalf; i++)
        {
        if(keypressed())
          break;
        for (j = 0; j < ydots; j++)
          {
          tempdot=getcolor(i,j);
          putcolor(i, j, getcolor(xdots-1-i,ydots-1-j));
          putcolor(xdots-1-i, ydots-1-j, tempdot);
          }
        }
      sxmin = xxmax;
      symax = yymin;
      sxmax = xxmin;
      symin = yymax;
      sx3rd = xxmax + xxmin - xx3rd;
      sy3rd = yymax + yymin - yy3rd;
    break;
    }
  reset_zoom_corners();
  buzzer(0);
}

/*ARGSUSED*/
void load_color_map(
Widget w,
char *filename)
{
  if(rubber_band_data.boxisactive == 1)
    {
    change_xor_gc(2);
    XDrawRectangle(display, XtWindow(canvas), xgc,
      rubber_band_data.tl_x, rubber_band_data.tl_y,
      rubber_band_data.br_x - rubber_band_data.tl_x,
      rubber_band_data.br_y - rubber_band_data.tl_y);

    if(rubber_band_data.panning == 0)
      {
      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.h_tl_x, rubber_band_data.h_tl_y,
        rubber_band_data.h_tr_x, rubber_band_data.h_tr_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.h_bl_x, rubber_band_data.h_bl_y,
        rubber_band_data.h_br_x, rubber_band_data.h_br_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.v_tl_x, rubber_band_data.v_tl_y,
        rubber_band_data.v_bl_x, rubber_band_data.v_bl_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.v_tr_x, rubber_band_data.v_tr_y,
        rubber_band_data.v_br_x, rubber_band_data.v_br_y);
      }
    }

  SetColorPaletteName(filename);

  if(mapdacbox)
    {
    memcpy((char *)dacbox,mapdacbox,768);
    spindac(0,0);
    change_xor_gc(1);
    colorstate = 0;
    }
  sprintf((char *) &MAP_name, filename);
  if(paledit.popped_up)
    redraw_palette_colorbox(main_window, NULL, NULL);
  mapset = 1;

  if(rubber_band_data.boxisactive == 1)
    {
    change_xor_gc(2);
    XDrawRectangle(display, XtWindow(canvas), xgc,
      rubber_band_data.tl_x, rubber_band_data.tl_y,
      rubber_band_data.br_x - rubber_band_data.tl_x,
      rubber_band_data.br_y - rubber_band_data.tl_y);

    if(rubber_band_data.panning == 0)
      {
      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.h_tl_x, rubber_band_data.h_tl_y,
        rubber_band_data.h_tr_x, rubber_band_data.h_tr_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.h_bl_x, rubber_band_data.h_bl_y,
        rubber_band_data.h_br_x, rubber_band_data.h_br_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.v_tl_x, rubber_band_data.v_tl_y,
        rubber_band_data.v_bl_x, rubber_band_data.v_bl_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.v_tr_x, rubber_band_data.v_tr_y,
        rubber_band_data.v_br_x, rubber_band_data.v_br_y);
      }
    }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: video.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.18  1995/06/08  23:59:38  darryl
 * Allow zoom and pan of a flipped image. Don't change the
 * calculation status of a flipped image.
 *
 * Revision 1.17  1995/05/26  19:05:18  darryl
 * Fix default color cycling options.
 *
 * Revision 1.16  1995/05/17  02:46:21  darryl
 * Fix a problem where loading a new color map could
 * leave a ghost zoom box on the screen.
 *
 * Revision 1.15  1995/05/10  22:14:04  darryl
 * Delete unused code and variables, fix problems
 * reported in beta test, enhance the file selection
 * dialogs and restore menu.
 *
 * Revision 1.14  1995/05/04  17:55:21  darryl
 * Fix bug where color editor was not aware of color map
 * changes if the map was loaded from a file or randomized.
 *
 * Revision 1.13  1995/05/03  21:29:57  darryl
 * Lint and optimization cleanup.
 *
 * Revision 1.12  1995/04/30  12:57:19  darryl
 * Color editor enhancements, place hooks for freestyle option.
 *
 * Revision 1.11  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.10  1995/04/26  20:31:53  darryl
 * Fix bugs reported in beta testing:
 *   -- solid-guessing would occasionally leave portions
 *      of the image in coarse pixels.
 *   -- Re-align more void pointers before type-casting.
 *   -- Change zoombox color on loading a map or gif file.
 *   -- Add panned images to restore history.
 *
 * Revision 1.9  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.8  1995/04/18  15:59:59  darryl
 * Fix problems and incorporate enhancements from beta test.
 *  -- not able to zoom after loading a saved image.
 *  -- string parsing would fail because the tags were incorrect.
 *  -- CLK_TCK is determined from sysconf() if it is available.
 *  -- Julia sets were not calculated correctly after image save.
 *  -- some void pointers were not cast before promotion.
 *  -- Info About Image display says it was working on pass 2 of 1.
 *  -- an incomplete image could not be saved.
 *  -- a saved image was not drawn if the refresh rate was < 100.
 *  -- added Refresh Rate menu item and command line support.
 *  -- added Parameter Forwarding menu item and command line support.
 *  -- added fallback resources.
 *  -- enhanced parameter handling.
 *  -- implemented the Fractint zoombox 'panning' capability.
 *  -- enhanced the LogMap and LogTable handling.
 *  -- various typograhic error fixes.
 *
 * Revision 1.7  1995/04/10  22:12:57  darryl
 * Add rate=nnn and menu support for setting the screen refresh
 * rate during image calculation. Fix a bug where the Julia
 * inverse mode was broken after an image save. Add fallback
 * resources that take effect if a resource file is not found.
 *
 * Revision 1.6  1995/04/03  15:49:33  darryl
 * Add Brighter and Darker buttons to the Range menu.
 * Fix a problem where the color editor cancel button
 * would reset the colors to the default color map
 * after a gif image was loaded. Fix a potential
 * problem where the skipval dialog was not being
 * destroyed when the color editor was popped down.
 *
 * Revision 1.5  1995/03/28  18:11:40  darryl
 * Fix trig function handling. Enhance center-mag button
 * management. Aesthetic changes in the parameter dialog.
 * Add frames around groups of scale widgets. Fix a bogus
 * message about color cell allocation if sharecolor.
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
 * Revision 0.303  1995/02/28  18:49:20  darryl
 * Modifications for Linux.
 *
 * Revision 0.302  1995/01/30  20:55:14  darryl
 * Ensure that the background of the screen is black
 * when color map is shared with other clients.
 *
 * Revision 0.301  1994/12/30  23:54:54  darryl
 * Delete unused global variables.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.105  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.104  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.103  1994/11/29  02:08:16  darryl
 * Reverse the operation of the zoomoff variable so
 * that a non-zero value means that zooming is disabled.
 *
 * Revision 0.102  1994/11/27  01:07:47  darryl
 * Fix bugs where resume was not being handled correctly.
 *
 * Revision 0.101  1994/11/19  02:53:05  darryl
 * Move plotblock() to solidguess.c and declare it static.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
