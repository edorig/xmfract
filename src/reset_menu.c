
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: reset_menu.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void refresh_image_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void reset_colors_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void reset_prms_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void reset_all_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/*ARGSUSED*/
void create_reset_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget reset_button;
  Widget reset_menu_toc;
  Widget reset_menu_pane;
  Widget reset_menu_btn[5];
  XmString rset[5];
  XmString str;

  /* create a menu pane as a child of the menu bar */

  n = 0;
  reset_menu_pane = XmCreatePulldownMenu
    (w, "reset_menu_pane", args, n);

  /* get the tear-off control */

  if((reset_menu_toc = XmGetTearOffControl
    (reset_menu_pane)) != NULL)
    {
    XtVaSetValues(reset_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Reset' cascade button */

  n = 0;
  str = XmStringCreateLocalized("Reset");
  XtSetArg(args[n], XmNsubMenuId, reset_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  reset_button = XmCreateCascadeButton
    (w, "reset_button", args, n);

  XtManageChild(reset_button);

  XmStringFree(str);

  rset[0] = XmStringCreateLocalized("Refresh");
  rset[1] = XmStringCreateLocalized("Reset Color");
  rset[2] = XmStringCreateLocalized("Clear Screen");
  rset[3] = XmStringCreateLocalized("Reset Parameters");
  rset[4] = XmStringCreateLocalized("All of the above");

  n = 0;
  XtSetArg(args[n], XmNlabelString, rset[0]); n++;

  reset_menu_btn[0] = XmCreatePushButton
    (reset_menu_pane, "reset_menu_btn_0", args, n);

  XtAddCallback(reset_menu_btn[0], XmNactivateCallback,
    refresh_image_cb, (XtPointer) NULL);

  n = 0;
  XtSetArg(args[n], XmNlabelString, rset[1]); n++;

  reset_menu_btn[1] = XmCreatePushButton
    (reset_menu_pane, "reset_menu_btn_1", args, n);

  XtAddCallback(reset_menu_btn[1], XmNactivateCallback,
    reset_colors_cb, (XtPointer) NULL);

  n = 0;
  XtSetArg(args[n], XmNlabelString, rset[2]); n++;

  reset_menu_btn[2] = XmCreatePushButton
    (reset_menu_pane, "reset_menu_btn_2", args, n);

  XtAddCallback(reset_menu_btn[2], XmNactivateCallback,
    clear_image_cb, (XtPointer) NULL);

  n = 0;
  XtSetArg(args[n], XmNlabelString, rset[3]); n++;

  reset_menu_btn[3] = XmCreatePushButton
    (reset_menu_pane, "reset_menu_btn_3", args, n);

  XtAddCallback(reset_menu_btn[3], XmNactivateCallback,
    reset_prms_cb, (XtPointer) NULL);

  n = 0;
  XtSetArg(args[n], XmNlabelString, rset[4]); n++;

  reset_menu_btn[4] = XmCreatePushButton
    (reset_menu_pane, "reset_menu_btn_4", args, n);

  XtAddCallback(reset_menu_btn[4], XmNactivateCallback,
    reset_all_cb, (XtPointer) NULL);

  XtManageChildren(reset_menu_btn, 5);

  XmStringFree(rset[0]);
  XmStringFree(rset[1]);
  XmStringFree(rset[2]);
  XmStringFree(rset[3]);
  XmStringFree(rset[4]);
}

/*ARGSUSED*/
static void refresh_image_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(rubber_band_data.boxisactive == 1)
    zoom_cancel_cb(main_window, NULL, NULL);
  else
    XPutImage(display, XtWindow(canvas), gc,
      image, 0, 0, 0, 0, sxdots, sydots);
}

/*ARGSUSED*/
static void reset_colors_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(mapdacbox)
    {
    memcpy((char *)dacbox,mapdacbox,768);
    spindac(0,0);
    colorstate = 0;
    }
}

/*ARGSUSED*/
void clear_image_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  clear_window();
  calc_status = IMG_PRMS_INIT;
  update_draw_menu(NEW_IMAGE);
}

/*ARGSUSED*/
static void reset_prms_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  initvars_fractal();
  calc_status = IMG_PRMS_INIT;
  update_draw_menu(NEW_IMAGE);
}

/*ARGSUSED*/
static void reset_all_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(mapdacbox)
    {
    memcpy((char *)dacbox,mapdacbox,768);
    spindac(0,0);
    colorstate = 0;
    }
  initvars_fractal();
  clear_window();
  calc_status = IMG_PRMS_INIT;
  update_draw_menu(NEW_IMAGE);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: reset_menu.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.6  1995/04/27  22:41:05  darryl
 * Erase any active zoom box upon screen refresh.
 *
 * Revision 1.5  1995/04/11  15:37:45  darryl
 * Remove hard-coded tearOffModel resources to allow the
 * resource file to over-ride the fallback resource.
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
 * Revision 0.301  1994/12/20  20:06:02  darryl
 * Add 'Refresh' to the 'Reset' menu.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.104  1994/11/30  21:09:19  darryl
 * Set the XmNseparatorType resource for TearOffControls.
 *
 * Revision 0.103  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.102  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.101  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
