
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: rotate.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int random_range_smooth(int from, int to, int modifier);

/* static variables */
static XtIntervalId xmtimeout;

/*ARGSUSED*/
void rotate(
  XtPointer client_data,
  XtIntervalId Id)
{
  int  last, next;
  int step, fstep, istep, jstep, oldstep;
  int incr, fromred, fromblue, fromgreen, tored, toblue, togreen;
  int rotate_max,rotate_size;
  int direction;

  static int fsteps[] = {2,4,8,12,16,24,32,40,54,100}; /* (for Fkeys) */

  fromred = fromblue = fromgreen = tored = toblue = togreen = 0;

  direction = rotate_stuff.direction;

  fstep = 1;
  incr = 999;                    /* ready to randomize */
  srand((unsigned)time(NULL));   /* randomize things   */
  oldstep = step = 1;            /* single-step        */

  rotate_max = (rotate_hi < colors) ? rotate_hi : colors-1;
  rotate_size = rotate_max - rotate_lo + 1;

  last = rotate_max; /* last box that was filled    */
  next = rotate_lo;  /* next box to be filled    */

  if (direction < 0)
    {
    last = rotate_lo;
    next = rotate_max;
    }

  if (rotate_stuff.fkey > 0) /* randomizing is on */
    {
    for (istep = 0; istep < step; istep++)
      {
      jstep = next + (istep * direction);
      while (jstep < rotate_lo)  jstep += rotate_size;
      while (jstep > rotate_max) jstep -= rotate_size;
      if (++incr > fstep) /* time to randomize */
        {
        incr = 1;
        fstep = ((fsteps[rotate_stuff.fkey-1]* (rand15() >> 9)) >> 6 ) + 1;
        fromred   = dacbox[last][0];
        fromgreen = dacbox[last][1];
        fromblue  = dacbox[last][2];
        tored     = rand15() >> 6;
        togreen   = rand15() >> 6;
        toblue    = rand15() >> 6;
        }
      dacbox[jstep][0] = fromred   + (((tored   - fromred  )*incr)/fstep);
      dacbox[jstep][1] = fromgreen + (((togreen - fromgreen)*incr)/fstep);
      dacbox[jstep][2] = fromblue  + (((toblue  - fromblue )*incr)/fstep);
      }
    }

  if (step >= rotate_size) step = oldstep;

  spindac(direction, step);

  if(rotate_stuff.onestep == 0)
    xmtimeout = XtAppAddTimeOut(app, interval,
      (XtTimerCallbackProc) rotate, canvas);
}

int save_palette(char *fname)
{
  FILE *dacfile;
  int i, ret;
  char msg[128];

  dacfile = fopen(fname,"w");

  if (dacfile == (FILE *)NULL)
    {
    sprintf((char *) &msg, "Cannot write to \"%s\".\n", fname);
    ret = confirm(main_window, (char *) &msg,
      OK_BTN, XmDIALOG_OK_BUTTON);
    return -1;
    }
  else {
    for (i = 0; i < 256; i++)
      fprintf(dacfile, "%3d %3d %3d\n",
        dacbox[i][0],
        dacbox[i][1],
        dacbox[i][2]);
    memcpy(olddacbox,dacbox,768);
    colorstate = 2;
    strcpy(colorfile,fname);
    }
  fclose(dacfile);
  return 0;
}

void
cycle_speed_increase()
{
  if(rotate_stuff.running == 1)
    {
    rotate_stuff.interval -= 5;
    interval = rotate_stuff.interval;
    if((int)interval < 0)
      {
      interval = 0;
      rotate_stuff.interval = interval;
      }
    if(xmtimeout)
      XtRemoveTimeOut(xmtimeout);
    xmtimeout = XtAppAddTimeOut(app, interval,
      (XtTimerCallbackProc) rotate, &rotate_stuff);
    }
}

void
cycle_speed_decrease()
{
  if(rotate_stuff.running == 1)
    {
    rotate_stuff.interval += 5;
    interval = rotate_stuff.interval;
    if((int)interval > 5000)
      {
      interval = 5000;
      rotate_stuff.interval = interval;
      }
    if(xmtimeout)
      XtRemoveTimeOut(xmtimeout);
    xmtimeout = XtAppAddTimeOut(app, interval,
      (XtTimerCallbackProc) rotate, &rotate_stuff);
    }
}

void
cycle_step_forward()
{
  if(rotate_stuff.running == 1)
    {
    rotate_stuff.direction = 1;
    if(xmtimeout)
      XtRemoveTimeOut(xmtimeout);
    xmtimeout = XtAppAddTimeOut(app, interval,
      (XtTimerCallbackProc) rotate, &rotate_stuff);
    }
  else
    {
    rotate_stuff.direction = 1;
    rotate_stuff.onestep = 1;
    if(xmtimeout)
      XtRemoveTimeOut(xmtimeout);
    rotate(NULL, 0);
    if(paledit.popped_up)
      redraw_palette_colorbox(main_window, NULL, NULL);
    }
}

void
cycle_step_backward()
{
  if(rotate_stuff.running == 1)
    {
    rotate_stuff.direction = -1;
    if(xmtimeout)
      XtRemoveTimeOut(xmtimeout);
    xmtimeout = XtAppAddTimeOut(app, interval,
      (XtTimerCallbackProc) rotate, &rotate_stuff);
    }
  else
    {
    rotate_stuff.direction = -1;
    rotate_stuff.onestep = 1;
    if(xmtimeout)
      XtRemoveTimeOut(xmtimeout);
    rotate(NULL, 0);
    if(paledit.popped_up)
      redraw_palette_colorbox(main_window, NULL, NULL);
    }
}

void
toggle_color_cycling()
{
  char msg[256];
  if(rotate_stuff.running == 1)
    {
    rotate_stuff.running = 0;
    if(xmtimeout)
      XtRemoveTimeOut(xmtimeout);
    if(paledit.popped_up)
      redraw_palette_colorbox(main_window, NULL, NULL);
    }
  else if(rotate_stuff.running == 0)
    {
    /* strange things happen in 2x modes */
    if(colors < 16)
      {
      buzzer(2);
      sprintf((char *) &msg, "Xmfract has currently allocated %d colors.\n\nColors cannot be cycled when xmfract\nhas allocated less than 16 colors.\n", colors);
      inform(main_window, (char *) &msg, NULL);
      return;
      }

    /*
     * shared colors cannot be rotated because the cells
     * need to be allocated as read/write
     */

    if(sharecolor)
      {
      ok_to_cycle = 0;
      sprintf((char *) &msg, "Xmfract has been invoked with the \"-share\"\ncommand-line option. Colors that are shared\nare not privately allocated by this program\nand cannot be cycled.\n");
      inform(main_window, (char *) &msg, NULL);
      return;
      }
    interval = rotate_stuff.interval;
    rotate_stuff.direction = -1;
    rotate_stuff.onestep = 0;
    rotate_stuff.fkey = 0;
    rotate_stuff.running = 1;
    memcpy(olddacbox,dacbox,768);
    if(xmtimeout)
      XtRemoveTimeOut(xmtimeout);
    xmtimeout = XtAppAddTimeOut(app, interval,
      (XtTimerCallbackProc) rotate, &rotate_stuff);
    }
}

void
randomize_cycle_palette(int points, int modifier)
{
  int step;
  int stepok;
  int i, done;
  int lastindex;
  int randshift;
  int colorindex;
  int start, stop;
  unsigned char tmpval;

  start = rotate_lo;
  stop = rotate_hi;
  lastindex = rotate_lo;

  if(points == 0)
    {
    for(i = 0; i < stop + 1; i++)
      {
      tmpval = rand15() >> 7;
      dacbox[i][0] = tmpval;
      tmpval = rand15() >> 7;
      dacbox[i][1] = tmpval;
      tmpval = rand15() >> 7;
      dacbox[i][2] = tmpval;
      }

    spindac(0,0);

    if(paledit.popped_up)
      {
      redraw_palette_colorbox(main_window, NULL, NULL);
      update_undo_index();
      }
    return;
    }
  else if(points == 1)
    {
    if(modifier == 6 || modifier == 7)
      {
      random_range_smooth(start, (stop / 2), modifier);
      random_range_smooth(((stop / 2) + 1), stop, modifier);
      }
    else
      random_range_smooth(start, stop, modifier);
    }
  else if(points == 2)
    {
    random_range_smooth(start, (stop / 2), modifier);
    random_range_smooth(((stop / 2) + 1), stop, modifier);
    }
  else if(points == 3)
    {
    for(i = 0; i < (((stop - start) / 16) + 1); i++)
      random_range_smooth(start + (i * 16), start + (16 + (i * 16)), modifier);
    }
  else if(points == 4)
    {
    for(i = 0; i < (((stop - start) / 8) + 1); i++)
      random_range_smooth(start + (i * 8), start + (8 + (i * 8)), modifier);
    }
  else
    {
    done = 0;
    while(!done)
      {
      stepok = 0;
      while(!stepok)
        {
        step = rand15() >> (points + 3);
        if(step > 5 && step < (colors / 2))
          stepok = 1;
        }
      colorindex = lastindex + step;
      if((colorindex > stop) || ((stop - colorindex) < 5))
        colorindex = stop;
      random_range_smooth(lastindex, colorindex, modifier);
      lastindex = colorindex + 1;
      if(lastindex > stop)
        done = 1;
      }
    }

  if(paledit.popped_up)
    {
    redraw_palette_colorbox(main_window, NULL, NULL);
    update_undo_index();
    }
}

static int random_range_smooth(int from, int to, int modifier)
{
  int colorval;
  int cointoss;
  int starteqend;
  int smoothmode;
  int done, num, n, x;
  double rm, gm, bm;
  unsigned char tmpval;
  static int startdark = 0;
  static int startlite = 1;

  starteqend = 0;
  smoothmode = 0;

  from = from < rotate_lo ? rotate_lo : from;
  to = to > rotate_hi ? rotate_hi : to;

  num = ABS(to - from);

  if(modifier == 0)
    cointoss = 0;
  else if(modifier == 1)
    cointoss = 1;
  else if(modifier == 2)
    {
    cointoss = (startdark == 1 ? 0 : 1);
    startdark = (cointoss == 1 ? 1 : 0);
    }
  else if(modifier == 3)
    {
    cointoss = (startlite == 1 ? 0 : 1);
    startlite = (cointoss == 1 ? 1 : 0);
    }
  else if(modifier == 4)
    {
    starteqend = 1;
    cointoss = (startdark == 1 ? 0 : 1);
    startdark = (cointoss == 1 ? 1 : 0);
    }
  else if(modifier == 5)
    {
    starteqend = 1;
    cointoss = (startlite == 1 ? 0 : 1);
    startlite = (cointoss == 1 ? 1 : 0);
    }
  else if(modifier == 6)
    {
    starteqend = 1;
    smoothmode = 1;
    cointoss = (startdark == 1 ? 0 : 1);
    startlite = (cointoss == 1 ? 1 : 0);
    }
  else if(modifier == 7)
    {
    starteqend = 1;
    smoothmode = 1;
    cointoss = (startlite == 1 ? 0 : 1);
    startlite = (cointoss == 1 ? 1 : 0);
    }
  else /* pathological case */
    cointoss = 0;

  if(cointoss == 1) /* Range is light to dark */
    {
    if(starteqend == 0 || from == rotate_lo)
      {
      /* Randomize the starting color */
      done = 0;
      while(!done)
        {
        tmpval = rand15() >> 7;
        dacbox[from][0] = tmpval;
        tmpval = rand15() >> 7;
        dacbox[from][1] = tmpval;
        tmpval = rand15() >> 7;
        dacbox[from][2] = tmpval;

        colorval = ((int)(dacbox[from][0] + dacbox[from][0] + dacbox[from][0]));

        if(colorval > 700)
          done = 1;
        }
      }
    else if(starteqend == 1)
      {
      dacbox[from][0] = dacbox[from - 1][0];
      dacbox[from][1] = dacbox[from - 1][1];
      dacbox[from][2] = dacbox[from - 1][2];
      }

    /* Randomize the end color */
    if((starteqend == 1) && (smoothmode == 1) && (to >= rotate_hi))
      {
      dacbox[to][0] = dacbox[rotate_lo][0];
      dacbox[to][1] = dacbox[rotate_lo][1];
      dacbox[to][2] = dacbox[rotate_lo][2];
      }
    else
      {
      done = 0;
      while(!done)
        {
        tmpval = rand15() >> 7;
        dacbox[to][0] = tmpval;
        tmpval = rand15() >> 7;
        dacbox[to][1] = tmpval;
        tmpval = rand15() >> 7;
        dacbox[to][2] = tmpval;

        colorval = ((int)(dacbox[to][0] + dacbox[to][0] + dacbox[to][0]));

        if(colorval < 200)
          done = 1;
        }
      }
    }
  else /* Range is dark to light */
    {
    if(starteqend == 0 || from == rotate_lo)
      {
      /* Randomize the starting color */
      done = 0;
      while(!done)
        {
        tmpval = rand15() >> 7;
        dacbox[from][0] = tmpval;
        tmpval = rand15() >> 7;
        dacbox[from][1] = tmpval;
        tmpval = rand15() >> 7;
        dacbox[from][2] = tmpval;

        colorval = ((int)(dacbox[from][0] + dacbox[from][0] + dacbox[from][0]));

        if(colorval < 200)
          done = 1;
        }
      }
    else if(starteqend == 1)
      {
      dacbox[from][0] = dacbox[from - 1][0];
      dacbox[from][1] = dacbox[from - 1][1];
      dacbox[from][2] = dacbox[from - 1][2];
      }

    /* Randomize the end color */
    if((starteqend == 1) && (smoothmode == 1) && (to >= rotate_hi))
      {
      dacbox[to][0] = dacbox[rotate_lo][0];
      dacbox[to][1] = dacbox[rotate_lo][1];
      dacbox[to][2] = dacbox[rotate_lo][2];
      }
    else
      {
      done = 0;
      while(!done)
        {
        tmpval = rand15() >> 7;
        dacbox[to][0] = tmpval;
        tmpval = rand15() >> 7;
        dacbox[to][1] = tmpval;
        tmpval = rand15() >> 7;
        dacbox[to][2] = tmpval;

        colorval = ((int)(dacbox[to][0] + dacbox[to][0] + dacbox[to][0]));

        if(colorval > 700)
          done = 1;
        }
      }
    }

  /*
   * Smooth the range between the ends, using the color editor's
   * gamma value to set the contrast of the range.
   */

  rm = (double)((int)dacbox[to][0] - (int)dacbox[from][0]) / num;
  gm = (double)((int)dacbox[to][1] - (int)dacbox[from][1]) / num;
  bm = (double)((int)dacbox[to][2] - (int)dacbox[from][2]) / num;

  for(n = from, x = 0; n < to; n++, x++)
    {
    dacbox[n][0] = (dacbox[from][0] == dacbox[to][0]) ?
      dacbox[from][0] : (unsigned char)(dacbox[from][0] +
      (unsigned char)(pow(x/(double)(num - 1),
      paledit.gamma_val) * num * rm));

    dacbox[n][1] = (dacbox[from][1] == dacbox[to][1]) ?
       dacbox[from][1] : (unsigned char)(dacbox[from][1] +
       (unsigned char)(pow(x/(double)(num - 1),
       paledit.gamma_val) * num * gm));

    dacbox[n][2] = (dacbox[from][2] == dacbox[to][2]) ?
       dacbox[from][2] : (unsigned char)(dacbox[from][2] +
       (unsigned char)(pow(x/(double)(num - 1),
       paledit.gamma_val) * num * bm));

    cols[n].red = dacbox[n][0] * 256;
    cols[n].blue = dacbox[n][2] * 256;
    cols[n].green = dacbox[n][1] * 256;

    XStoreColor(display, colormap, &cols[n]);
    }
  spindac(0, 0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: rotate.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.17  1995/06/15  15:36:25  darryl
 * Fix potential problem where palette randomization could
 * produce a palette consisting of all one color,
 * in the largest random smoothing mode.
 *
 * Revision 1.16  1995/06/14  01:30:07  darryl
 * Add color editor undo-last-command and undo-last-redo and
 * command-line and ini file support for max undo commands.
 *
 * Revision 1.15  1995/06/08  23:32:35  darryl
 * Fix one-segment smooth-mode palette randomization.
 *
 * Revision 1.14  1995/05/31  20:35:17  darryl
 * Allow palette randomization to affect only colors
 * within the cycle range.
 *
 * Revision 1.13  1995/05/24  03:45:08  darryl
 * Add the Smooth modes to the palette randomization routines.
 *
 * Revision 1.12  1995/05/23  18:50:55  darryl
 * Fix color editor and palette randomization problems
 * reported in beta testing.
 *
 * Revision 1.11  1995/05/22  06:57:06  darryl
 * Make mandelcloud and julia_inverse resumeable.
 *
 * Revision 1.10  1995/05/21  09:54:04  darryl
 * Enhance palette randomization routines.
 *
 * Revision 1.9  1995/05/20  03:17:09  darryl
 * Color editor and view windows enhancements.
 *
 * Revision 1.8  1995/05/08  19:44:43  darryl
 * Fix bugs reported in beta test.
 *
 * Revision 1.7  1995/05/04  17:55:21  darryl
 * Fix bug where color editor was not aware of color map
 * changes if the map was loaded from a file or randomized.
 *
 * Revision 1.6  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.5  1995/04/18  15:59:59  darryl
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
 * Revision 0.301  1995/01/30  19:54:51  darryl
 * Enhance information messages concerning shared color.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.102  1994/11/29  06:15:07  darryl
 * More bulletproofing the zoom box, and default rotate_lo to
 * two so the lake is not cycled, and fix a bug where the
 * scale multiple on the upper and lower cycle range scales
 * was not set to one.
 *
 * Revision 0.101  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
