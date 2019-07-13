
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: options_menu.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void basic_opts_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void extended_opts_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void view_window_opts_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_menu_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/*ARGSUSED*/
void create_options_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget option_button;
  Widget option_menu_toc;
  Widget option_menu_pane;
  Widget option_menu_btn[4];
  XmString str[4];
  XmString accel[4];

  /* create a menu pane as a child of the menu bar */

  n = 0;
  option_menu_pane = XmCreatePulldownMenu (w, "option_menu_pane", args, n);

  /* get the tear-off control */

  if((option_menu_toc = XmGetTearOffControl
    (option_menu_pane)) != NULL)
    {
    XtVaSetValues(option_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Options' cascade button */

  n = 0;
  str[0] = XmStringCreateLocalized("Options");
  XtSetArg(args[n], XmNsubMenuId, option_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str[0]); n++;

  option_button = XmCreateCascadeButton(w, "option_button", args, n);

  XtManageChild(option_button);

  XmStringFree(str[0]);

  /*
   * create pushbuttons and/or cascade pushbuttons for the 'Options' menu
   *
   * Basic options...
   * Extended options...
   * View window options...
   * Fractal 3D parameters...
   *
   */

  n = 0;
  str[0] = XmStringCreateLocalized("Basic options...");
  accel[0] = XmStringCreateLocalized("[ x ]");
  XtSetArg(args[n], XmNlabelString, str[0]); n++;
  XtSetArg(args[n], XmNacceleratorText, accel[0]); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>x:"); n++;

  option_menu_btn[0] = XmCreatePushButton
    (option_menu_pane, "option_menu_btn_0", args, n);

  XtAddCallback(option_menu_btn[0], XmNactivateCallback,
    basic_opts_cb, (XtPointer) NULL);

  n = 0;
  str[1] = XmStringCreateLocalized("Extended options...");
  accel[1] = XmStringCreateLocalized("[ y ]");
  XtSetArg(args[n], XmNlabelString, str[1]); n++;
  XtSetArg(args[n], XmNacceleratorText, accel[1]); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>y:"); n++;

  option_menu_btn[1] = XmCreatePushButton
    (option_menu_pane, "option_menu_btn_1", args, n);

  XtAddCallback(option_menu_btn[1], XmNactivateCallback,
    extended_opts_cb, (XtPointer) NULL);

  n = 0;
  str[2] = XmStringCreateLocalized("View window options...");
  accel[2] = XmStringCreateLocalized("[ v ]");
  XtSetArg(args[n], XmNlabelString, str[2]); n++;
  XtSetArg(args[n], XmNacceleratorText, accel[2]); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>v:"); n++;

  option_menu_btn[2] = XmCreatePushButton
    (option_menu_pane, "option_menu_btn_2", args, n);

  XtAddCallback(option_menu_btn[2], XmNactivateCallback,
    view_window_opts_cb, (XtPointer) NULL);

  n = 0;
  str[3] = XmStringCreateLocalized("Fractal 3D parameters...");
  accel[3] = XmStringCreateLocalized("[ i ]");
  XtSetArg(args[n], XmNlabelString, str[3]); n++;
  XtSetArg(args[n], XmNacceleratorText, accel[3]); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>i:"); n++;

  option_menu_btn[3] = XmCreatePushButton
    (option_menu_pane, "option_menu_btn_3", args, n);

  XtAddCallback(option_menu_btn[3], XmNactivateCallback,
    three_d_menu_cb, (XtPointer) NULL);

  XtManageChildren(option_menu_btn, 4);

  XmStringFree(str[0]);
  XmStringFree(str[1]);
  XmStringFree(str[2]);
  XmStringFree(str[3]);

  XmStringFree(accel[0]);
  XmStringFree(accel[1]);
  XmStringFree(accel[2]);
  XmStringFree(accel[3]);

}

/*ARGSUSED*/
static void basic_opts_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  create_basic_options_dialog(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void extended_opts_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  create_extended_options_dialog(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void view_window_opts_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  create_window_options_dialog(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void three_d_menu_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  create_three_d_options_dialog(main_window, NULL, NULL);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: options_menu.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
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
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.102  1994/11/30  21:09:19  darryl
 * Set the XmNseparatorType resource for TearOffControls.
 *
 * Revision 0.101  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
