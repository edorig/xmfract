
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: misc_menu.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void info_about_current_image_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void flip_image_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void create_starfield_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/*ARGSUSED*/
void create_misc_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget misc_button;
  Widget misc_menu_toc;
  Widget misc_menu_pane;
  Widget flip_btn[3];
  Widget flip_submenu_toc;
  Widget flip_submenu_pane;
  XmString str[4];
  XmString accel[3];
  XmString flip[3];

  flip[0] = XmStringCreateLocalized(" Horizontal (X Axis)");
  flip[1] = XmStringCreateLocalized(" Vertical   (Y Axis)");
  flip[2] = XmStringCreateLocalized(" Both   (X + Y Axis)");

  /* create a menu pane as a child of the menu bar */

  n = 0;
  misc_menu_pane = XmCreatePulldownMenu (w, "misc_menu_pane", args, n);

  /* get the tear-off control */

  if((misc_menu_toc = XmGetTearOffControl
    (misc_menu_pane)) != NULL)
    {
    XtVaSetValues(misc_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Misc' cascade button */

  n = 0;
  str[0] = XmStringCreateLocalized("Misc");
  XtSetArg(args[n], XmNsubMenuId, misc_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str[0]); n++;

  misc_button = XmCreateCascadeButton
    (w, "misc_button", args, n);

  XtManageChild(misc_button);

  XmStringFree(str[0]);

  /*
   * create pushbuttons and/or cascade pushbuttons
   * for the 'Misc' menu
   */

  n = 0;
  str[0] = XmStringCreateLocalized("Info about image");
  accel[0] = XmStringCreateLocalized("[ Tab ]");
  XtSetArg(args[n], XmNlabelString, str[0]); n++;
  XtSetArg(args[n], XmNacceleratorText, accel[0]); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>Tab:"); n++;

  misc_menu_btn[0] = XmCreatePushButton
    (misc_menu_pane, "misc_menu_btn_0", args, n);

  XtAddCallback(misc_menu_btn[0], XmNactivateCallback,
    info_about_current_image_cb, (XtPointer) NULL);

  n = 0;
  str[1] = XmStringCreateLocalized("Resize window");
  accel[1] = XmStringCreateLocalized("[  Z  ]");
  XtSetArg(args[n], XmNlabelString, str[1]); n++;
  XtSetArg(args[n], XmNacceleratorText, accel[1]); n++;
  XtSetArg(args[n], XmNaccelerator, "Shift<Key>z:"); n++;

  misc_menu_btn[1] = XmCreatePushButton
    (misc_menu_pane, "misc_menu_btn_1", args, n);

  XtAddCallback(misc_menu_btn[1], XmNactivateCallback,
    window_size_cb, (XtPointer) NULL);

  n = 0;
  str[2] = XmStringCreateLocalized("Create starfield");
  accel[2] = XmStringCreateLocalized("[  a  ]");
  XtSetArg(args[n], XmNlabelString, str[2]); n++;
  XtSetArg(args[n], XmNacceleratorText, accel[2]); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>a:"); n++;

  misc_menu_btn[2] = XmCreatePushButton
    (misc_menu_pane, "misc_menu_btn_2", args, n);

  XtAddCallback(misc_menu_btn[2], XmNactivateCallback,
    create_starfield_cb, (XtPointer) NULL);

  n = 0;
  misc_menu_btn[3] = XmCreateSeparator
    (misc_menu_pane, "misc_menu_btn_3", args, n);

  n = 0;
  flip_submenu_pane = XmCreatePulldownMenu(misc_button,
    "flip_submenu_pane", args, n);

  /* get the tear-off control */

  if((flip_submenu_toc = XmGetTearOffControl
    (flip_submenu_pane)) != NULL)
    {
    XtVaSetValues(flip_submenu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  n = 0;
  str[3] = XmStringCreateLocalized("Flip image");
  XtSetArg(args[n], XmNlabelString, str[3]); n++;
  XtSetArg(args[n], XmNsubMenuId, flip_submenu_pane); n++;

  misc_menu_btn[4] = XmCreateCascadeButton
    (misc_menu_pane, "misc_menu_btn_4", args, n);

  n = 0;
  XtSetArg(args[n], XmNlabelString, flip[0]); n++;
  flip_btn[0] = XmCreatePushButton
    (flip_submenu_pane, "flip_btn_0", args, n);

  XtAddCallback(flip_btn[0], XmNactivateCallback,
    flip_image_cb, (XtPointer) "0");

  n = 0;
  XtSetArg(args[n], XmNlabelString, flip[1]); n++;
  flip_btn[1] = XmCreatePushButton
    (flip_submenu_pane, "flip_btn_1", args, n);

  XtAddCallback(flip_btn[1], XmNactivateCallback,
    flip_image_cb, (XtPointer) "1");

  n = 0;
  XtSetArg(args[n], XmNlabelString, flip[2]); n++;
  flip_btn[2] = XmCreatePushButton
    (flip_submenu_pane, "flip_btn_2", args, n);

  XtAddCallback(flip_btn[2], XmNactivateCallback,
    flip_image_cb, (XtPointer) "2");

  XtManageChildren(misc_menu_btn, 5);
  XtManageChildren(flip_btn, 3);

  XmStringFree(str[0]);
  XmStringFree(str[1]);
  XmStringFree(str[2]);
  XmStringFree(str[3]);
  XmStringFree(accel[0]);
  XmStringFree(accel[1]);
  XmStringFree(accel[2]);
  XmStringFree(flip[0]);
  XmStringFree(flip[1]);
  XmStringFree(flip[2]);
}

/*ARGSUSED*/
void continue_calculation_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  create_image_cb(main_window, NULL, NULL);
}

/*ARGSUSED*/
void info_about_current_image_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  create_current_image_info_dialog(main_window, NULL, NULL);
}

/*ARGSUSED*/
void flip_image_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int axis = atoi((char *)client_data);
  flip_image(axis);
}

/*ARGSUSED*/
void create_starfield_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  get_starfield_params(main_window);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: misc_menu.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.8  1995/04/21  18:31:23  darryl
 * Update client_data handling.
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
 * Revision 1.6  1995/04/11  15:37:45  darryl
 * Remove hard-coded tearOffModel resources to allow the
 * resource file to over-ride the fallback resource.
 *
 * Revision 1.5  1995/03/20  21:30:38  darryl
 * Add an entry point for editing the current fractal type
 * parameters without having to traverse the type selection
 * process. Invoked with a menu item or the 'z' key.
 * The window resize accelerator (previously 'z') is now 'Z'.
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
 * Revision 0.301  1995/02/28  18:49:20  darryl
 * Modifications for Linux.
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
 * Revision 0.101  1994/11/27  01:07:47  darryl
 * Fix bugs where resume was not being handled correctly.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
