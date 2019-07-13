
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: fractal_types.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void select_fractal_type_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void edit_current_parms_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_parameter_forwarding
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget fractal_types_submenu_btn1;
static Widget fractal_types_submenu_btn2;

void
create_fractal_types_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget fractal_types_button;
  Widget fractal_types_menu_toc;
  Widget fractal_types_menu_pane;
  Widget fractal_types_submenu_toc;
  Widget fractal_types_submenu_pane;
  Widget fractal_types_menu_btn1;
  Widget fractal_types_menu_btn2;
  Widget fractal_types_menu_btn3;
  XmString str;
  XmString accel;

  /* create a menu pane as a child of the menu bar */

  n = 0;
  fractal_types_menu_pane = XmCreatePulldownMenu
    (w, "fractal_types_menu_pane", args, n);

  /* get the tear-off control */

  if((fractal_types_menu_toc = XmGetTearOffControl
    (fractal_types_menu_pane)) != NULL)
    {
    XtVaSetValues(fractal_types_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Parameters' cascade button */

  n = 0;
  str = XmStringCreateLocalized("Parameters");
  XtSetArg(args[n], XmNsubMenuId, fractal_types_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  fractal_types_button = XmCreateCascadeButton
    (w, "fractal_types_button", args, n);

  XtManageChild(fractal_types_button);

  XmStringFree(str);

  /*
   * create pushbuttons and/or cascade pushbuttons for the 'New Image' menu
   *
   * Select fractal type
   * Current parameters
   * Parameter forwarding ->
   *                        On
   *                        Off
   *                        (radio buttons)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Select fractal type");
  accel = XmStringCreateLocalized("[ t ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>t:"); n++;

  fractal_types_menu_btn1 = XmCreatePushButton
    (fractal_types_menu_pane, "fractal_types_menu_btn1", args, n);

  XtAddCallback(fractal_types_menu_btn1, XmNactivateCallback,
    select_fractal_type_cb, (XtPointer) NULL);

  XtManageChild(fractal_types_menu_btn1);

  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Current parameters");
  accel = XmStringCreateLocalized("[ z ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>z:"); n++;

  fractal_types_menu_btn2 = XmCreatePushButton
    (fractal_types_menu_pane, "fractal_types_menu_btn2", args, n);

  XtAddCallback(fractal_types_menu_btn2, XmNactivateCallback,
    edit_current_parms_cb, (XtPointer) NULL);

  XtManageChild(fractal_types_menu_btn2);

  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  fractal_types_submenu_pane = XmCreatePulldownMenu
   (fractal_types_button, "fractal_types_submenu_pane", args, n);

  /* get the tear-off control */

  if((fractal_types_submenu_toc = XmGetTearOffControl
    (fractal_types_submenu_pane)) != NULL)
    {
    XtVaSetValues(fractal_types_submenu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  n = 0;
  str = XmStringCreateLocalized("Parameter forwarding");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNsubMenuId, fractal_types_submenu_pane); n++;

  fractal_types_menu_btn3 = XmCreateCascadeButton
    (fractal_types_menu_pane, "fractal_types_menu_btn3", args, n);

  XtManageChild(fractal_types_menu_btn3);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("On");
  XtSetArg(args[n], XmNlabelString, str); n++;
  fractal_types_submenu_btn1 = XmCreateToggleButton
    (fractal_types_submenu_pane, "fractal_types_submenu_btn1", args, n);

  XtAddCallback(fractal_types_submenu_btn1, XmNvalueChangedCallback,
    toggle_parameter_forwarding, (XtPointer) "1");

  XtManageChild(fractal_types_submenu_btn1);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Off");
  XtSetArg(args[n], XmNlabelString, str); n++;
  fractal_types_submenu_btn2 = XmCreateToggleButton
    (fractal_types_submenu_pane, "fractal_types_submenu_btn2", args, n);

  XtAddCallback(fractal_types_submenu_btn2, XmNvalueChangedCallback,
    toggle_parameter_forwarding, (XtPointer) "0");

  XtManageChild(fractal_types_submenu_btn2);
  XmStringFree(str);

  if(forwarding == 1)
    {
    XtVaSetValues(fractal_types_submenu_btn1, XmNset, True, NULL);
    XtVaSetValues(fractal_types_submenu_btn2, XmNset, False, NULL);
    }
  else
    {
    XtVaSetValues(fractal_types_submenu_btn1, XmNset, False, NULL);
    XtVaSetValues(fractal_types_submenu_btn2, XmNset, True, NULL);
    }
}

/*ARGSUSED*/
static void select_fractal_type_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  create_type_selection_dialog(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void edit_current_parms_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  select_type_ok_cb(main_window, (XtPointer) "1", NULL);
}

/*ARGSUSED*/
static void toggle_parameter_forwarding(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int which = atoi((char *)client_data);

  if(which == 1)
    {
    forwarding = 1;
    XtVaSetValues(fractal_types_submenu_btn1, XmNset, True, NULL);
    XtVaSetValues(fractal_types_submenu_btn2, XmNset, False, NULL);
    }
  else
    {
    forwarding = 0;
    XtVaSetValues(fractal_types_submenu_btn1, XmNset, False, NULL);
    XtVaSetValues(fractal_types_submenu_btn2, XmNset, True, NULL);
    }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: fractal_types.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.9  1995/05/03  04:54:20  darryl
 * Re-arrange main menu bar, rename some menus, move others.
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
