
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: create_image.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*ARGSUSED*/
void create_image_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i, ret, done;
  double ftemp;
  char msg[256];
  XEvent event;
  Boolean stat1;
  Boolean stat2;
  Boolean stat3;

  if(fractype == NOFRACTAL)
    {
    sprintf((char *) &msg, "No fractal type selected.");
    inform(main_window, (char *) &msg, NULL);
    return;
    }

  if(calc_status == IMG_ACTV_CALC || calc_status == IMG_INTR_RESM)
    {
    if(curfractalspecific->flags&NORESUME)
      {
      sprintf((char *) &msg, "This fractal type cannot be resumed. If you wish to re-calculate\nthis image using the same paramters, first clear the screen by\ninvoking the Clear Screen item from the Reset menu.");
      caution(main_window, (char *) &msg, NULL);
      return;
      }
    }

  if(calc_status == IMG_INTR_NRES)
    {
    sprintf((char *) &msg, "This fractal type cannot be resumed. If you wish to re-calculate\nthis image using the same paramters, first clear the screen by\ninvoking the Clear Screen item from the Reset menu.");
    caution(main_window, (char *) &msg, NULL);
    return;
    }

  if(calc_status == IMG_COMPLETED && !rubber_band_data.boxisactive)
    {
    sprintf((char *) &msg, "Parameters for this image have not changed.\n\nRe-calculate anyway?");
    ret = confirm(main_window, (char *) &msg,
          OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON);
    if(ret != 1)
      return;
    }

  if((xdots < window_presets.min_width  ||
      xdots > window_presets.max_width  ||
      ydots < window_presets.min_height ||
      ydots > window_presets.max_height) &&
      (!viewwindow))
    {
    sprintf((char *) &msg, "The canvas is too small or too large to calculate an image.\n\nPlease resize the main window.");
    ret = confirm(main_window, (char *) &msg,
          OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
    if(ret != 1)
      return;
    else
      window_size_cb(main_window, NULL, NULL);
    return;
    }

  if(display3d)
    {
    showfile = 0;
    if(mapset)
      mapset = 0;
    display_file();
    display3d = 0;
    return;
    }

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(rubber_band_data.boxisactive)
    {
    zrotate = 0;
    zskew = 0;
    zbx = (MIN(rubber_band_data.tl_x,rubber_band_data.br_x)-sxoffs)/dx_size;
    zby = (MIN(rubber_band_data.tl_y,rubber_band_data.br_y)-syoffs)/dy_size;
    zwidth = ABS(rubber_band_data.br_x-rubber_band_data.tl_x)/dx_size;
    zdepth = zwidth;
    rubber_band_data.boxisactive = 0;
    drawbox();
    init_pan_or_recalc(0);
    update_zoom_menu();
    }

  ok_to_cycle = 0;

  resize_window(main_window, NULL, NULL);

  if(viewwindow)
    {
    ftemp = finalaspectratio * (double)sydots / (double)sxdots / screenaspect;
    if ((xdots = viewxdots)) /* xdots specified */
      {
      /* calc ydots? */
      if ((ydots = viewydots) == 0)
        {
        ydots = (double)xdots * ftemp + 0.5;
        }
      }
    else
      {
      if (finalaspectratio <= screenaspect)
        {
        xdots = (double)sxdots / viewreduction + 0.5;
        ydots = (double)xdots * ftemp + 0.5;
        }
      else
        {
        ydots = (double)sydots / viewreduction + 0.5;
        xdots = (double)ydots / ftemp + 0.5;
        }
      }
    if (xdots > sxdots || ydots > sydots)
      {
      static char mesg[] = {"View window too large; using full screen."};
      inform(main_window, (char *) &mesg, NULL);
      xdots = sxdots;
      ydots = sydots;
      }
    else if (xdots <= sxdots/20 || ydots <= sydots/20) /* so ssg works */
      {
      static char message[] = {"View window too small; using full screen."};
      inform(main_window, (char *) &message, NULL);
      xdots = sxdots;
      ydots = sydots;
      }
    sxoffs = (sxdots - xdots) / 2;
    syoffs = (sydots - ydots) / 3;
    if(xdots < window_presets.min_width  ||
       xdots > window_presets.max_width  ||
       ydots < window_presets.min_height ||
       ydots > window_presets.max_height)
      {
      sprintf((char *) &msg, "The canvas is too small or too large to calculate an image.\n\nPlease resize the main window or turn off the preview mode.");
      ret = confirm(main_window, (char *) &msg,
            OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
      if(ret != 1)
        return;
      else
        window_size_cb(main_window, NULL, NULL);
      return;
      }
    }
  else if(sxoffs || syoffs || viewxdots || viewydots)
    {
    sxoffs = syoffs = 0;
    viewxdots = viewydots = 0;
    sxdots = videotable[0].xdots;
    sydots = videotable[0].ydots;
    xdots = sxdots;
    ydots = sydots;
    dx_size = xdots - 1;
    dy_size = ydots - 1;
    }

  dx_size = xdots - 1;
  dy_size = ydots - 1;

  if(calc_status <= IMG_PRMS_CHGD)
    clear_window();

  calcfracinit();

  /* save 3 corners for zoom.c ref points */

  sxmin = xxmin;
  sxmax = xxmax;
  sx3rd = xx3rd;
  symin = yymin;
  symax = yymax;
  sy3rd = yy3rd;

  /* initialize the history file */

  if (history[0].fractype == -1)
    {
    histocycle = 0;
    for (i = 0; i < MAXHISTORY; i++)
      {
      int j;
      historymax = -1;
      history[i].xxmax = xxmax;
      history[i].xxmin = xxmin;
      history[i].yymax = yymax;
      history[i].yymin = yymin;
      history[i].xx3rd = xx3rd;
      history[i].yy3rd = yy3rd;

      for(j=0;j<MAXPARAMS;j++)
        {
        history[i].param[j] = param[j];
        }

      history[i].fractype = fractype;
      }
    }

  if(historyflag == 0 &&
    ((calc_status < IMG_ACTV_CALC || calc_status > IMG_INTR_NRES) ||
    (rubber_band_data.panning == 1)))
    {
    int j;

    rubber_band_data.panning = 0;

    if(++historyptr == MAXHISTORY)
      {
      historymax = MAXHISTORY;
      historyptr = 0;
      histocycle = 1;
      }
    else if(historymax == MAXHISTORY)
      {
      if(historyptr >= MAXHISTORY)
        historyptr = 0;
      }
    else
      {
      historyptr = ++historymax;
      if(historyptr >= MAXHISTORY)
        {
        historymax = MAXHISTORY;
        historyptr = 0;
        histocycle = 1;
        }
      }

    history[historyptr].xxmax = xxmax;
    history[historyptr].xxmin = xxmin;
    history[historyptr].yymax = yymax;
    history[historyptr].yymin = yymin;
    history[historyptr].xx3rd = xx3rd;
    history[historyptr].yy3rd = yy3rd;

    for(j=0;j<MAXPARAMS;j++)
      history[historyptr].param[j] = param[j];

    history[historyptr].fractype = fractype;
    }

  update_restore_menu();

  calcfract();

  XPutImage(display, XtWindow(canvas), gc, image, 0, 0, 0, 0, sxdots, sydots);

  create_xor_gc();

  /* prevent confusion for the zoom box pointer */

  done = 0;
  while(!done)
    {
    stat1 = XCheckTypedWindowEvent(display, XtWindow(canvas),
      ButtonPress, &event);
    stat2 = XCheckTypedWindowEvent(display, XtWindow(canvas),
      ButtonRelease, &event);
    stat3 = XCheckTypedWindowEvent(display, XtWindow(canvas),
      MotionNotify, &event);
    if(!stat1 && !stat2 && !stat3)
      done = 1;
    }

  historyflag = 0;
  ok_to_cycle = 1;
  display3d = 0;
  zoomoff = 0;

  if((!(curfractalspecific->flags&NOZOOM)))
    toggle_zoom_grab(ZOOM_ON);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  buzzer(0);
}

void
create_previous_image(int which)
{
  int j;

  if(which < 10)
    {
    for(j = 0; j < which; j++)
      if(--historyptr < 0)
        historyptr = MAXHISTORY;
    }
  else
    {
    for(j = 0; j < which - 10; j++)
      if(++historyptr == MAXHISTORY)
        historyptr = 0;
    }

  xxmax  = history[historyptr].xxmax;
  xxmin  = history[historyptr].xxmin;
  yymax  = history[historyptr].yymax;
  yymin  = history[historyptr].yymin;
  xx3rd  = history[historyptr].xx3rd;
  yy3rd  = history[historyptr].yy3rd;

  for(j=0;j<MAXPARAMS;j++)
    param[j] = history[historyptr].param[j];

  fractype = history[historyptr].fractype;

  curfractalspecific = &fractalspecific[fractype];

  historyflag = 1;

  if(rubber_band_data.boxisactive)
    {
    erase_current_zoombox();
    rubber_band_data.boxisactive = 0;
    update_zoom_menu();
    }

  create_image_cb(main_window, NULL, NULL);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: create_image.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.15  1995/06/02  06:09:35  darryl
 * Change default button in some dialogs from Cancel to Ok.
 *
 * Revision 1.14  1995/05/18  22:55:48  darryl
 * Update warning message concerning non-resumeable types.
 *
 * Revision 1.13  1995/05/17  01:37:31  darryl
 * Fix a bug in the history pointer calculation.
 *
 * Revision 1.12  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.11  1995/05/03  07:36:37  darryl
 * Enable zoom and pan at the end of image creation.
 *
 * Revision 1.10  1995/05/03  06:38:46  darryl
 * Re-install processing to discard pointer events that
 * happened in the canvas during image creation.
 *
 * Revision 1.9  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.8  1995/04/26  20:31:53  darryl
 * Fix bugs reported in beta testing:
 *   -- solid-guessing would occasionally leave portions
 *      of the image in coarse pixels.
 *   -- Re-align more void pointers before type-casting.
 *   -- Change zoombox color on loading a map or gif file.
 *   -- Add panned images to restore history.
 *
 * Revision 1.7  1995/04/18  15:59:59  darryl
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
 * Revision 1.6  1995/03/31  18:47:10  darryl
 * Update parameter file processing so that 3D parameters
 * are handled in the same fashion as other entries.
 *
 * Revision 1.5  1995/03/26  22:28:18  darryl
 * Add buttons to the type selection dialogs for returning
 * to a file or name selection dialog if the type is
 * formula, lsystem, or ifs. Update the help dialog so that
 * only one may be displayed at a time. Display a clock
 * cursor while an image is being generated.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1.1.1  1995/03/11  03:43:20  tom
 * change dxsize and dysize
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.302  1995/01/12  19:15:26  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.301  1995/01/10  00:40:37  darryl
 * Add comments, delete references to obsolete code.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.202  1994/12/10  01:05:03  darryl
 * More attempts to fortify the rubber-banding functions.
 *
 * Revision 0.201  1994/12/08  19:27:17  darryl
 * Ensure that the xgc contains the special colors as the
 * foreground and background when the color map changes.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.109  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.108  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.107  1994/11/29  02:08:16  darryl
 * Reverse the operation of the zoomoff variable so
 * that a non-zero value means that zooming is disabled.
 *
 * Revision 0.106  1994/11/27  01:07:47  darryl
 * Fix bugs where resume was not being handled correctly.
 *
 * Revision 0.105  1994/11/26  23:30:46  darryl
 * Fix bug where calc_status was being set incorrectly.
 *
 * Revision 0.104  1994/11/25  18:18:01  darryl
 * Move the event handlers for the rubber band functions
 * from the initialization section to the image creation
 * section so they can be installed and removed more easily.
 * Prevents event processing errors caused by delays in
 * posting events from the X server that would occasionally
 * make strange and interesting things to happen to the zoom box.
 *
 * Revision 0.103  1994/11/21  22:15:32  darryl
 * Enforce an 80 x 60 minimum window size.
 *
 * Revision 0.102  1994/11/19  02:53:05  darryl
 * Delete unnecessary Eventhandler removal.
 *
 * Revision 0.101  1994/11/18  17:09:57  darryl
 * Aesthetic changes - widget and label placement. Minor bug
 * fixes when saving parameters and drawing a zoom box.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
